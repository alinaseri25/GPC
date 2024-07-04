#include "locations.h"
#include "ui_locations.h"

#include <QClipboard>

Locations::Locations(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Locations)
{
    ui->setupUi(this);

    TimerCurrentDistance = new QTimer(this);
    TimerCurrentDistance->setInterval(250);
    //connect(TimerCurrentDistance,SIGNAL(timeout()),this,SLOT(on_Btn_SelectMyLocationD_clicked()));
    connect(TimerCurrentDistance,&QTimer::timeout,this,&Locations::on_Btn_SelectMyLocationD_clicked);

    TimerCurrentSource = new QTimer(this);
    TimerCurrentSource->setInterval(250);
    //connect(TimerCurrentSource,SIGNAL(timeout()),this,SLOT(on_Btn_SelectMyLocationS_clicked()));
    connect(TimerCurrentSource,&QTimer::timeout,this,&Locations::on_Btn_SelectMyLocationS_clicked);

    ui->Tbw_Locations->setColumnCount(8);
    ui->Tbw_Locations->setRowCount(0);

    QStringList QSL;
    QSL << QString("ID") << QString("Location Name") << QString("latitude") << QString("longitude")
        << QString("HAC")<< QString("Height") << QString("Speed") << QString("Direction");

    ui->Tbw_Locations->setHorizontalHeaderLabels(QSL);

    geoCoordinate = NULL;
    geoCoordinateOther = NULL;

    ui->Frm_MainBtns->setVisible(true);
    ui->Frm_Rename->setVisible(false);
    ui->Frm_Distance->setVisible(false);
}

Locations::~Locations()
{
    delete ui;
}

void Locations::setData(QString *_Path, QString *_Longitude, QString *_latitude, QString *_Hight, QString *_HAC, QString *_GSPeed, QString *_Direction, QString *_Magvar)
{
    Path = _Path;
    Longitude = _Longitude;
    latitude = _latitude;
    Hight = _Hight;
    HAC = _HAC;
    GSPeed = _GSPeed;
    Direction = _Direction;
    Magvar = _Magvar;

    LogFile = new QFile(*Path);
    if(!LogFile->open(QIODevice::ReadWrite))
    {
        QMessageBox::warning(this,QString("خطا در ایجاد فایل"),QString("متاسفانه قادر به ایجاد و بازگشایی فایل نبودیم"));
        return;
    }
    LogFile->close();

    sqlDatabase = QSqlDatabase::addDatabase("QSQLITE","Connection");
    sqlDatabase.setDatabaseName(*Path);

    CreateTable();

}

void Locations::on_Btn_Exit_clicked()
{
    this->close();
}

void Locations::on_Btn_Reset_clicked()
{
    if(ui->Tbw_Locations->selectedItems().count() == 0)
    {
        QMessageBox::about(this,QString("خطای عدم انتخاب"),QString("لطفا یک رکورد را انتخاب کنید"));
        return;
    }
    int retVal = QMessageBox::question(this,QString("Reset Location ."),
                                       QString("Do You Want To Reset This Place ?"),
                                       QMessageBox::Yes,QMessageBox::No);
    if(retVal == QMessageBox::Yes)
    {
        if(sqlDatabase.open())
        {
            QSqlQuery sqlQuery(sqlDatabase);
            sqlQuery.prepare(QString("UPDATE Locations SET latitude = '%1',longitude = '%2',"
                                     "HAC = '%3',Height = '%4',GSPeed = '%5',Direction = '%6' WHERE ID = %7")
                             .arg(*latitude).arg(*Longitude).arg(*HAC).arg(*Hight).arg(*GSPeed).arg(*Direction)
                             .arg(ui->Tbw_Locations->selectedItems()[0]->text()));
            if(sqlQuery.exec())
            {
                QMessageBox::about(this,QString("پایان عملیات"),QString("تغییرات انجام پذیرفت ."));
                if(sqlDatabase.isOpen())
                    sqlDatabase.close();
            }
            else
            {
                QMessageBox::warning(this,QString("خطا در اجرای دستورات بانک")
                                     ,QString("به دلایلی قادر به اجرای دستورات بانک نبودیم ."));
                if(sqlDatabase.isOpen())
                    sqlDatabase.close();
                return;
            }
        }
        else
        {
            QMessageBox::warning(this,QString("خطا در بازگشایی بانک اطلاعاتی")
                                 ,QString("متاسفانه قادر به باز گشایی بانک اطلاعاتی نبودیم ."));
            return;
        }
    }
    ui->Tbw_Locations->clear();
    ui->Tbw_Locations->setRowCount(0);
    CreateTable();
}

