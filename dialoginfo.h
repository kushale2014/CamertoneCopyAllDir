#ifndef DIALOGINFO_H
#define DIALOGINFO_H

#include <QDialog>

namespace Ui {
class DialogInfo;
}

class DialogInfo : public QDialog
{
    Q_OBJECT

public:
    explicit DialogInfo(QWidget *parent = 0);
    ~DialogInfo();

private:
    Ui::DialogInfo *ui;
public slots:
    void setPath(QString);
    void setFile(QString);
};

#endif // DIALOGINFO_H
