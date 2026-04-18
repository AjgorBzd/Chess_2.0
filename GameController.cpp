#include "GameController.h"
#include "ChessModel.h"
#include "mainwindow.h"

GameController::GameController(ChessModel *model, MainWindow *view, QObject *parent)
    : QObject{parent}, m_model(model), m_view(view)
{
    connect(m_view, &MainWindow::testClickDetected, this, &GameController::handleViewClick);
    connect(m_model, &ChessModel::modelUpdated, this, &GameController::handleModelMessage);
}

void GameController::handleViewClick(int x, int y)
{
    qDebug() << "2. CONTROLLER: Heard View click. Telling Model to calculate...";
    m_model->processClick(x, y);
}

void GameController::handleModelMessage(const QString &msg)
{
    qDebug() << "3.5 CONTROLLER: Heard Model update. Pushing data to View...";
    m_view->displayMessage(msg);
}