void Locations::CreateTable()
{

    QStringList QSL;
    QSL << QString("ID") << QString("Location Name") << QString("latitude") << QString("longitude")
        << QString("HAC")<< QString("Height") << QString("Speed") << QString("Direction");

    ui->Tbw_Locations->setHorizontalHeaderLabels(QSL);


    if(sqlDatabase.open())
    {
        QSqlQuery sqlQuery(sqlDatabase);
        sqlQuery.prepare(QString("SELECT * FROM Locations"));

        if(sqlQuery.exec())
        {
            while (sqlQuery.next()) {
                QTableWidgetItem *_Tbw_ID = new QTableWidgetItem(QString("%1").arg(sqlQuery.value(0).toString()));
                QTableWidgetItem *_Tbw_L_Name = new QTableWidgetItem(QString("%1").arg(sqlQuery.value(1).toString()));
                QTableWidgetItem *_Tbw_latitude = new QTableWidgetItem(QString("%1").arg(sqlQuery.value(2).toString()));
                QTableWidgetItem *_Tbw_longitude = new QTableWidgetItem(QString("%1").arg(sqlQuery.value(3).toString()));
                QTableWidgetItem *_Tbw_HAC = new QTableWidgetItem(QString("%1").arg(sqlQuery.value(4).toString()));
                QTableWidgetItem *_Tbw_Height = new QTableWidgetItem(QString("%1").arg(sqlQuery.value(5).toString()));
                QTableWidgetItem *_Tbw_GSpeed = new QTableWidgetItem(QString("%1").arg(sqlQuery.value(6).toString()));
                QTableWidgetItem *_Tbw_Direction = new QTableWidgetItem(QString("%1").arg(sqlQuery.value(7).toString()));
                int rowcount = ui->Tbw_Locations->rowCount() + 1;
                ui->Tbw_Locations->setRowCount(rowcount);
                --rowcount;
                //ui->Tbw_Locations->setVerticalHeaderItem(rowcount,_Tbw_ID);
                ui->Tbw_Locations->setItem(rowcount,0,_Tbw_ID);
                ui->Tbw_Locations->setItem(rowcount,1,_Tbw_L_Name);
                ui->Tbw_Locations->setItem(rowcount,2,_Tbw_latitude);
                ui->Tbw_Locations->setItem(rowcount,3,_Tbw_longitude);
                ui->Tbw_Locations->setItem(rowcount,4,_Tbw_HAC);
                ui->Tbw_Locations->setItem(rowcount,5,_Tbw_Height);
                ui->Tbw_Locations->setItem(rowcount,6,_Tbw_GSpeed);
                ui->Tbw_Locations->setItem(rowcount,7,_Tbw_Direction);

            }
            if(sqlDatabase.isOpen())
                sqlDatabase.close();
        }
        else
        {
            QMessageBox::warning(this,QString("خطا در اجرای دستورات بانک")
                                 ,QString("به دلایلی قادر به اجرای دستورات بانک نبودیم ."));
            if(sqlDatabase.isOpen())
                sqlDatabase.close();
            return;
        }
    }
    else
    {
        QMessageBox::warning(this,QString("خطا در بازگشایی بانک اطلاعاتی")
                             ,QString("متاسفانه قادر به باز گشایی بانک اطلاعاتی نبودیم ."));
        return;
    }
    ui->Tbw_Locations->resizeColumnsToContents();
}

