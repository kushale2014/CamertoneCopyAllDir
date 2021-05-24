#include "datamodel_cc.h"
#include <QColor>
#include <QFont>

DataModel::DataModel() : QSqlQueryModel() {
}

QVariant DataModel::data(
                const QModelIndex &index,
                int role) const {
    
        QVariant value = QSqlQueryModel::data(index, role);
        int col = index.column();
        QString str;
    
        switch (role) {
    
        case Qt::DisplayRole: // Данные для отображения
                  str = QString("d%1").arg(col);
                  if (map.contains(str)) { 
                      int nn = map.value(str).toInt();
                      QString ss = value.toString();
                      if (!ss.contains('.')) ss = ss.append('.');
                      ss += QString(nn, '0');
                      return ss.left(ss.indexOf('.')+1+nn);
                  }
                  str = QString("date%1").arg(col);
                  if (map.contains(str)) {
                      QString ss = value.toString();
                      return QString("%1-%2-%3").arg(ss.mid(8,2)).arg(ss.mid(5,2)).arg(ss.mid(0,4));
                  }
                  str = QString("prefix%1").arg(col);
                  if (map.contains(str)) {
                      QString ss = value.toString();
                      QString dd = map.value(str).toString();
                      return (dd + ss);
                  }
            break;

        case Qt::EditRole:    // Данные для редактирования
                return value;
    
        case Qt::TextColorRole: {// Цвет текста
            str = QString("color%1").arg(col);
            if (map.contains(str)) {
                QColor color;
                color.setNamedColor(map.value(str).toString());
                return qVariantFromValue(color);
            }
            else 
                return value;
            }
    
        case Qt::TextAlignmentRole: {// Выравнивание
            int H; int V;
            str = QString("V%1").arg(col);
            V = 0x0080;
            if (map.contains(str)){
                if (map.value(str) == "top") V=0x0020;
                if (map.value(str) == "bottom") V=0x0040;
            }
            H = 0x0001;
            str = QString("H%1").arg(col);
            if (map.contains(str)){
                if (map.value(str) == "right") H=0x0002;
                if (map.value(str) == "center") H=0x0004;
                if (map.value(str) == "justify") H=0x0008;
            }
            return (H | V);
        }
        
        case Qt::FontRole: {// Шрифт
            QFont font; QString family; int PS; QString W;
            str = QString("family%1").arg(col);
            family = map.value(str, "Helvetica").toString();
            font.setFamily(family);
            str = QString("PS%1").arg(col);
            PS = map.value(str, 8).toInt();
            font.setPointSize(PS);
            str = QString("W%1").arg(col);
            W = map.value(str, "").toString();
            if (W == "bold") font.setBold(true);
            return qVariantFromValue(font);
         }
   
        case Qt::BackgroundColorRole: {  // Цвет фона
            int a = (index.row() % 2) ? 14 : 0;
            return qVariantFromValue(QColor(220,240-a,230-a));
        }
    
        case Qt::SizeHintRole: { // Размер ячейки
            str = QString("s%1").arg(col);
            return map.value(str).toSize();
        }
    
    }
    return value;
}
