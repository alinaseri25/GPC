#include "gpsmain.h"
#include "ui_gpsmain.h"
#include "../../JalaliDate/qdatejalali.h"

GPSMain::GPSMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GPSMain)
{
    QApplication::setApplicationVersion(QString("2.0.0"));
    QApplication::setApplicationDisplayName(QString("GP Control"));
    LocationWindow = NULL;
    QompWindow = NULL;

    Qompass = new QAction("&Qompass",this);
    Qompass->setEnabled(false);

    Equipment = menuBar()->addMenu("&Equipment");
    Equipment->addAction(Qompass);
    connect(Qompass,&QAction::triggered,this,&GPSMain::on_Qompass_clicked);

    LocationNumber = 0;
    this->setFixedSize(this->geometry().width(),this->geometry().height());

    QStringList systemEnvironment = QProcess::systemEnvironment();
    foreach (QString Str, systemEnvironment) {
        //QMessageBox::about(this,QString("System Environment"),QString("Data : %1").arg(Str));
        if(Str.contains("EXTERNAL_STORAGE="))
        {
            Str.remove("EXTERNAL_STORAGE=");
            Path = Str;
            break;
        }
    }
    trackerPath = Path;
    Path = QString("%1/gps.db").arg(Path);
    //Path = QString("/storage/emulated/gps.db").arg(Path);
    //QMessageBox::about(this,QString("GPS Path"),QString("Data : %1").arg(Path));
    LogFile = new QFile(Path);
    //LogTextStream = new QTextStream(LogFile);
    //LogTextStream->setCodec("UTF-8");
    //----------------------Position Info
    QgeoPositionInfoSource = QGeoPositionInfoSource::createDefaultSource(this);
    if(QgeoPositionInfoSource)
    {
        QgeoPositionInfoSource->setPreferredPositioningMethods(QGeoPositionInfoSource::AllPositioningMethods);
        connect(QgeoPositionInfoSource,SIGNAL(error(QGeoPositionInfoSource::Error)),this
                ,SLOT(error(QGeoPositionInfoSource::Error)));
        connect(QgeoPositionInfoSource,SIGNAL(positionUpdated(QGeoPositionInfo)),this
                ,SLOT(setGPSLocation(QGeoPositionInfo)));
        QgeoPositionInfoSource->setUpdateInterval(100);
        QgeoPositionInfoSource->startUpdates();
    }
    else
    {
        QMessageBox::warning(this,"Error","Qt Position Can Not Start .");
    }

    //----------------------Satellite Info
    QgeoSatelliteInfoSource = QGeoSatelliteInfoSource::createDefaultSource(this);
    if(QgeoSatelliteInfoSource)
    {
        connect(QgeoSatelliteInfoSource,SIGNAL(error(QGeoSatelliteInfoSource::Error)),this
                ,SLOT(error(QGeoSatelliteInfoSource::Error)));
        connect(QgeoSatelliteInfoSource,SIGNAL(satellitesInViewUpdated(QList<QGeoSatelliteInfo>)),this
                ,SLOT(satellitesInViewUpdated(QList<QGeoSatelliteInfo>)));
        connect(QgeoSatelliteInfoSource,SIGNAL(satellitesInUseUpdated(QList<QGeoSatelliteInfo>)),this
                ,SLOT(satellitesInUseUpdated(QList<QGeoSatelliteInfo>)));
        QgeoSatelliteInfoSource->setUpdateInterval(100);
        QgeoSatelliteInfoSource->requestUpdate(6000);
        QgeoSatelliteInfoSource->startUpdates();
    }
    else
    {
        QMessageBox::warning(this,"Error","Qt Satellite Can Not Start .");
    }
    InUse = InView = 0;
    SatSet = false;
    Direction = "nan";
    ui->setupUi(this);
    ChangeBtnState(false);
    changelocationNameBar(false);

    trackerTimer = new QTimer(this);
    connect(trackerTimer,&QTimer::timeout,this,&GPSMain::on_trackerTimer_Timeout);
    trackerTimer->stop();

    connect(ui->DspbTracker,static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),this,&GPSMain::on_DspbTracker_valuechanged);

    connect(ui->BtnTracker,&QPushButton::clicked,this,&GPSMain::on_BtnTracker_Clicked);

    trackerFile = new QFile(this);
}

GPSMain::~GPSMain()
{
    delete ui;
}

QString GPSMain::SatelliteType(int StType)
{
    switch (StType) {
    case QGeoSatelliteInfo::GLONASS:
        return "GLONASS";
        break;
    case QGeoSatelliteInfo::GPS:
        return "GPS";
        break;
    default:
        return QString("OTHER.%1").arg(StType);
        break;
    }
}