void Locations::on_Btn_Rename_clicked()
{
    if(ui->Tbw_Locations->selectedItems().count() == 0)
    {
        QMessageBox::about(this,QString("خطای عدم انتخاب"),QString("لطفا یک رکورد را انتخاب کنید"));
        return;
    }
    ui->Frm_MainBtns->setVisible(false);
    ui->Frm_Rename->setVisible(true);
    ui->Frm_Distance->setVisible(false);
    ui->Txt_Name->setText(ui->Tbw_Locations->selectedItems()[1]->text());
    SelectedId = ui->Tbw_Locations->selectedItems()[0]->text().toInt();
}

void Locations::on_Btn_Save_clicked()
{
    if(sqlDatabase.open())
    {
        QSqlQuery sqlQuery(sqlDatabase);
        sqlQuery.prepare(QString("UPDATE Locations SET L_Name = '%1' WHERE ID = %2")
                         .arg(ui->Txt_Name->text())
                         .arg(SelectedId));
        if(sqlQuery.exec())
        {
            QMessageBox::about(this,QString("پایان عملیات"),QString("تغییرات انجام پذیرفت ."));
            if(sqlDatabase.isOpen())
                sqlDatabase.close();
        }
        else
        {
            QMessageBox::warning(this,QString("خطا در اجرای دستورات بانک")
                                 ,QString("به دلایلی قادر به اجرای دستورات بانک نبودیم ."));
            if(sqlDatabase.isOpen())
                sqlDatabase.close();
            return;
        }
    }
    else
    {
        QMessageBox::warning(this,QString("خطا در بازگشایی بانک اطلاعاتی")
                             ,QString("متاسفانه قادر به باز گشایی بانک اطلاعاتی نبودیم ."));
        return;
    }
    ui->Frm_MainBtns->setVisible(true);
    ui->Frm_Rename->setVisible(false);
    ui->Frm_Distance->setVisible(false);
    ui->Tbw_Locations->clear();
    ui->Tbw_Locations->setRowCount(0);
    CreateTable();
}

void Locations::on_Btn_Cancel_clicked()
{
    ui->Frm_MainBtns->setVisible(true);
    ui->Frm_Rename->setVisible(false);
    ui->Frm_Distance->setVisible(false);
}

void Locations::on_Btn_Distance_clicked()
{
    ui->Frm_Distance->setVisible(true);
    ui->Frm_MainBtns->setVisible(false);
    ui->Frm_Rename->setVisible(false);
    if(geoCoordinate != NULL)
        delete geoCoordinate;
    geoCoordinate = new QGeoCoordinate();
    geoCoordinate->setLatitude((*latitude).toDouble());
    geoCoordinate->setAltitude((*Hight).toDouble());
    geoCoordinate->setLongitude((*Longitude).toDouble());
    ui->Lbl_LatitudeS->setText(QString("%1").arg(geoCoordinate->latitude()));
    ui->Lbl_LongtitudeS->setText(QString("%1").arg(geoCoordinate->longitude()));


    if(geoCoordinateOther != NULL)
        delete geoCoordinateOther;
    geoCoordinateOther = new QGeoCoordinate();
    geoCoordinateOther->setLatitude((*latitude).toDouble());
    geoCoordinateOther->setAltitude((*Hight).toDouble());
    geoCoordinateOther->setLongitude((*Longitude).toDouble());
    ui->Lbl_LatitudeD->setText(QString("%1").arg(geoCoordinateOther->latitude()));
    ui->Lbl_LongtitudeD->setText(QString("%1").arg(geoCoordinateOther->longitude()));

}

void Locations::on_Btn_DistanceCancel_clicked()
{
    ui->Frm_Distance->setVisible(false);
    ui->Frm_MainBtns->setVisible(true);
    ui->Frm_Rename->setVisible(false);
}

