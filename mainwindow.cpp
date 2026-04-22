#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Szachy");
    this->setWindowFlags(Qt::FramelessWindowHint);

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