#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void displayMessage(const QString &msg);

signals:
    void testClickDetected(int x, int y);

private slots:
    void on_testButton_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
