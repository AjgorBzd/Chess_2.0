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

    board.movePiece(fromRow, fromCol, toRow, toCol);

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
    return MoveValidator::getLegalMoves(row, col, board);
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

