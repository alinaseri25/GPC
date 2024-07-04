#ifndef LOCATIONS_H
#define LOCATIONS_H

#include <QFile>
#include <QMainWindow>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QTableView>
#include <QTimer>
#include <QGeoCoordinate>
#include <QTableWidget>

namespace Ui {
class Locations;
}

class Locations : public QMainWindow
{
    Q_OBJECT

public:
    explicit Locations(QWidget *parent = 0);
    ~Locations();
    void setData(QString *_Path,QString *_Longitude,QString *_latitude,QString *_Hight,QString *_HAC,
                 QString *_GSPeed,QString *_Direction,QString *_Magvar);

private slots:
    void on_Btn_Exit_clicked();

    void on_Btn_Reset_clicked();

    void on_Btn_Rename_clicked();

    void on_Btn_Save_clicked();

    void on_Btn_Cancel_clicked();

    void on_Btn_Distance_clicked();

    void on_Btn_DistanceCancel_clicked();

    void on_Btn_SelectFromTableS_clicked();

    void on_Btn_SelectMyLocationS_clicked();

    void on_Btn_SelectFromTableD_clicked();

    void on_Btn_SelectMyLocationD_clicked();

    void on_Btn_DistanceCalculate_clicked();

    void on_Btn_Send_clicked();

    void on_Tbw_Locations_itemSelectionChanged();

private:
    Ui::Locations *ui;
    QFile *LogFile;
    QString *Path;

    int SelectedId;

    QString *Longitude,*latitude,*Hight,*HAC,*GSPeed,*Direction,*Magvar;

    QSqlDatabase sqlDatabase;

    QSqlQueryModel *_Model;

    QGeoCoordinate *geoCoordinate;
    QGeoCoordinate *geoCoordinateOther;

    QTimer *TimerCurrentDistance,*TimerCurrentSource;

    void CreateTable(void);
};

#endif // LOCATIONS_H
