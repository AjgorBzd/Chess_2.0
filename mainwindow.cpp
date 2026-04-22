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

            // Ensure tiles stretch equally to form perfect squares
            tile->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

            // Font setup for chess pieces (make them large and centered)
            QFont pieceFont("Arial", 36, QFont::Bold);
            tile->setFont(pieceFont);

            // Determine tile color (Alternating logic)
            bool isLightSquare = (row + col) % 2 == 0;

            if (isLightSquare) {
                // Your exact "Szachy" title golden color
                tile->setStyleSheet("background-color: #e0c08b; border: none; border-radius: 0px; color: black;");
            } else {
                // Dark gray color
                tile->setStyleSheet("background-color: #555555; border: none; border-radius: 0px; color: white;");
            }

            // --- QUICK TEST: SPAWN PAWNS ---
            // Row 1 is the starting line for Black Pawns
            if (row == 1) {
                tile->setIcon(QIcon(":/images/black_pawn.png")); // Use your exact file name
            }
            // Row 6 is the starting line for White Pawns
            if (row == 6) {
                tile->setIcon(QIcon(":/images/white_pawn.png")); // Use your exact file name
            }

            // Add to the grid layout at specific row and column
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
}

void MainWindow::on_btn_playComputer_clicked()
{
    ui->pageManager->setCurrentIndex(1);
    resizeBoard();
}

void MainWindow::resizeBoard()
{
    // Don't calculate anything if we aren't looking at the game board
    if (ui->pageManager->currentIndex() != 1) return;

    int w = ui->boardContainer->width();
    int h = ui->boardContainer->height();
    int shortestSide = std::min(w, h);

    int tileSize = (shortestSide - 16) / 8;

    // Safety check: Don't let the tiles crush themselves
    if (tileSize < 10) return;

    for (int i = 0; i < ui->gridLayout_Board->count(); ++i) {
        // We must cast it to a QPushButton to access the setIconSize property
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
    resizeBoard(); // Call our helper whenever the window resizes
}