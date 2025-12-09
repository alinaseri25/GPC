#include "qompasswindow.h"
#include "ui_qompasswindow.h"

QompassWindow::QompassWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QompassWindow)
{
    ui->setupUi(this);
    pixmap = QPixmap::fromImage(QImage(":/preFiles/digital-compass.png"));
    compass = new QCompass(this);
    connect(compass,&QCompass::readingChanged,this,&QompassWindow::onReadingChanged);
    compass->start();
}

QompassWindow::~QompassWindow()
{
    delete ui;
}

void QompassWindow::onReadingChanged()
{
    QCompassReading *reading = compass->reading();
    if (reading) {
        azimuth = reading->azimuth();   // زاویه جهت شمال مغناطیسی (درجه)
        calibration = reading->calibrationLevel(); // سطح کالیبراسیون
        QString compassStr = QString("%1°").arg(azimuth);
        //qDebug() << compassStr;
        ui->Lbl_Degree->setText(compassStr);

        int side = qMin(ui->LblView->width(), ui->LblView->height());
        QPixmap canvas(side, side);
        canvas.fill(Qt::transparent);

        QPainter p(&canvas);
        p.setRenderHint(QPainter::SmoothPixmapTransform, true);
        p.setRenderHint(QPainter::Antialiasing, true);

        // مرکز بوم = مبدا چرخش
        p.translate(side / 2.0, side / 2.0);
        p.rotate(-azimuth);
        p.translate(-side / 2.0, -side / 2.0);

        // حالا تصویر اصلی را fit می‌کنیم داخل بوم
        QSize targetSize(side, side);
        QPixmap scaledPixmap = pixmap.scaled(targetSize,
                                             Qt::KeepAspectRatio,
                                             Qt::SmoothTransformation);

        // رسم در مرکز بوم
        int x = (side - scaledPixmap.width()) / 2;
        int y = (side - scaledPixmap.height()) / 2;
        p.drawPixmap(x, y, scaledPixmap);

        p.end();

        ui->LblView->setPixmap(canvas);
    }
}
