#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>

class ChessModel;
class MainWindow;

class GameController : public QObject
{
    Q_OBJECT
public:
    GameController(ChessModel *model, MainWindow *view, QObject *parent = nullptr);

public slots:

private:
    ChessModel *m_model;
    MainWindow *m_view;
};

#endif // GAMECONTROLLER_H