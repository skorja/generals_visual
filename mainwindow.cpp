#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <unistd.h>
#include <QTimer>
#include "QFileDialog"
#include "fstream"
#include "cmath"
#include "algorithm"
#include "tuple"

int speed = 1;
bool pause_game = false;

int W = 1, H = 1, mx_x = 0, mx_y = 0, R = 5, dx = 150, dy = 80, MX_N = 10, MX_M = 10, size_font = 10;
int timer_interval = 1000;
QWidget *par;
QString fileName;
std::ifstream fin;
int N_player = 1;
bool skip = true;
bool game_not_stopped = true;
bool full_turn = false;
bool skip_step = false;
int skip_next_X_steps = 0;
int skip_next_X_turn = 0;
int turn_counter = 0;
Ui::MainWindow * uii;
QVector<QLabel *> players_score;
QVector<QPair<int, int>> score;
QVector<QVector<std::tuple<int, int, int>>> field; // type, player, value

QVector<QColor> colors {Qt::lightGray, Qt::red, Qt::blue, Qt::green, Qt::magenta};

std::tuple<int, int, int, int> realcoord(int x, int y)
{
    int x1 = R + W * y / MX_M;
    int y1 = dy + H * x / MX_N;
    int x2 = R + W * (y + 1) / MX_M;
    int y2 = dy + H * (x + 1) / MX_N;
    return {x1, y1, x2, y2};
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    uii = ui;
    ui->setupUi(this);
    ui->label_2->setText("3");
    par = parent;
    this->resize(600, 600);
    fileName = QFileDialog::getOpenFileName(this);
    QFile tmp(fileName);
    if (!QFileInfo::exists(fileName)) return;
    try {
        fin.open(fileName.toStdString(), std::ios_base::in);
        int del;
        fin >> MX_N >> MX_M >> N_player >> del;
        if (N_player > 4)
        {
            colors.push_back(QColor("#7FFFD4"));
            colors.push_back(QColor("#E32636"));
            colors.push_back(QColor("#FF2400"));
            colors.push_back(QColor("#AB274F"));
            colors.push_back(QColor("#9966CC"));
            colors.push_back(QColor("#CD9575"));
            colors.push_back(QColor("#44944A"));
            colors.push_back(QColor("#2F4F4F"));
            colors.push_back(QColor("#6A5ACD"));
            colors.push_back(QColor("#A8E4A0"));
            colors.push_back(QColor("#614051"));
            colors.push_back(QColor("#990066"));
            colors.push_back(QColor("#FAE7B5"));
            colors.push_back(QColor("#79553D"));
            colors.push_back(QColor("#C1876B"));
            colors.push_back(QColor("#003153"));
            colors.push_back(QColor("#3D2B1F"));
            colors.push_back(QColor("#F984E5"));
            colors.push_back(QColor("#CED23A"));
            colors.push_back(QColor("#FFDC33"));
            colors.push_back(QColor("#FF97BB"));
            colors.push_back(QColor("#DD80CC"));
            colors.push_back(QColor("#009B76"));
            colors.push_back(QColor("#480607"));
            colors.push_back(QColor("#B00000"));
            colors.push_back(QColor("#3E5F8A"));
            colors.push_back(QColor("#FFB02E"));
            colors.push_back(QColor("#900020"));
            colors.push_back(QColor("#45161C"));
            colors.push_back(QColor("#D5713F"));
            colors.push_back(QColor("#34C924"));
            colors.push_back(QColor("#00FF7F"));
            colors.push_back(QColor("#A7FC00"));
            colors.push_back(QColor("#BD33A4"));
            colors.push_back(QColor("#702963"));
            colors.push_back(QColor("#5E2129"));
            colors.push_back(QColor("#911E42"));
            colors.push_back(QColor("#256D7B"));
            colors.push_back(QColor("#FFCF48"));
            colors.push_back(QColor("#DF73FF"));
            colors.push_back(QColor("#734222"));
            colors.push_back(QColor("#C154C1"));
            colors.push_back(QColor("#F64A46"));
            colors.push_back(QColor("#00541F"));
            colors.push_back(QColor("#B57900"));
            colors.push_back(QColor("#00693E"));
            colors.push_back(QColor("#CA3767"));
            colors.push_back(QColor("#78866B"));
            colors.push_back(QColor("#158078"));
            colors.push_back(QColor("#2E8B57"));
            colors.push_back(QColor("#006633"));
            colors.push_back(QColor("#004953"));
            colors.push_back(QColor("#FFD700"));
            colors.push_back(QColor("#4B0082"));
            colors.push_back(QColor("#1B5583"));
            colors.push_back(QColor("#FF0033"));
        }
        score.resize(N_player + 1);
        field.resize(MX_N);
        for (int i = 0; i < MX_N; ++i) {
            field[i].resize(MX_M);
        }
        QVBoxLayout * layout_label = new QVBoxLayout(this);
        ui->gridLayout->addLayout(layout_label, 1, 6);
        for (int i = 0; i < N_player; ++i)
        {
            players_score.push_back(new QLabel(QString("  1 - 100"), this));
            QPalette sample_palette;
            sample_palette.setColor(QPalette::Window, colors[i + 1]);
            players_score.last()->setPalette(sample_palette);
            players_score.last()->setAutoFillBackground(true);
            layout_label->addWidget(players_score.last());
        }

        tmr = new QTimer();
        tmr->setInterval(timer_interval);
        connect(tmr, SIGNAL(timeout()), this, SLOT(updateTime()));
        tmr->start();
    } catch (...) {
        return;
    }

}


