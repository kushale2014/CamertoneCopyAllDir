#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <QSqlQueryModel>
#include <QSize>

class DataModel : public QSqlQueryModel {
public:
    DataModel();
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QMap<QString, QVariant> map;
    void setDouble(int col, int value) {map.insert(QString("d%1").arg(col), value);};
    void setSize(int col, QSize value) {map.insert(QString("s%1").arg(col), value);};
    void setHAlign(int col, QString value) {map.insert(QString("H%1").arg(col), value);};
    void setVAlign(int col, QString value) {map.insert(QString("V%1").arg(col), value);};
    void setPointSize(int col, int value) {map.insert(QString("PS%1").arg(col), value);};
    void setBold(int col) {map.insert(QString("W%1").arg(col), "bold");};
    void setColor(int col, QString value) {map.insert(QString("color%1").arg(col), value);};
    void setFamilyFont(int col, QString value) {map.insert(QString("family%1").arg(col), value);};
    void setDate(int col, QString value) {map.insert(QString("date%1").arg(col), value);};
    void setPrefix(int col, QString value) {map.insert(QString("prefix%1").arg(col), value);};
};

#endif