QString GPSMain::DirectionSet(QString _Data)
{
    double Drc = _Data.toDouble();
    emit QompassChange(int(Drc));
    if(Drc >= 350 || Drc <= 10)
    {
        return QString("%1\tDir:%2").arg(Drc).arg("N");
    }
    else if(Drc > 10 && Drc < 80)
    {
        return QString("%1\tDir:%2").arg(Drc).arg("NE");
    }
    else if(Drc >= 80 && Drc <= 100)
    {
        return QString("%1\tDir:%2").arg(Drc).arg("E");
    }
    else if(Drc > 100 && Drc < 170)
    {
        return QString("%1\tDir:%2").arg(Drc).arg("SE");
    }
    else if(Drc >= 170 && Drc <= 190)
    {
        return QString("%1\tDir:%2").arg(Drc).arg("S");
    }
    else if(Drc > 190 && Drc < 260)
    {
        return QString("%1\tDir:%2").arg(Drc).arg("SW");
    }
    else if(Drc >= 260 && Drc <= 280)
    {
        return QString("%1\tDir:%2").arg(Drc).arg("W");
    }
    else if(Drc > 280 && Drc < 350)
    {
        return QString("%1\tDir:%2").arg(Drc).arg("NW");
    }
    else
    {
        return QString("%1\tDir:%2").arg(Drc).arg("nan");
    }
}

void GPSMain::ChangeBtnState(bool state)
{
    ui->Btn_SaveLocation->setEnabled(state);
    ui->Btn_Send->setEnabled(state);
    Qompass->setEnabled(true);
}

void GPSMain::changelocationNameBar(bool state)
{
    ui->Btn_SaveLocationEnd->setVisible(state);
    ui->Txt_LocationName->setVisible(state);
    ui->Lbl_LocationName->setVisible(state);
    ui->Btn_Cancel->setVisible(state);
}

void GPSMain::error(QGeoSatelliteInfoSource::Error error)
{
    QMessageBox::warning(this,"Error Read StelliteInfo"
                         ,QString("StelliteInfo Error Number : %1 \nWas Happend !").arg(int(error)));
}

void GPSMain::error(QGeoPositionInfoSource::Error error)
{
    QMessageBox::warning(this,"Error Read PositionInfo"
                         ,QString("PositionInfo Error Number : %1 \nWas Happend !").arg(int(error)));
}

void GPSMain::satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &infos)
{
    QString Str = "";
    QStringList QstringList;
    StringListInView.clear();
    QStringListModel *QstringListModel = new QStringListModel(this);
    InView = 0;
    foreach (const QGeoSatelliteInfo &info, infos)
    {
        Str = QString("V->A:%1,E:%2,SS:%3,SN:%4,T:%5").arg(info.attribute(QGeoSatelliteInfo::Azimuth))
                .arg(info.attribute(QGeoSatelliteInfo::Elevation)).arg(info.signalStrength())
                .arg(info.satelliteIdentifier()).arg(SatelliteType(info.satelliteSystem()));
        //QstringList.append(Str);
        StringListInView.append(Str);
        InView++;
    }
    if(SatSet)
    {
        StringListInView.clear();
        ChangeBtnState(true);
    }
    else
    {
        QstringList = StringListInView + StringLisInUse;
        QstringListModel->setStringList(QstringList);
        ui->Lsv_SourceInfo->setModel(QstringListModel);
        ChangeBtnState(false);
    }
}

void GPSMain::satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &infos)
{
    QString Str = "";
    QStringList QstringList;
    StringLisInUse.clear();
    QStringListModel *QstringListModel = new QStringListModel(this);
    InUse = 0;
    foreach (const QGeoSatelliteInfo &info, infos)
    {
        Str = QString("U->A:%1,E:%2,SS:%3,SN:%4,T:%5").arg(info.attribute(QGeoSatelliteInfo::Azimuth))
                .arg(info.attribute(QGeoSatelliteInfo::Elevation)).arg(info.signalStrength())
                .arg(info.satelliteIdentifier()).arg(SatelliteType(info.satelliteSystem()));
        //QstringList.append(Str);
        StringLisInUse.append(Str);
        InUse++;
    }
    QstringList = StringListInView + StringLisInUse;
    QstringListModel->setStringList(QstringList);
    ui->Lsv_SourceInfo->setModel(QstringListModel);
}

