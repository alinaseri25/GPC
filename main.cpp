#include "gpsmain.h"
#include <QApplication>
#include <QSplashScreen>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QImage img(":/preFiles/icon.png");
    QSplashScreen *scc = new QSplashScreen(QPixmap::fromImage(img));
    scc->showMessage(QString("GP Control."),Qt::AlignHCenter|Qt::AlignCenter,Qt::black);

    GPSMain w;

    QTimer::singleShot(2000,scc,SLOT(close()));
    QTimer::singleShot(2000,&w,SLOT(show()));
    scc->show();
    w.hide();

    return a.exec();
}
