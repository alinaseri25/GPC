#ifndef QOMPASSWINDOW_H
#define QOMPASSWINDOW_H

#define PI  	3.14159265358979323846

#include <QMainWindow>
#include <QPixmap>
#include <QTransform>
#include <QCompass>
#include <QCompassReading>
#include <QPainter>

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

    qreal azimuth;
    qreal calibration;
    QCompass *compass;
    QPixmap pixmap;

private slots:
    void onReadingChanged(void);
};

#endif // QOMPASSWINDOW_H
