#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "ChessEnums.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

signals:
    void requestPlayPlayer();
    void requestPlayComputer();

private slots:
    void on_closeButton_clicked();
    void on_btn_playPlayer_clicked();
    void on_btn_playComputer_clicked();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void showGamePage();
    void setSquare(int row, int col, PieceColor color, PieceType type);
    void clearHistoryUI();
    void drawCoordinates(PieceColor perspective);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::MainWindow *ui;
    void resizeBoard();
    QLabel* rankLabels[8]; // Numbers 1-8 (Rows)
    QLabel* fileLabels[8]; // Letters a-h (Columns)
};
#endif // MAINWINDOW_H