void Locations::on_Btn_SelectFromTableS_clicked()
{
    if(ui->Tbw_Locations->selectedItems().count() == 0)
    {
        QMessageBox::about(this,QString("خطای عدم انتخاب"),QString("لطفا یک رکورد را انتخاب کنید"));
        return;
    }
    if(geoCoordinate != NULL)
        delete geoCoordinate;
    geoCoordinate = new QGeoCoordinate();
    geoCoordinate->setLatitude(ui->Tbw_Locations->selectedItems()[2]->text().toDouble());
    geoCoordinate->setAltitude(ui->Tbw_Locations->selectedItems()[5]->text().toDouble());
    geoCoordinate->setLongitude(ui->Tbw_Locations->selectedItems()[3]->text().toDouble());
    ui->Lbl_LatitudeS->setText(QString("%1").arg(geoCoordinate->latitude()));
    ui->Lbl_LongtitudeS->setText(QString("%1").arg(geoCoordinate->longitude()));
    TimerCurrentSource->stop();
    ui->Btn_SelectMyLocationS->setText(QString("Current"));
    if(ui->Btn_SelectFromTableS->text() == QString(">Selected"))
    {
        ui->Btn_SelectFromTableS->setText(QString("*Selected"));
    }
    else
    {
        ui->Btn_SelectFromTableS->setText(QString(">Selected"));
    }
    on_Btn_DistanceCalculate_clicked();
}

void Locations::on_Btn_SelectMyLocationS_clicked()
{
    if(geoCoordinate != NULL)
        delete geoCoordinate;
    geoCoordinate = new QGeoCoordinate();
    geoCoordinate->setLatitude((*latitude).toDouble());
    geoCoordinate->setAltitude((*Hight).toDouble());
    geoCoordinate->setLongitude((*Longitude).toDouble());
    ui->Lbl_LatitudeS->setText(QString("%1").arg(geoCoordinate->latitude()));
    ui->Lbl_LongtitudeS->setText(QString("%1").arg(geoCoordinate->longitude()));
    TimerCurrentSource->start(250);
    ui->Btn_SelectMyLocationS->setText(QString(">Current"));
    ui->Btn_SelectFromTableS->setText(QString("Selected"));
    on_Btn_DistanceCalculate_clicked();
}

void Locations::on_Btn_SelectFromTableD_clicked()
{
    if(ui->Tbw_Locations->selectedItems().count() == 0)
    {
        QMessageBox::about(this,QString("خطای عدم انتخاب"),QString("لطفا یک رکورد را انتخاب کنید"));
        return;
    }
    if(geoCoordinateOther != NULL)
        delete geoCoordinateOther;
    geoCoordinateOther = new QGeoCoordinate();
    geoCoordinateOther->setLatitude(ui->Tbw_Locations->selectedItems()[2]->text().toDouble());
    geoCoordinateOther->setAltitude(ui->Tbw_Locations->selectedItems()[5]->text().toDouble());
    geoCoordinateOther->setLongitude(ui->Tbw_Locations->selectedItems()[3]->text().toDouble());
    ui->Lbl_LatitudeD->setText(QString("%1").arg(geoCoordinateOther->latitude()));
    ui->Lbl_LongtitudeD->setText(QString("%1").arg(geoCoordinateOther->longitude()));
    TimerCurrentDistance->stop();
    if(ui->Btn_SelectFromTableD->text() == QString(">Selected"))
    {
        ui->Btn_SelectFromTableD->setText(QString("*Selected"));
    }
    else
    {
        ui->Btn_SelectFromTableD->setText(QString(">Selected"));
    }
    ui->Btn_SelectMyLocationD->setText(QString("Current"));
    on_Btn_DistanceCalculate_clicked();
}

void Locations::on_Btn_SelectMyLocationD_clicked()
{
    if(geoCoordinateOther != NULL)
        delete geoCoordinateOther;
    geoCoordinateOther = new QGeoCoordinate();
    geoCoordinateOther->setLatitude((*latitude).toDouble());
    geoCoordinateOther->setAltitude((*Hight).toDouble());
    geoCoordinateOther->setLongitude((*Longitude).toDouble());
    ui->Lbl_LatitudeD->setText(QString("%1").arg(geoCoordinateOther->latitude()));
    ui->Lbl_LongtitudeD->setText(QString("%1").arg(geoCoordinateOther->longitude()));
    TimerCurrentDistance->start(250);
    ui->Btn_SelectFromTableD->setText(QString("Selected"));
    ui->Btn_SelectMyLocationD->setText(QString(">Current"));
    on_Btn_DistanceCalculate_clicked();
}

