#ifndef QOMPASSWINDOW_H
#define QOMPASSWINDOW_H

#define PI  	3.14159265358979323846

#include <QMainWindow>
#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsScene>

namespace Ui {
class QompassWindow;
}

class QompassWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit QompassWindow(QWidget *parent = 0);
    ~QompassWindow();

private:
    Ui::QompassWindow *ui;

    void paintEvent(QPaintEvent *_paint);
    int degree;
    QGraphicsScene *graphicsScene;

private slots:
    void QompassChanged(int _degree);
};

#endif // QOMPASSWINDOW_H
