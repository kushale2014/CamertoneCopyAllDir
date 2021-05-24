#ifndef COPYDIALOG_H
#define COPYDIALOG_H

#include <QDialog>
#include <QSqlQuery>
#include "datamodel_cc.h"
#include "qzipwriter_p.h"

namespace Ui {
class CopyDialog;
}

class CopyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CopyDialog(QWidget *parent = 0);
    ~CopyDialog();

private slots:
    void to_right_all();
    void to_right_one();
    void to_left_one();
    void to_left_all();
    void dir_select();
    void store_files();
private:
    Ui::CopyDialog *ui;
    QSqlQuery sqlquery; QString qu;
    DataModel *usemodel_left;
    DataModel *usemodel_right;
    QModelIndex useindex;
    void LoadData();
    void LoadData_left();
    void LoadData_right();
    void SetModelsParam(DataModel*&);
    QString getZipName();
    void store_dirs(QZipWriter&, QString);
};

#endif // COPYDIALOG_H