void draw_mountain(int x, int y, QPainter & canv)
{
    auto [x1, y1, x2, y2] = realcoord(x, y);
    QVector<QLine> lines(4);
    lines[0] = QLine(x1 + 2, y2 - (y2 - y1) / 10, x1 + (x2 - x1) / 3, y1 + (y2 - y1) / 4);
    lines[1] = QLine(x1 + (x2 - x1) / 3, y1 + (y2 - y1) / 4, x1 + (x2 - x1) * 3 / 5, y2 - (y2 - y1) / 6);
    lines[2] = QLine((x1 + x2) / 2, y1 + (y2 - y1) * 39 / 64, x2 - (x2 - x1) / 3, y1 + (y2 - y1) * 2 / 5);
    lines[3] = QLine(x2 - (x2 - x1) / 3, y1 + (y2 - y1) * 2 / 5, x2 - 2, y2 - (y2 - y1) / 10);
    canv.drawLines(lines);
    return;
}

void draw_crown(int x, int y, QPainter & canv)
{
    auto [x1, y1, x2, y2] = realcoord(x, y);
    QVector<QLine> lines(10);
    int r = 2;
    canv.drawEllipse(x1 + (x2 - x1) / 10 - r, y1 + (y2 - y1) * 2 / 5 - r, 2 * r + 1, 2 * r + 1);
    lines[0] = QLine(x1 + (x2 - x1) / 10, y1 + (y2 - y1) * 2 / 5, x1 + (x2 - x1) * 3 / 10, (y1 + y2) / 2);
    lines[1] = QLine(x1 + (x2 - x1) * 3 / 10, (y1 + y2) / 2, (x1 + x2) / 2, y1 + (y2 - y1) * 1 / 3);
    canv.drawEllipse((x1 + x2) / 2 - r, y1 + (y2 - y1) * 1 / 3 - r, 2 * r + 1, 2 * r + 1);
    lines[2] = QLine((x1 + x2) / 2, y1 + (y2 - y1) * 1 / 3, x1 + (x2 - x1) * 7 / 10, (y1 + y2) / 2);
    lines[3] = QLine(x1 + (x2 - x1) * 7 / 10, (y1 + y2) / 2, x2 - (x2 - x1) / 10, y1 + (y2 - y1) * 2 / 5);
    canv.drawEllipse(x2 - (x2 - x1) / 10 - r, y1 + (y2 - y1) * 2 / 5 - r, 2 * r + 1, 2 * r + 1);
    lines[4] = QLine(x1 + (x2 - x1) / 5, y2 - (y2 - y1) / 5, x1 + (x2 - x1) / 10, y1 + (y2 - y1) * 2 / 5);
    lines[5] = QLine(x2 - (x2 - x1) / 10, y1 + (y2 - y1) * 2 / 5, x2 - (x2 - x1) / 5, y2 - (y2 - y1) / 5);
    lines[6] = QLine(x1 + (x2 - x1) / 5, y2 - (y2 - y1) / 5, x2 - (x2 - x1) / 5, y2 - (y2 - y1) / 5);
    lines[7] = QLine(x2 - (x2 - x1) / 5, y2 - (y2 - y1) / 5, x2 - (x2 - x1) / 5, y2 - (y2 - y1) / 10);
    lines[8] = QLine(x2 - (x2 - x1) / 5, y2 - (y2 - y1) / 10, x1 + (x2 - x1) / 5, y2 - (y2 - y1) / 10);
    lines[9] = QLine(x1 + (x2 - x1) / 5, y2 - (y2 - y1) / 10, x1 + (x2 - x1) / 5, y2 - (y2 - y1) / 5);
    canv.drawLines(lines);
    return;
}

void draw_city(int x, int y, QPainter & canv)
{
    auto [x1, y1, x2, y2] = realcoord(x, y);
    canv.drawEllipse(x1 + (x2 - x1) / 5, y1 + (y2 - y1) / 5 , (x2 - x1) * 3 / 5, (y2 - y1) * 3 / 5);
    return;
}

