#include "ChessGame.h"

ChessGame::ChessGame(QObject *parent)
    : QObject{parent}
{}

void ChessGame::startNewGame(const GameSettings& settings) {
    board.setupStandardBoard();
    currentTurn = PieceColor::White;
    moveHistory.clear();
    gameStarted = false;

    playerWhite.clearCapturedPieces();
    playerBlack.clearCapturedPieces();

    applySettings(settings);
}

PieceType ChessGame::getPieceTypeAt(int row, int col) const {
    return board.getPieceAt(row, col)->getType();
}

PieceColor ChessGame::getPieceColorAt(int row, int col) const {
    return board.getPieceAt(row, col)->getColor();
}

void ChessGame::switchTurn() {
    currentTurn = (currentTurn == PieceColor::White) ? PieceColor::Black : PieceColor::White;
}

bool ChessGame::attemptMove(int fromRow, int fromCol, int toRow, int toCol) {
    auto legalMoves = getLegalMovesForPiece(fromRow, fromCol);

    bool isLegal = false;
    for (const auto& move : legalMoves) {
        if (move.row == toRow && move.col == toCol) { isLegal = true; break; }
    }
    if (!isLegal) return false;

    PieceType movingPiece = getPieceTypeAt(fromRow, fromCol);
    PieceType captured = getPieceTypeAt(toRow, toCol);

    bool isEnPassant = (movingPiece == PieceType::Pawn && std::abs(fromCol - toCol) == 1 && captured == PieceType::Empty);
    bool isShortCastle = (movingPiece == PieceType::King && toCol - fromCol == 2);
    bool isLongCastle = (movingPiece == PieceType::King && fromCol - toCol == 2);

    if (isEnPassant) {
        captured = PieceType::Pawn;
        board.setPieceAt(fromRow, toCol, std::make_shared<EmptyPiece>());
    }

    if (captured != PieceType::Empty) {
        getPlayer(currentTurn).addCapturedPiece(captured);
    }

    MoveRecord record;
    record.fromRow = fromRow;
    record.fromCol = fromCol;
    record.toRow = toRow;
    record.toCol = toCol;
    record.pieceMoved = movingPiece;
    record.playerColor = currentTurn;
    record.pieceCaptured = captured;
    record.isFirstMove = !board.getPieceAt(fromRow, fromCol)->hasMoved();
    record.isEnPassant = isEnPassant;
    record.isShortCastling = isShortCastle;
    record.isLongCastling = isLongCastle;
    record.p1Time = playerWhite.getTimeLeft();
    record.p2Time = playerBlack.getTimeLeft();

    board.movePiece(fromRow, fromCol, toRow, toCol);

    if (isShortCastle) {
        board.movePiece(fromRow, 7, toRow, 5);
    } else if (isLongCastle) {
        board.movePiece(fromRow, 0, toRow, 3);
    }

    PieceColor enemyColor = (currentTurn == PieceColor::White) ? PieceColor::Black : PieceColor::White;
    CheckInfo enemyCheck = MoveValidator::getCheckInfo(enemyColor, board);
    record.isCheck = enemyCheck.inCheck;

    moveHistory.push_back(record);
    currentTurn = enemyColor;

    return true;
}

bool ChessGame::canPickUp(int row, int col) const {
    PieceType type = getPieceTypeAt(row, col);
    PieceColor color = getPieceColorAt(row, col);

    // You can only pick it up if it's not empty, AND it belongs to the current player
    return (type != PieceType::Empty && color == currentTurn);
}

std::vector<LegalMove> ChessGame::getLegalMovesForPiece(int row, int col) {
    if (getPieceColorAt(row, col) != currentTurn) {
        return {};
    }
    return MoveValidator::getLegalMoves(row, col, board, moveHistory);
}

CheckInfo ChessGame::getCurrentCheckInfo() const {
    return MoveValidator::getCheckInfo(currentTurn, board);
}

void ChessGame::applySettings(const GameSettings& settings) {
    playerWhite.setName(settings.p1Name);
    playerWhite.setTimeLeft(settings.p1BaseTimeSeconds);

    playerBlack.setName(settings.p2Name);
    playerBlack.setTimeLeft(settings.p2BaseTimeSeconds);
}

bool ChessGame::undoLastMove() {
    if (moveHistory.empty()) return false;

    MoveRecord lastMove = moveHistory.back();
    moveHistory.pop_back();

    board.setPieceAt(lastMove.fromRow, lastMove.fromCol, board.getPieceAt(lastMove.toRow, lastMove.toCol));

    if (lastMove.isFirstMove) {
        board.getPieceAt(lastMove.fromRow, lastMove.fromCol)->setMoved(false);
    }

    if (lastMove.isShortCastling) {
        board.setPieceAt(lastMove.fromRow, 7, board.getPieceAt(lastMove.toRow, 5));
        board.setPieceAt(lastMove.toRow, 5, std::make_shared<EmptyPiece>());
        board.getPieceAt(lastMove.fromRow, 7)->setMoved(false);
    } else if (lastMove.isLongCastling) {
        board.setPieceAt(lastMove.fromRow, 0, board.getPieceAt(lastMove.toRow, 3));
        board.setPieceAt(lastMove.toRow, 3, std::make_shared<EmptyPiece>());
        board.getPieceAt(lastMove.fromRow, 0)->setMoved(false);
    }

    PieceColor enemyColor = (lastMove.playerColor == PieceColor::White) ? PieceColor::Black : PieceColor::White;

    std::shared_ptr<Piece> restoredPiece = std::make_shared<EmptyPiece>();

    if (lastMove.pieceCaptured != PieceType::Empty) {
        switch (lastMove.pieceCaptured) {
        case PieceType::Pawn: restoredPiece = std::make_shared<Pawn>(enemyColor); break;
        case PieceType::Knight: restoredPiece = std::make_shared<Knight>(enemyColor); break;
        case PieceType::Bishop: restoredPiece = std::make_shared<Bishop>(enemyColor); break;
        case PieceType::Rook: restoredPiece = std::make_shared<Rook>(enemyColor); restoredPiece->setMoved(true); break;
        case PieceType::Queen: restoredPiece = std::make_shared<Queen>(enemyColor); break;
        default: break;
        }
        getPlayer(lastMove.playerColor).popCapturedPiece();

        if (lastMove.isEnPassant) {
            board.setPieceAt(lastMove.fromRow, lastMove.toCol, restoredPiece);
            board.setPieceAt(lastMove.toRow, lastMove.toCol, std::make_shared<EmptyPiece>());
        } else {
            board.setPieceAt(lastMove.toRow, lastMove.toCol, restoredPiece);
        }
    } else if (!lastMove.isEnPassant) {
        board.setPieceAt(lastMove.toRow, lastMove.toCol, std::make_shared<EmptyPiece>());
    }

    playerWhite.setTimeLeft(lastMove.p1Time);
    playerBlack.setTimeLeft(lastMove.p2Time);

    currentTurn = lastMove.playerColor;

    return true;
}

