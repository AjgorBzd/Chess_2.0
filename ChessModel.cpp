#include "ChessModel.h"
#include <QDebug>

ChessModel::ChessModel(QObject *parent)
    : QObject{parent}
{

}

void ChessModel::processClick(int x, int y)
{
    qDebug() << "3. MODEL: Calculating stuff for data:" << x << "," << y;

    QString result = QString("Performing some processing on data %1 and %2. Returned %3.").arg(x).arg(y).arg(5);

    // Broadcast the result
    emit modelUpdated(result);
}