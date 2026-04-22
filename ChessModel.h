#ifndef CHESSMODEL_H
#define CHESSMODEL_H

#include <QObject>

class ChessModel : public QObject
{
    Q_OBJECT
public:
    explicit ChessModel(QObject *parent = nullptr);

signals:
};

#endif // CHESSMODEL_H