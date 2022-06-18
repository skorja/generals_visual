#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    virtual void paintEvent(QPaintEvent *event);

    virtual void resizeEvent(QResizeEvent* event);
    QTimer *tmr;

private slots:
    void on_horizontalSlider_valueChanged(int value);
    void updateTime();

    void on_actionSkip_empty_turns_changed();

    void on_pushButton_clicked();

    void on_actionFull_turn_changed();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
