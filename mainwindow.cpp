#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "SettingsDialog.h"
#include <QTimer>
#include <QMouseEvent>
#include <QString>
#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

namespace TileStyles {
inline const QString Base = "border: none; border-radius: 0px; font-weight: bold; font-size: 32px; ";

inline const QString WhiteNormal = Base + "background-color: #e0c08b; color: black;";
inline const QString BlackNormal = Base + "background-color: #555555; color: white;";

inline const QString WhiteBlue   = Base + "background-color: #8bb5ff; color: black;";
inline const QString BlackBlue   = Base + "background-color: #55559c; color: white;";

inline const QString WhiteRed    = Base + "background-color: #fb8b8b; color: black;";
inline const QString BlackRed    = Base + "background-color: #9c5555; color: white;";

inline const QString WhiteGreen  = Base + "background-color: #a5d68b; color: black;";
inline const QString BlackGreen  = Base + "background-color: #559c55; color: white;";

inline bool isWhiteSquare(int row, int col) {
    return (row + col) % 2 == 0;
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    m_floatingPiece = new QLabel(this);
    m_floatingPiece->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_floatingPiece->hide();

    qApp->installEventFilter(this);

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {

            QPushButton *tile = new QPushButton(this);

            tile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

            QFont pieceFont("Arial", 36, QFont::Bold);
            tile->setFont(pieceFont);

            bool isLightSquare = (row + col) % 2 == 0;

            QGridLayout *btnLayout = new QGridLayout(tile);
            btnLayout->setContentsMargins(6, 6, 6, 6);

            if (col == 0) {
                rankLabels[row] = new QLabel(tile);
                rankLabels[row]->setStyleSheet(isLightSquare ? "color: #555555; background: transparent;" : "color: #e0c08b; background: transparent;");

                QFont coordFont("Arial", 16, QFont::Bold);
                rankLabels[row]->setFont(coordFont);

                btnLayout->addWidget(rankLabels[row], 0, 0, Qt::AlignTop | Qt::AlignLeft);
            }

            if (row == 7) {
                fileLabels[col] = new QLabel(tile);
                fileLabels[col]->setStyleSheet(isLightSquare ? "color: #555555; background: transparent;" : "color: #e0c08b; background: transparent;");

                QFont coordFont("Arial", 16, QFont::Bold);
                fileLabels[col]->setFont(coordFont);

                btnLayout->addWidget(fileLabels[col], 1, 1, Qt::AlignBottom | Qt::AlignRight);
            }

            tile->setStyleSheet(isLightSquare ? TileStyles::WhiteNormal : TileStyles::BlackNormal);

            connect(tile, &QPushButton::clicked, this, [this, row, col]() {
                emit squareClicked(mapFlippedCoord(row), mapFlippedCoord(col));
            });

            ui->gridLayout_Board->addWidget(tile, row, col);
        }
    }

