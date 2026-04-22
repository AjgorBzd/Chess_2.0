#include "mainwindow.h"
#include "ChessGame.h"
#include "GameController.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow view;
    ChessGame model;
    GameController controller(&model, &view);

    view.show();
    return QCoreApplication::exec();
}