void GPSMain::setGPSLocation(QGeoPositionInfo geoPositionInfo)
{
    QDATEJALALI *jalaliDate = new QDATEJALALI(this);
    QString Str = "Location Unknown!";
    if(geoPositionInfo.isValid())
    {

        SatSet = true;
        ChangeBtnState(true);
        latitude = QString("%1").arg(geoPositionInfo.coordinate().latitude());
        Longitude = QString("%1").arg(geoPositionInfo.coordinate().longitude());
        HAC = QString("%1").arg(geoPositionInfo.attribute(geoPositionInfo.HorizontalAccuracy));
        Hight = QString("%1").arg(geoPositionInfo.coordinate().altitude());
        GSPeed = QString("%1").arg(geoPositionInfo.attribute(geoPositionInfo.GroundSpeed) * 3.6);
        if(QString("%1").arg(geoPositionInfo.attribute(geoPositionInfo.Direction)) != "nan")
            Direction = DirectionSet(QString("%1").arg(geoPositionInfo.attribute(geoPositionInfo.Direction)));
        Magvar = QString("%1").arg(geoPositionInfo.attribute(geoPositionInfo.MagneticVariation));

        Str = QString("Latitude:%1,Longitude:%2\nVAC:%3,HAC:%4").arg(latitude).arg(Longitude)
                .arg(geoPositionInfo.attribute(geoPositionInfo.VerticalAccuracy)).arg(HAC);
        Str += QString("\nHight:%1").arg(Hight);
        Str += QString("\nVSpeed:%1,GSpeed:%2 K/H").arg(geoPositionInfo.attribute(geoPositionInfo.VerticalSpeed))
                .arg(GSPeed);
        Str += QString("\nTimeDate:%1").arg(geoPositionInfo.timestamp().toString("yyyy/MM/dd - hh:mm:ss"));
        jalaliDate->SetMiladiDate(geoPositionInfo.timestamp().toString("yyyy/MM/dd"));
        uint8_t jalaliStr[100];
        sprintf((char *)jalaliStr,"%d/%02d/%02d",jalaliDate->getyearsh(),jalaliDate->getmonthsh(),jalaliDate->getdaysh());
        //Str += QString("\nJalaliDate:%1/%2/%3").arg(jalaliDate->getyearsh()).arg(jalaliDate->getmonthsh())
        //        .arg(jalaliDate->getdaysh());
        Str += QString("\nJalaliDate:%1").arg(QString::fromLatin1((char *)jalaliStr));
        Str += QString("\nDirection:%1").arg(Direction);
        Str += QString("\nMag Var:%1").arg(Magvar);
    }
    Str += QString("\nInUse/InView:%2/%1").arg(InView).arg(InUse);
    ui->Txt_Location->setText(Str);
}

void GPSMain::on_Btn_Exit_clicked()
{
    if(trackerFile->isOpen())
    {
        trackerFile->close();
    }
    exit(0);
}

void GPSMain::on_Btn_Send_clicked()
{
    if(SatSet)
    {
        QString Str = QString("http://www.google.com/maps/place/%1,%2/@%1,%2,30z/data=!3m1!1e3/")
                .arg(latitude).arg(Longitude);
        Str += QString("\nHAC:%1").arg(HAC);
        Str += QString("\nHight:%1").arg(Hight);
        Str += QString("\nGSPeed:%1 K/H").arg(GSPeed);
        Str += QString("\nDir:%1").arg(Direction);
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->clear();
        clipboard->setText(Str);
        QMessageBox::about(this,"Copied to ClipBoard.","Data Copied to Clipboard.");
    }
    else
    {
        QMessageBox::about(this,"Not OK.","Now GPS Not Ok");
    }
}

void GPSMain::on_Btn_SaveLocation_clicked()
{
    changelocationNameBar(true);
    ui->Txt_LocationName->setText(QString("location%1")
                                  .arg(LocationNumber));
    ui->Txt_LocationName->setFocus();
    ui->Txt_LocationName->selectAll();
}

