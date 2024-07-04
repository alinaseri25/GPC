#ifndef GPSMAIN_H
#define GPSMAIN_H

#include <QMainWindow>
#include <QtCore>
#include <QtPositioning/QGeoSatelliteInfoSource>
#include <QtPositioning/QGeoPositionInfoSource>
#include <QMessageBox>
#include <QClipboard>
#include <../AndroidNotification/notificationclient.h>
#include <QFile>
#include <QTextStream>
#include <QMenuBar>
#include <qompasswindow.h>
//#include <QSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlDriver>
#include <QSqlDriverPlugin>
#include <locations.h>
#include <QTimer>
#include <QTime>
#include <QDoubleSpinBox>

namespace Ui {
class GPSMain;
}

class GPSMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit GPSMain(QWidget *parent = 0);
    ~GPSMain();

private:
    Ui::GPSMain *ui;
    QGeoSatelliteInfoSource *QgeoSatelliteInfoSource;
    QGeoPositionInfoSource *QgeoPositionInfoSource;
    QString SatelliteType(int StType);
    int InUse,InView;
    QStringList StringListInView,StringLisInUse;
    bool SatSet;
    QString Longitude,latitude,Hight,HAC,GSPeed,Direction,Magvar;
    QString DirectionSet(QString _Data);
    QFile *LogFile,*trackerFile;
    QTextStream *LogTextStream,*trackerTextStream;
    int IDCounter;

    void ChangeBtnState(bool state);
    void changelocationNameBar(bool state);
    int LocationNumber;


    QompassWindow *QompWindow;
    Locations *LocationWindow;

    QMenu *Equipment;
    QAction *Qompass;
    QSqlDatabase sqlDatabase;
    QString Path,trackerPath;

    QTimer *trackerTimer;

private slots:
    void error(QGeoSatelliteInfoSource::Error error);
    void satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &infos);
    void satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &infos);
    void setGPSLocation(QGeoPositionInfo geoPositionInfo);
    void error(QGeoPositionInfoSource::Error error);
    void on_Btn_Exit_clicked();
    void on_Btn_Send_clicked();
    void on_Btn_SaveLocation_clicked();
    void on_Btn_SaveLocationEnd_clicked();
    void on_Btn_Cancel_clicked();
    void on_Qompass_clicked();
    void on_BtnTracker_Clicked(void);
    void on_trackerTimer_Timeout(void);
    void on_DspbTracker_valuechanged(double value);

    void on_Btn_Locations_clicked();


signals:
    void QompassChange(int degree);
};

#endif // GPSMAIN_H
