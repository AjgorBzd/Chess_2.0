#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_testButton_clicked()
{
    qDebug() << "1. VIEW: Button clicked. Emitting signal to Controller...";
    // We will pretend the user clicked grid coordinate (3, 4)
    emit testClickDetected(3, 4);
}

void MainWindow::displayMessage(const QString &msg)
{
    qDebug() << "4. VIEW: Received final message:" << msg;
    ui->testButton->setText("Test Passed!"); // Visually change the button text
}