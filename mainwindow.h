#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "ChessEnums.h"
#include "GameSettings.h"

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

    void requestSettingsOpen(bool isMidGame);
    void settingsSaved(GameSettings newSettings);
    void squareClicked(int row, int col);
    void requestUndoMove();

private slots:
    void on_closeButton_clicked();
    void on_btn_closeGame_clicked();

    void on_btn_playPlayer_clicked();
    void on_btn_playComputer_clicked();

    void on_btn_settings_clicked();

    void on_btn_undo_clicked();
    void on_btn_gameSettings_clicked();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void showGamePage();
    void setSquare(int row, int col, PieceColor color, PieceType type);
    void clearHistoryUI();
    void drawCoordinates(PieceColor perspective);

    void openSettingsDialog(const GameSettings& currentSettings, bool isMidGame);
    void applySettingsToUI(const GameSettings& settings);

    void pickUpPiece(int row, int col, PieceColor color, PieceType type);
    void dropPiece();

    void highlightMoves(const std::vector<LegalMove>& moves);
    void clearHighlights();
    void highlightCheck(const CheckInfo& info);

    void updateHistory(const std::vector<MoveRecord>& history);
    void updateTimers(int p1Seconds, int p2Seconds);
    void updateCaptures(const std::vector<PieceType>& whiteCaps, const std::vector<PieceType>& blackCaps, int p1Adv, int p2Adv);
    void RestartUI(int p1Time, int p2Time);

    int mapFlippedCoord(int coord) const { return m_isFlipped ? 7 - coord : coord; }
    void setFlipped(bool flipped);

    PieceType showPromotionDialog(PieceColor color, int logicalRow, int logicalCol);

protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::MainWindow *ui;
    void resizeBoard();
    QLabel* rankLabels[8]; // Numbers 1-8 (Rows)
    QLabel* fileLabels[8]; // Letters a-h (Columns)
    QLabel* m_floatingPiece;
    bool m_isFlipped = false;
};
#endif // MAINWINDOW_H