void Locations::on_Btn_DistanceCalculate_clicked()
{
    if(geoCoordinate == NULL || geoCoordinateOther == NULL)
    {
        QMessageBox::warning(this,QString("خطا در محاسبه"),QString("یکی از مقادیر مبدا یا مقصد خالی می باشد ."));
        return;
    }
    if(!geoCoordinate->isValid())
    {
        QMessageBox::warning(this,QString("خطا در محاسبه"),QString("مقدار مبدا غیر قابل قبول می باشد ."));
        return;
    }
    if(!geoCoordinateOther->isValid())
    {
        QMessageBox::warning(this,QString("خطا در محاسبه"),QString("مقدار مقصد غیر قابل قبول می باشد ."));
        return;
    }
    qreal distance = geoCoordinate->distanceTo(*geoCoordinateOther);
    if(distance < 1000)
        ui->Lbl_Distance->setText(QString("%1 m").arg(distance));
    else if(distance < (1000*1000))
        ui->Lbl_Distance->setText(QString("%1 Km").arg(distance/1000));
    else
        ui->Lbl_Distance->setText(QString("%1 Mm").arg(distance/(1000*1000)));
    ui->Lbl_Degree->setText(QString("%1 Degree From North").arg(geoCoordinate->azimuthTo(*geoCoordinateOther)));

}

void Locations::on_Btn_Send_clicked()
{
    QString _Longitude,_latitude,_Hight,_HAC,_GSPeed,_Direction,DataOut;
    if(ui->Tbw_Locations->selectedItems().count() == 0)
    {
        _Longitude = *Longitude;
        _latitude = *latitude;
        _Hight = *Hight;
        _HAC = *HAC;
        _GSPeed = *GSPeed;
        _Direction = *Direction;
        DataOut = QString("Data Copied to Clipboard.\r\nFrom Live GPS!");
    }
    else
    {
        _Longitude = QString("%1").arg(ui->Tbw_Locations->selectedItems()[3]->text().toDouble());
        _latitude = QString("%1").arg(ui->Tbw_Locations->selectedItems()[2]->text().toDouble());
        _Hight = QString("%1").arg(ui->Tbw_Locations->selectedItems()[5]->text().toDouble());;
        _HAC = QString("%1").arg(ui->Tbw_Locations->selectedItems()[4]->text().toDouble());
        _GSPeed = QString("%1").arg(ui->Tbw_Locations->selectedItems()[6]->text().toDouble());;
        _Direction = QString("%1").arg(ui->Tbw_Locations->selectedItems()[7]->text());;
        DataOut = QString("Data Copied to Clipboard.\r\nFrom Selected Location!");
    }
    QString Str = QString("http://www.google.com/maps/place/%1,%2/@%1,%2,30z/data=!3m1!1e3/")
            .arg(_latitude).arg(_Longitude);
    Str += QString("\nHAC:%1").arg(_HAC);
    Str += QString("\nHight:%1").arg(_Hight);
    Str += QString("\nGSPeed:%1 K/H").arg(_GSPeed);
    Str += QString("\nDir:%1").arg(_Direction);
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->clear();
    clipboard->setText(Str);
    QMessageBox::about(this,"Copied to ClipBoard.",DataOut);
}

void Locations::on_Tbw_Locations_itemSelectionChanged()
{
    if(ui->Btn_SelectFromTableD->text() == QString(">Selected"))
    {
        ui->Btn_SelectFromTableD->setText(QString("*Selected"));
        on_Btn_SelectFromTableD_clicked();
    }
    else if(ui->Btn_SelectFromTableS->text() == QString(">Selected"))
    {
        ui->Btn_SelectFromTableS->setText(QString("*Selected"));
        on_Btn_SelectFromTableS_clicked();
    }
}
