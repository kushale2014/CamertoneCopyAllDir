#ifndef SELECTDIR_H
#define SELECTDIR_H

#include <QWidget>
#include <QSqlQuery>
#include "datamodel_cc.h"

namespace Ui {
class SelectDir;
}

class SelectDir : public QWidget
{
    Q_OBJECT

public:
    explicit SelectDir(QString currentDir, QWidget *parent = 0);
    ~SelectDir();

private slots:
    void to_right_all();
    void to_right_one();
    void to_left_one();
    void to_left_all();

signals:
    update();

private:
    Ui::SelectDir *ui;
    QSqlQuery sqlquery; QString qu;
    DataModel *usemodel_left;
    DataModel *usemodel_right;
    QModelIndex useindex;
    void LoadData();
    void LoadData_left();
    void LoadData_right();
    void SetModelsParam(DataModel*&);

};

#endif // SELECTDIR_H
