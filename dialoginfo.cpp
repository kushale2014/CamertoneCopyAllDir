#include "dialoginfo.h"
#include "ui_dialoginfo.h"

DialogInfo::DialogInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogInfo)
{
    ui->setupUi(this);
}

DialogInfo::~DialogInfo()
{
    delete ui;
}

void DialogInfo::setPath(QString path)
{
    ui->path_LE->setText(path);
}

void DialogInfo::setFile(QString file)
{
    ui->file_LE->setText(file);
}

