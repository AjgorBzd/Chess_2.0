#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "SettingsDialog.h"
#include <QTimer>
#include <QMouseEvent>

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
            btnLayout->setContentsMargins(6, 6, 6, 6); // Padding from the edge

            // Column 0 gets the Numbers (Ranks)
            if (col == 0) {
                rankLabels[row] = new QLabel(tile);
                // Dark squares get light text, light squares get dark text
                rankLabels[row]->setStyleSheet(isLightSquare ? "color: #555555; background: transparent;" : "color: #e0c08b; background: transparent;");

                QFont coordFont("Arial", 16, QFont::Bold);
                rankLabels[row]->setFont(coordFont);

                btnLayout->addWidget(rankLabels[row], 0, 0, Qt::AlignTop | Qt::AlignLeft);
            }

            // Row 7 gets the Letters (Files)
            if (row == 7) {
                fileLabels[col] = new QLabel(tile);
                fileLabels[col]->setStyleSheet(isLightSquare ? "color: #555555; background: transparent;" : "color: #e0c08b; background: transparent;");

                QFont coordFont("Arial", 16, QFont::Bold);
                fileLabels[col]->setFont(coordFont);

                // Put it in the bottom-right corner
                btnLayout->addWidget(fileLabels[col], 1, 1, Qt::AlignBottom | Qt::AlignRight);
            }

            if (isLightSquare) {
                tile->setStyleSheet("background-color: #e0c08b; border: none; border-radius: 0px; color: black;");
            } else {
                tile->setStyleSheet("background-color: #555555; border: none; border-radius: 0px; color: white;");
            }

            connect(tile, &QPushButton::clicked, this, [this, row, col]() {
                // This tiny invisible function remembers its exact row and col!
                emit squareClicked(row, col);
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

void MainWindow::on_btn_playPlayer_clicked()
{
    ui->pageManager->setCurrentIndex(1);
    resizeBoard();
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
    // A quick helper to get the right string path
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

    // Find the button and set the icon
    QLayoutItem* item = ui->gridLayout_Board->itemAtPosition(row, col);
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

void MainWindow::applySettingsToUI(const GameSettings& settings)
{
    // Update the player names on the board screen
    ui->p1Name->setText(QString::fromStdString(settings.p1Name));
    ui->p2Name->setText(QString::fromStdString(settings.p2Name));

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