    ui->gridLayout_Board->setAlignment(Qt::AlignCenter);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_closeButton_clicked()
{
    this->close();
}

void MainWindow::on_btn_closeGame_clicked()
{
    ui->pageManager->setCurrentIndex(0);
}

void MainWindow::on_btn_playPlayer_clicked()
{
    ui->pageManager->setCurrentIndex(1);
    resizeBoard();
    clearHighlights();
    emit requestPlayPlayer();

}

void MainWindow::on_btn_playComputer_clicked()
{
    ui->pageManager->setCurrentIndex(1);
    resizeBoard();
}

void MainWindow::drawCoordinates(PieceColor perspective)
{
    // If White is on the bottom, rows go 8 down to 1, cols go 'a' to 'h'
    if (perspective == PieceColor::White) {
        for (int i = 0; i < 8; ++i) {
            if (rankLabels[i]) rankLabels[i]->setText(QString::number(8 - i));
            if (fileLabels[i]) fileLabels[i]->setText(QString(QChar('a' + i)));
        }
    }
    // If Black is on the bottom, rows go 1 up to 8, cols go 'h' down to 'a'
    else {
        for (int i = 0; i < 8; ++i) {
            if (rankLabels[i]) rankLabels[i]->setText(QString::number(1 + i));
            if (fileLabels[i]) fileLabels[i]->setText(QString(QChar('h' - i)));
        }
    }
}

void MainWindow::resizeBoard()
{
    if (ui->pageManager->currentIndex() != 1) return;

    int w = ui->boardContainer->width();
    int h = ui->boardContainer->height();
    int shortestSide = std::min(w, h);

    int tileSize = (shortestSide - 16) / 8;

    if (tileSize < 10) return;

    for (int i = 0; i < ui->gridLayout_Board->count(); ++i) {
        QPushButton *tile = qobject_cast<QPushButton*>(ui->gridLayout_Board->itemAt(i)->widget());

        if (tile) {
            tile->setFixedSize(tileSize, tileSize);
            tile->setIconSize(QSize(tileSize, tileSize));
        }
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    resizeBoard();
}

void MainWindow::showGamePage()
{
    ui->pageManager->setCurrentIndex(1);
    QTimer::singleShot(10, this, &MainWindow::resizeBoard);
}

void MainWindow::setSquare(int row, int col, PieceColor color, PieceType type)
{
    QString path = "";
    if (type != PieceType::Empty && color != PieceColor::None) {
        path = ":/images/";
        path += (color == PieceColor::White) ? "white_" : "black_";
        switch (type) {
        case PieceType::Pawn:   path += "pawn.png"; break;
        case PieceType::Knight: path += "knight.png"; break;
        case PieceType::Bishop: path += "bishop.png"; break;
        case PieceType::Rook:   path += "rook.png"; break;
        case PieceType::Queen:  path += "queen.png"; break;
        case PieceType::King:   path += "king.png"; break;
        default: break;
        }
    }

    // Map logical to visual layout position
    QLayoutItem* item = ui->gridLayout_Board->itemAtPosition(mapFlippedCoord(row), mapFlippedCoord(col));
    if (item && item->widget()) {
        QPushButton* tile = qobject_cast<QPushButton*>(item->widget());
        if (tile) {
            tile->setIcon(QIcon(path));
        }
    }
}

void MainWindow::clearHistoryUI()
{
    ui->listWidget_History->clear();
}

void MainWindow::on_btn_settings_clicked() {
    emit requestSettingsOpen(false); // False = Not mid-game
}

void MainWindow::on_btn_gameSettings_clicked() {
    emit requestSettingsOpen(true);  // True = Mid-game! Lock the timers!
}

// --- The Controller Commands ---
void MainWindow::openSettingsDialog(const GameSettings& currentSettings, bool isMidGame)
{
    // Create the dialog, passing in the locked state and current settings
    SettingsDialog dialog(currentSettings, isMidGame, this);

    // exec() freezes the main window until the dialog is closed
    if (dialog.exec() == QDialog::Accepted) {
        // If they clicked Save, send the updated struct to the Controller!
        emit settingsSaved(dialog.getSettings());
    }
}

QPixmap makeSquareAvatar(const QString& imagePath, int size) {
    QPixmap original(imagePath);
    if (original.isNull()) return QPixmap(); // Return empty if not found

    int shortest = std::min(original.width(), original.height());
    int x = (original.width() - shortest) / 2;
    int y = (original.height() - shortest) / 2;

    // Crop to center square, then scale smoothly
    QPixmap square = original.copy(x, y, shortest, shortest);
    return square.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void MainWindow::applySettingsToUI(const GameSettings& settings)
{
    ui->p1Name->setFixedWidth(150);
    ui->p2Name->setFixedWidth(150);

    QString p1Full = QString::fromStdString(settings.p1Name);
    QString p2Full = QString::fromStdString(settings.p2Name);

    QFontMetrics metrics1(ui->p1Name->font());
    QString p1Elided = metrics1.elidedText(p1Full, Qt::ElideRight, 150);

    QFontMetrics metrics2(ui->p2Name->font());
    QString p2Elided = metrics2.elidedText(p2Full, Qt::ElideRight, 150);

    ui->p1Name->setText(p1Elided);
    ui->p1Name->setToolTip(p1Full);

    ui->p2Name->setText(p2Elided);
    ui->p2Name->setToolTip(p2Full);

    ui->p1Icon->setPixmap(makeSquareAvatar(QString::fromStdString(settings.p1AvatarPath), 80));
    ui->p2Icon->setPixmap(makeSquareAvatar(QString::fromStdString(settings.p2AvatarPath), 80));

    // Format the timers (e.g., 300 seconds -> "05:00")
    int p1Mins = settings.p1BaseTimeSeconds / 60;
    int p1Secs = settings.p1BaseTimeSeconds % 60;
    ui->p1Timer->setText(QString("%1:%2").arg(p1Mins, 2, 10, QChar('0')).arg(p1Secs, 2, 10, QChar('0')));

    int p2Mins = settings.p2BaseTimeSeconds / 60;
    int p2Secs = settings.p2BaseTimeSeconds % 60;
    ui->p2Timer->setText(QString("%1:%2").arg(p2Mins, 2, 10, QChar('0')).arg(p2Secs, 2, 10, QChar('0')));
}

QString getImagePath(PieceColor color, PieceType type) {
    if (type == PieceType::Empty || color == PieceColor::None) return "";
    QString path = ":/images/";
    path += (color == PieceColor::White) ? "white_" : "black_";
    switch (type) {
    case PieceType::Pawn:   path += "pawn.png"; break;
    case PieceType::Knight: path += "knight.png"; break;
    case PieceType::Bishop: path += "bishop.png"; break;
    case PieceType::Rook:   path += "rook.png"; break;
    case PieceType::Queen:  path += "queen.png"; break;
    case PieceType::King:   path += "king.png"; break;
    default: return "";
    }
    return path;
}

void MainWindow::pickUpPiece(int row, int col, PieceColor color, PieceType type)
{
    // 1. Erase the icon from the actual button so it looks like we picked it up
    setSquare(row, col, PieceColor::None, PieceType::Empty);

    // 2. Set the floating label's image
    QPixmap pixmap(getImagePath(color, type));

    // Scale it to match your current tile size
    int tileSize = ui->gridLayout_Board->itemAtPosition(0,0)->widget()->width();
    m_floatingPiece->setPixmap(pixmap.scaled(tileSize, tileSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_floatingPiece->setFixedSize(tileSize, tileSize);

    // 3. Move it to the mouse instantly and show it
    QPoint mousePos = this->mapFromGlobal(QCursor::pos());
    m_floatingPiece->move(mousePos.x() - (tileSize / 2), mousePos.y() - (tileSize / 2));
    m_floatingPiece->show();
    m_floatingPiece->raise(); // Ensure it's drawn on top of everything
}

void MainWindow::dropPiece()
{
    m_floatingPiece->hide();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    // If the mouse is moving AND our floating piece is visible...
    if (event->type() == QEvent::MouseMove && !m_floatingPiece->isHidden()) {

        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

        // Translate the coordinate to relative MainWindow space
        QPoint globalPos = mouseEvent->globalPosition().toPoint();
        QPoint localPos = this->mapFromGlobal(globalPos);

        // Center the piece exactly on the cursor
        int offset = m_floatingPiece->width() / 2;
        m_floatingPiece->move(localPos.x() - offset, localPos.y() - offset);
    }

    // Let the standard event processing continue
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::highlightMoves(const std::vector<LegalMove>& moves) {
    for (const auto& move : moves) {
        QLayoutItem* item = ui->gridLayout_Board->itemAtPosition(mapFlippedCoord(move.row), mapFlippedCoord(move.col));
        if (item && item->widget()) {
            QPushButton* tile = qobject_cast<QPushButton*>(item->widget());
            if (tile) {
                bool isWhite = TileStyles::isWhiteSquare(move.row, move.col);
                if (move.isSpecial) {
                    tile->setStyleSheet(isWhite ? TileStyles::WhiteGreen : TileStyles::BlackGreen);
                } else if (move.isCapture) {
                    tile->setStyleSheet(isWhite ? TileStyles::WhiteRed : TileStyles::BlackRed);
                } else {
                    tile->setStyleSheet(isWhite ? TileStyles::WhiteBlue : TileStyles::BlackBlue);
                }
            }
        }
    }
}

void MainWindow::clearHighlights() {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            QLayoutItem* item = ui->gridLayout_Board->itemAtPosition(mapFlippedCoord(r), mapFlippedCoord(c));
            if (item && item->widget()) {
                QPushButton* tile = qobject_cast<QPushButton*>(item->widget());
                if (tile) {
                    tile->setStyleSheet(TileStyles::isWhiteSquare(r, c) ? TileStyles::WhiteNormal : TileStyles::BlackNormal);
                }
            }
        }
    }
}

void MainWindow::highlightCheck(const CheckInfo& info) {
    if (!info.inCheck) return;

    auto paintRed = [&](int r, int c) {
        // Map logical to visual layout position
        QLayoutItem* item = ui->gridLayout_Board->itemAtPosition(mapFlippedCoord(r), mapFlippedCoord(c));
        if (item && item->widget()) {
            QPushButton* tile = qobject_cast<QPushButton*>(item->widget());
            if (tile) {
                bool isWhite = TileStyles::isWhiteSquare(r, c);
                tile->setStyleSheet(isWhite ? TileStyles::WhiteRed : TileStyles::BlackRed);
            }
        }
    };

    paintRed(info.kingPos.row, info.kingPos.col);
    for (const auto& attacker : info.attackers) {
        paintRed(attacker.row, attacker.col);
    }
}

void MainWindow::updateHistory(const std::vector<MoveRecord>& history) {
    ui->listWidget_History->clear();

    auto formatMove = [](const MoveRecord& m) {
        QString res = "";

        if (m.isShortCastling) {
            res = "O-O";
        }
        else if (m.isLongCastling) {
            res = "O-O-O";
        }
        else
        {
            if (m.pieceMoved == PieceType::Knight) res += "N";
            else if (m.pieceMoved == PieceType::Bishop) res += "B";
            else if (m.pieceMoved == PieceType::Rook) res += "R";
            else if (m.pieceMoved == PieceType::Queen) res += "Q";
            else if (m.pieceMoved == PieceType::King) res += "K";

            if (m.pieceCaptured != PieceType::Empty) {
                if (m.pieceMoved == PieceType::Pawn) {
                    res += QChar('a' + m.fromCol);
                }
                res += "x";
            }
            res += QChar('a' + m.toCol);
            res += QString::number(8 - m.toRow);
        }

        if (m.isPromotionToQueen) res += "=Q";
        else if (m.isPromotionToRook) res += "=R";
        else if (m.isPromotionToBishop) res += "=B";
        else if (m.isPromotionToKnight) res += "=N";

        if (m.isCheckmate) {
            res += "#";
        } else if (m.isCheck) {
            res += "+";
        }

        return res;
    };

    int moveNum = 1;
    QString currentRow = "";

    for (size_t i = 0; i < history.size(); ++i) {
        if (i % 2 == 0) {
            currentRow = QString::number(moveNum++) + ". " + formatMove(history[i]);

            if (i == history.size() - 1) {
                ui->listWidget_History->addItem(currentRow);
            }
        } else {
            currentRow += "   " + formatMove(history[i]);
            ui->listWidget_History->addItem(currentRow);
        }
    }

    ui->listWidget_History->scrollToBottom();
}

void MainWindow::updateTimers(int p1Seconds, int p2Seconds) {
    auto formatTime = [](int totalSecs) {
        return QString("%1:%2").arg(totalSecs / 60, 2, 10, QChar('0'))
        .arg(totalSecs % 60, 2, 10, QChar('0'));
    };

    ui->p1Timer->setText(formatTime(p1Seconds));
    ui->p2Timer->setText(formatTime(p2Seconds));
}

void MainWindow::updateCaptures(const std::vector<PieceType>& whiteCaps, const std::vector<PieceType>& blackCaps, int p1Adv, int p2Adv) {

    auto buildHtml = [](std::vector<PieceType> pieces, const QString& colorPrefix, int adv) {

        std::sort(pieces.begin(), pieces.end(), [](PieceType a, PieceType b) {
            auto getRank = [](PieceType t) {
                if (t == PieceType::Pawn) return 1;
                if (t == PieceType::Knight) return 2;
                if (t == PieceType::Bishop) return 3;
                if (t == PieceType::Rook) return 4;
                if (t == PieceType::Queen) return 5;
                return 6;
            };
            return getRank(a) < getRank(b);
        });

        QString html = "";
        for (auto type : pieces) {
            QString name;
            switch(type) {
            case PieceType::Pawn:   name = "pawn"; break;
            case PieceType::Knight: name = "knight"; break;
            case PieceType::Bishop: name = "bishop"; break;
            case PieceType::Rook:   name = "rook"; break;
            case PieceType::Queen:  name = "queen"; break;
            default: break;
            }
            html += "<img src=':/images/" + colorPrefix + "_" + name + ".png' width='20' height='20'> ";
        }

        if (adv > 0) {
            html += "<span style='color: #a5d68b; font-weight: bold; font-size: 16px;'> +" + QString::number(adv) + "</span>";
        }
        return html;
    };

    ui->p1Captured->setText(buildHtml(whiteCaps, "black", p1Adv));
    ui->p2Captured->setText(buildHtml(blackCaps, "white", p2Adv));
}

void MainWindow::RestartUI(int p1Time, int p2Time) {
    clearHistoryUI();

    ui->p1Captured->clear();
    ui->p2Captured->clear();

    clearHighlights();
    updateTimers(p1Time, p2Time);
    setFlipped(false);
}

void MainWindow::on_btn_undo_clicked() {
    emit requestUndoMove();
}

void MainWindow::setFlipped(bool flipped) {
    if (m_isFlipped != flipped) {
        m_isFlipped = flipped;
        drawCoordinates(m_isFlipped ? PieceColor::Black : PieceColor::White);
    }
}

PieceType MainWindow::showPromotionDialog(PieceColor color, int logicalRow, int logicalCol) {
    QDialog dialog(this);
    dialog.setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    dialog.setStyleSheet("QDialog { background-color: #2b2b2b; border: 2px solid #555555; border-radius: 8px; }");

    QHBoxLayout* layout = new QHBoxLayout(&dialog);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(10);

    PieceType selectedPiece = PieceType::Empty;
    QString prefix = (color == PieceColor::White) ? "white_" : "black_";

    auto addButton = [&](PieceType type, const QString& iconName) {
        QPushButton* btn = new QPushButton(&dialog);
        btn->setIcon(QIcon(":/images/" + prefix + iconName + ".png"));
        btn->setIconSize(QSize(45, 45));
        btn->setFixedSize(60, 60);
        btn->setStyleSheet("QPushButton { background-color: #404040; border: none; border-radius: 5px; } "
                           "QPushButton:hover { background-color: #55559c; }");

        connect(btn, &QPushButton::clicked, [&dialog, &selectedPiece, type]() {
            selectedPiece = type;
            dialog.accept();
        });
        layout->addWidget(btn);
    };

    addButton(PieceType::Queen, "queen");
    addButton(PieceType::Rook, "rook");
    addButton(PieceType::Bishop, "bishop");
    addButton(PieceType::Knight, "knight");


    QPushButton* cancelBtn = new QPushButton("X", &dialog);
    cancelBtn->setFixedSize(40, 60);
    cancelBtn->setStyleSheet("QPushButton { color: #fb8b8b; font-weight: bold; font-size: 24px; border: none; background: transparent; } "
                             "QPushButton:hover { color: #ff0000; }");
    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);
    layout->addWidget(cancelBtn);

    QWidget* tile = ui->gridLayout_Board->itemAtPosition(mapFlippedCoord(logicalRow), mapFlippedCoord(logicalCol))->widget();
    if (tile) {
        dialog.adjustSize();

        QPoint globalPos = tile->mapToGlobal(QPoint(0, 0));
        int targetX = globalPos.x() - (dialog.width() / 4);
        int targetY = globalPos.y() - 30;

        QRect windowRect = this->geometry();

        if (targetX < windowRect.left()) targetX = windowRect.left() + 10;
        if (targetX + dialog.width() > windowRect.right()) targetX = windowRect.right() - dialog.width() - 10;

        if (targetY < windowRect.top()) targetY = windowRect.top() + 10;
        if (targetY + dialog.height() > windowRect.bottom()) targetY = windowRect.bottom() - dialog.height() - 10;

        dialog.move(targetX, targetY);
    }

    dialog.exec();
    return selectedPiece;
}

void MainWindow::showGameOverDialog(GameState state, const GameSettings& settings) {
    QDialog dialog(this);
    dialog.setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    dialog.setStyleSheet("QDialog { background-color: #2b2b2b; border: 2px solid #555555; border-radius: 8px; } "
                         "QLabel { color: white; font-family: 'Segoe UI'; }");

    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    QLabel* titleLbl = new QLabel(&dialog);
    titleLbl->setAlignment(Qt::AlignCenter);
    titleLbl->setStyleSheet("font-size: 28px; font-weight: bold; color: #e0c08b;");

    QLabel* scoreLbl = new QLabel(&dialog);
    scoreLbl->setAlignment(Qt::AlignCenter);
    scoreLbl->setStyleSheet("font-size: 42px; font-weight: bold;");

    QString p1Name = QString::fromStdString(settings.p1Name);
    QString p2Name = QString::fromStdString(settings.p2Name);

    if (state == GameState::WhiteWins) {
        titleLbl->setText("Szach Mat! Wygrywa " + p1Name);
        scoreLbl->setText("1 - 0");
    } else if (state == GameState::BlackWins) {
        titleLbl->setText("Szach Mat! Wygrywa " + p2Name);
        scoreLbl->setText("0 - 1");
    } else {
        titleLbl->setText("Koniec Gry");
        scoreLbl->setText("½ - ½");
    }
    mainLayout->addWidget(titleLbl);
    mainLayout->addWidget(scoreLbl);

    QHBoxLayout* playersLayout = new QHBoxLayout();

    auto createPlayerWidget = [&](const QString& name, const QString& avatarPath) {
        QVBoxLayout* pLayout = new QVBoxLayout();
        QLabel* avLbl = new QLabel(&dialog);
        avLbl->setFixedSize(80, 80);
        avLbl->setPixmap(makeSquareAvatar(avatarPath, 80));
        avLbl->setAlignment(Qt::AlignCenter);

        QLabel* nameLbl = new QLabel(name, &dialog);
        nameLbl->setAlignment(Qt::AlignCenter);
        nameLbl->setStyleSheet("font-size: 18px; font-weight: bold;");

        pLayout->addWidget(avLbl);
        pLayout->addWidget(nameLbl);
        pLayout->setAlignment(Qt::AlignCenter);
        return pLayout;
    };

    playersLayout->addLayout(createPlayerWidget(p1Name, QString::fromStdString(settings.p1AvatarPath)));

    QLabel* vsLbl = new QLabel("VS", &dialog);
    vsLbl->setAlignment(Qt::AlignCenter);
    vsLbl->setStyleSheet("font-size: 24px; font-weight: bold; color: #777777; margin: 0px 15px;");
    playersLayout->addWidget(vsLbl);

    playersLayout->addLayout(createPlayerWidget(p2Name, QString::fromStdString(settings.p2AvatarPath)));

    mainLayout->addLayout(playersLayout);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(15);
    btnLayout->setContentsMargins(0, 20, 0, 0);

    QPushButton* btnMenu = new QPushButton("Wróć do menu", &dialog);
    QPushButton* btnRematch = new QPushButton("Zagraj ponownie", &dialog);

    QString btnStyle = "QPushButton { background-color: #404040; border: none; border-radius: 5px; padding: 12px 20px; font-size: 16px; font-weight: bold; color: white;} "
                       "QPushButton:hover { background-color: #55559c; }";
    btnRematch->setStyleSheet(btnStyle);
    btnMenu->setStyleSheet(btnStyle);

    connect(btnRematch, &QPushButton::clicked, this, [&]() {
        dialog.accept();
        emit requestPlayPlayer();
    });

    connect(btnMenu, &QPushButton::clicked, this, [&]() {
        dialog.reject();
        on_btn_closeGame_clicked();
    });

    btnLayout->addWidget(btnMenu);
    btnLayout->addWidget(btnRematch);
    mainLayout->addLayout(btnLayout);

    dialog.exec();
}