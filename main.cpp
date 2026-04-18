#include "mainwindow.h"
#include "ChessModel.h"
#include "GameController.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow view;
    ChessModel model;
    GameController controller(&model, &view);

    view.show();
    return QCoreApplication::exec();
}
