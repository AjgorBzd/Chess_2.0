#include "GameController.h"
#include "ChessModel.h"
#include "mainwindow.h"

GameController::GameController(ChessModel *model, MainWindow *view, QObject *parent)
    : QObject{parent}, m_model(model), m_view(view)
{

}