void GPSMain::on_Btn_SaveLocationEnd_clicked()
{
    if(!LogFile->open(QIODevice::ReadWrite))
    {
        QMessageBox::warning(this,"خطا در ایجاد فایل","متاسفانه قادر به ایجاد و بازگشایی فایل نبودیم");
        return;
    }
    LogFile->close();
    sqlDatabase = QSqlDatabase::addDatabase("QSQLITE","Connection");
    sqlDatabase.setDatabaseName(Path);
    if(sqlDatabase.open())
    {
        QSqlQuery query(sqlDatabase);
        QString Str = QString("SELECT count(*) FROM Locations");
        int id;
        query.prepare(Str);
        if(query.exec())
        {
            if(query.next())
                id = query.value(0).toInt();
            else
                id = 0;
        }
        else
        {
            id = 0;
        }
        Str = QString("insert into Locations (ID,L_Name,latitude,longitude"
                     ",HAC,Height,GSPeed,Direction) values (%1,'%2','%3','%4','%5','%6','%7','%8')")
                .arg(id).arg(ui->Txt_LocationName->text()).arg(latitude).arg(Longitude).arg(HAC).arg(Hight)
                .arg(GSPeed).arg(Direction);
        //QClipboard *clipboard = QApplication::clipboard();
        //clipboard->clear();
        //clipboard->setText(Str);
        query.prepare(Str);
        if(query.exec())
        {
            QMessageBox::about(this,"اتمام کار","ذخیره سازی بدون مشکل پایان یافت");
        }
        else
        {
            Str = "CREATE TABLE Locations"
                  "(ID INT AUTO_INCREMENT PRIMARY KEY NOT NULL,L_Name varchar(100),latitude varchar(50),longitude varchar(50)"
                  ",HAC varchar(50),Height varchar(50),GSPeed varchar(50),Direction varchar(50))";
            //clipboard->clear();
            //clipboard->setText(Str);
            if(query.exec(Str))
            {
                on_Btn_SaveLocationEnd_clicked();
                changelocationNameBar(false);
                return;
            }
            else
            {
                QMessageBox::warning(this,"خطا در ثبت اطلاعات","قادر به ثبت اطلاعات نبودیم .");
                changelocationNameBar(false);
                return;
            }
        }
    }
    else
    {
        QMessageBox::warning(this,"خطای بانک اطلاعاتی",QString("بانک اطلاعاتی موجود نیست.\nAddress:%1")
                             .arg(Path));
        changelocationNameBar(false);
        return;
    }


    changelocationNameBar(false);
    LocationNumber++;
    return;
}

void GPSMain::on_Btn_Cancel_clicked()
{
    changelocationNameBar(false);
}

void GPSMain::on_Qompass_clicked()
{
    QMessageBox::about(this,QString("GPS Path"),QString("Data : %1").arg(Path));
    if(QompWindow)
        delete QompWindow;
    QompWindow = new QompassWindow(this);
    QompWindow->show();
    connect(this,SIGNAL(QompassChange(int)),QompWindow,SLOT(QompassChanged(int)));
}

void GPSMain::on_BtnTracker_Clicked()
{
    if(ui->BtnTracker->text() == QString("ایجاد دنباله ی جدید"))
    {
        QDATEJALALI jalali;
        jalali.ProcNow();
        ui->BtnTracker->setText("در حال دنباله سازی");
        trackerFile = new QFile(QString("%1/%2-%3-%4 %5-%6-%7.csv").arg(trackerPath)
                                .arg(jalali.getyearsh())
                                .arg(jalali.getmonthsh())
                                .arg(jalali.getdaysh())
                                .arg(QTime::currentTime().hour())
                                .arg(QTime::currentTime().minute())
                                .arg(QTime::currentTime().second()));
        //QMessageBox::about(this,QString("ADDR"),trackerFile->fileName());
        trackerTextStream = new QTextStream(trackerFile);
        //trackerTextStream->setCodec("UTF-8");
        trackerFile->open(QFile::ReadWrite);
        QString Data = QString("ID,Longitude,latitude,DateTime\n");
        *trackerTextStream << Data;
        IDCounter = 0;
        double value = ui->DspbTracker->value() * 1000.0;
        trackerTimer->setInterval(value);
        trackerTimer->start();
    }
    else
    {
        trackerTimer->stop();
        trackerFile->close();
        ui->BtnTracker->setText(QString("ایجاد دنباله ی جدید"));
    }
}

void GPSMain::on_trackerTimer_Timeout()
{
    if(trackerFile->isOpen())
    {
        QDATEJALALI jalali;
        jalali.ProcNow();
        QString Data;
        IDCounter++;
        Data = QString("%1,%2,%3,%4/%5/%6 %7:%8:%9\n").arg(IDCounter).arg(Longitude).arg(latitude).arg(jalali.getyearsh())
                .arg(jalali.getmonthsh())
                .arg(jalali.getdaysh())
                .arg(QTime::currentTime().hour())
                .arg(QTime::currentTime().minute())
                .arg(QTime::currentTime().second());
        *trackerTextStream << Data;
    }
    else
    {
        trackerTimer->stop();
        trackerFile->close();
        ui->BtnTracker->setText(QString("ایجاد دنباله ی جدید"));
    }
}

void GPSMain::on_DspbTracker_valuechanged(double value)
{
    value *= 1000;
    trackerTimer->setInterval(value);
}

void GPSMain::on_Btn_Locations_clicked()
{
    if(LocationWindow)
        delete LocationWindow;
    LocationWindow = new Locations(this);
    LocationWindow->setData(&Path,&Longitude,&latitude,&Hight,&HAC,&GSPeed,&Direction,&Magvar);
    LocationWindow->show();
}
