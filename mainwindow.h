#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "datamodel_cc.h"
#include "selectdir.h"
#include "copydialog.h"
#include <QSqlQuery>
#include <QLabel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
private slots:
    void update();
    void sortByColumn(int);
    void textChanged();
    void clearFilter1(); void clearFilter2(); void clearFilter3();
    void clearFilterAll();
    void show_name();
    void createArhiv();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    QWidget *dirview_widget;
    QString qu;    QSqlQuery sqlquery;
    QString useOrder;
    bool all_dirs_to_update;
    void moveFiles();
    void createDirMoved();
    int moveToDir(QString);
    QString currentDir;
    DataModel *usemodel;
    QModelIndex useindex;
    QString getFromTag(QString&, QString);
    bool scanedFiles_dir(QString);
    void getWhere(QString&);
    void LoadData();
    void SetModelsParam();
    QSqlQueryModel *usemodel_dirname;
    void setInit_dirname();
    QLabel *vsego;
    void createTable(QString);
};

#endif // MAINWINDOW_H
