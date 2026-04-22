#pragma once
#include <QObject>

class ChessGame;
class MainWindow;

class GameController : public QObject
{
    Q_OBJECT
public:
    GameController(ChessGame *model, MainWindow *view, QObject *parent = nullptr);

public slots:
    void handlePlayPlayerRequest();

private:
    ChessGame *m_model;
    MainWindow *m_view;
    void syncBoardToView();
};