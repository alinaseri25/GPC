#include "qompasswindow.h"
#include "ui_qompasswindow.h"

QompassWindow::QompassWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QompassWindow)
{
    ui->setupUi(this);
    degree = 0;
    graphicsScene = new QGraphicsScene(this);
    ui->GV_Qompass->setScene(graphicsScene);
    QompassChanged(0);
}

QompassWindow::~QompassWindow()
{
    delete ui;
}

void QompassWindow::paintEvent(QPaintEvent *_paint)
{
    Q_UNUSED(_paint)
}

void QompassWindow::QompassChanged(int _degree)
{
    int Lenghth = ui->scrollArea->width()-30;
    ui->GV_Qompass->setFixedWidth(Lenghth);
    ui->GV_Qompass->setFixedHeight(Lenghth);
    degree = _degree;
    ui->Lbl_Degree->setText(QString("%1").arg(degree));
    QTransform mt;
    mt.rotate(degree);
    graphicsScene->clear();
    graphicsScene->addPixmap((QPixmap::fromImage(QImage(":/preFiles/digital-compass.png")))
                             .scaledToHeight(Lenghth)
                             .scaledToWidth(Lenghth)
                             .transformed(mt));
}