void draw_grid(int x1, int y1, int x2, int y2, QPainter & canv)
{
    QVector<QLine> lines;
    for (int i = 0; i < MX_N + 1; ++i) {
        lines.push_back(QLine(x1, y1 + (y2 - y1) * i / MX_N, x2, y1 + (y2 - y1) * i / MX_N));
    }
    for (int i = 0; i < MX_M + 1; ++i) {
        lines.push_back(QLine(x1 + (x2 - x1) * i / MX_M, y1, x1 + (x2 - x1) * i / MX_M, y2));
    }
    canv.drawLines(lines);
    return;
}

void print(int x, int y, int value, QPainter & canv)
{
    auto [x1, y1, x2, y2] = realcoord(x, y);
    canv.drawText((x2 + x1) / 2 - R * 2 - 8, (y2 + y1) / 2 - size_font / 2, R * 4 + 16, size_font, Qt::AlignCenter, QString::number(value));
}

void fill_rect(int x, int y, int player, QPainter & canv)
{
    QBrush b(colors[player]);
    auto [x1, y1, x2, y2] = realcoord(x, y);
    canv.fillRect(x1, y1, x2 - x1, y2 - y1, b);
    return;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    if (skip_step) return;
    for (int i = 0; i < N_player; ++i)
    {
        players_score[i]->setText(QString("%1 - %2").arg(score[i].second, 3, 10, QChar(' ')).arg(score[i].first));
    }
    QPainter canv(this);
    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(R / 2);
    canv.setPen(pen);
    QFont f;
    f.setPointSize(size_font);
    canv.setFont(f);
    for (int i = 0; i < MX_N; ++i) {
        for (int j = 0; j < MX_M; ++j) {
            auto [t, pl, v] = field[i][j];
            if (t == 1) // space
            {
                fill_rect(i, j, pl, canv);
                if (v > 0) {
                    print(i, j, v, canv);
                }
            }
            else if (t == 2) // city
            {
                fill_rect(i, j, pl, canv);
                draw_city(i, j, canv);
                if (v > 0) {
                    print(i, j, v, canv);
                }
            }
            else if (t == 3) // capital
            {
                fill_rect(i, j, pl, canv);
                draw_crown(i, j, canv);
                if (v > 0) {
                    print(i, j, v, canv);
                }
            }
            else if (t == 4) {
                draw_mountain(i, j, canv);
            }
        }
    }
    draw_grid(R, dy, W + R, H + dy, canv);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    W = this->size().width() - dx;
    H = this->size().height() - dy;
    R = std::min(W, H) / 100;
    size_font = (std::min(W, H) / 40);
}

bool read_next_event()
{
    uii->Turn_counter_text->setTitle(QString::number(++turn_counter) + '/' + QString::number(turn_counter / N_player));
    int m;
    fin >> m;
    if (m == 0)
    {
        game_not_stopped = false;
        return false;
    }
    for (int i = 0; i < N_player; ++i) {
        fin >> score[i].first >> score[i].second;
    }
    int del;
    for (int i = 0; i < MX_N; ++i) {
        for (int j = 0; j < MX_M; ++j) {
            fin >> del >> std::get<0>(field[i][j]);
            if (std::get<0>(field[i][j]) != 4) {
                fin >> std::get<1>(field[i][j]) >> std::get<2>(field[i][j]);
            }
        }
    }
    return !skip;
}

void MainWindow::updateTime()
{
    tmr->setInterval(timer_interval);
    bool ch = read_next_event();
    if (game_not_stopped)
    {
        if (full_turn || skip_next_X_steps > 0)
        {
            if (skip_next_X_steps > 0)
            {
                --skip_next_X_steps;
            }
            skip_step = true;
            tmr->setInterval(0);
            return;
        }
        skip_step = false;
        QWidget::repaint();
    }
    else
    {
        tmr->stop();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    ui->label_2->setText(QString::number(value));
    timer_interval = 1000 / value;
    tmr->setInterval(timer_interval);
}

void MainWindow::on_actionSkip_empty_turns_changed()
{
    skip = ui->actionSkip_empty_turns->isChecked();
}

void MainWindow::on_pushButton_clicked()
{
    if (!pause_game) {
        skip_next_X_steps = 100;
        tmr->setInterval(1);
    }
}

void MainWindow::on_actionFull_turn_changed()
{
    full_turn = ui->actionFull_turn->isChecked();
}

void MainWindow::on_pushButton_2_clicked()
{   
    if (!pause_game) {
        skip_next_X_steps = 500 * N_player;
        tmr->setInterval(1);
    }
}

void MainWindow::on_pushPauseButton_clicked()
{
    pause_game = !pause_game;
    if (pause_game) {
        tmr->stop();
    } else {
        tmr->start();
    }
}
