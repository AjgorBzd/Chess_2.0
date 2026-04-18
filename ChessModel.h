#ifndef CHESSMODEL_H
#define CHESSMODEL_H

#include <QObject>

class ChessModel : public QObject
{
    Q_OBJECT
public:
    explicit ChessModel(QObject *parent = nullptr);
    void processClick(int x, int y);

signals:
    void boardUpdated();
    void modelUpdated(const QString &result);
};

#endif // CHESSMODEL_H