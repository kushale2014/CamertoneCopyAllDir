#include "copydialog.h"
#include "ui_copydialog.h"
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QSettings>
#include <QFileDialog>
#include <QProgressDialog>

CopyDialog::CopyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CopyDialog)
{
    ui->setupUi(this);

    QSettings settings("config.ini", QSettings::IniFormat);
    ui->dir_select_LE->setText(settings.value("path_store").toString());

    usemodel_left = new DataModel();
    usemodel_right = new DataModel();

    connect(ui->right_all_PB, SIGNAL(clicked()), this, SLOT(to_right_all()));
    connect(ui->right_one_PB, SIGNAL(clicked()), this, SLOT(to_right_one()));
    connect(ui->left_one_PB, SIGNAL(clicked()), this, SLOT(to_left_one()));
    connect(ui->left_all_PB, SIGNAL(clicked()), this, SLOT(to_left_all()));
    connect(ui->tableView_left, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(to_right_one()));
    connect(ui->tableView_right, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(to_left_one()));
    connect(ui->dir_select_PB, SIGNAL(clicked()), this, SLOT(dir_select()));
    connect(ui->store_PB, SIGNAL(clicked()), this, SLOT(store_files()));

    QFont font("Helvetica", 9, QFont::Bold);
    ui->tableView_left->horizontalHeader()->setFont(font);
    ui->tableView_left->horizontalHeader()->setSortIndicator(1, Qt::AscendingOrder);
    ui->tableView_right->horizontalHeader()->setFont(font);
    ui->tableView_right->horizontalHeader()->setSortIndicator(1, Qt::AscendingOrder);

    LoadData();
}

CopyDialog::~CopyDialog()
{
    delete ui;
}

void CopyDialog::LoadData() {
    LoadData_left();
    LoadData_right();
}

void CopyDialog::LoadData_left()
{
    qu = "SELECT id, number, reason, room FROM Files WHERE id IN (SELECT id From Arhiv WHERE pr = 1)";
    usemodel_left->setQuery(qu);
    if (usemodel_left->lastError().isValid()) {
        QMessageBox::warning(this, "Ошибка!!!", usemodel_left->lastError().databaseText());
        return;
    }
    while (usemodel_left->canFetchMore()) {
         usemodel_left->fetchMore();
    }

    SetModelsParam(usemodel_left);
    ui->tableView_left->setModel(usemodel_left);
    ui->tableView_left->hideColumn(0);
    ui->tableView_left->resizeColumnsToContents();
}

void CopyDialog::LoadData_right()
{
    qu = "SELECT id, number, reason, room FROM Files WHERE id IN (SELECT id From Arhiv WHERE pr = 2)";
    usemodel_right->setQuery(qu);
    if (usemodel_right->lastError().isValid()) {
        QMessageBox::warning(this, "Ошибка!!!", usemodel_right->lastError().databaseText());
        return;
    }
    while (usemodel_right->canFetchMore()) {
         usemodel_right->fetchMore();
    }

    SetModelsParam(usemodel_right);
    ui->tableView_right->setModel(usemodel_right);
    ui->tableView_right->hideColumn(0);
    ui->tableView_right->resizeColumnsToContents();
}

void CopyDialog::SetModelsParam(DataModel*& usemodel) {
    int ii = 0;

    ii++;
    usemodel->setHeaderData(ii, Qt::Horizontal, "Номер справи");
    usemodel->setHeaderData(ii, Qt::Horizontal, QSize(100, 24), Qt::SizeHintRole);
    usemodel->setColor(ii, "red");
    usemodel->setPointSize(ii, 9);

    ii++;
    usemodel->setHeaderData(ii, Qt::Horizontal, "За обвинуваченням\nза позовом");
    usemodel->setHeaderData(ii, Qt::Horizontal, QSize(140, 36), Qt::SizeHintRole);
    usemodel->setHAlign(ii, "left");
//    usemodel->setPointSize(ii, 10);

    ii++;
    usemodel->setHeaderData(ii, Qt::Horizontal, "Номер\nзали");
    usemodel->setHeaderData(ii, Qt::Horizontal, QSize(50, 36), Qt::SizeHintRole);
    usemodel->setHAlign(ii, "center");
//    usemodel->setPointSize(ii, 10);
}

void CopyDialog::to_right_all() {
    qu = "UPDATE Arhiv SET pr = 2";
    sqlquery.exec(qu);
    LoadData();
}

void CopyDialog::to_right_one() {
    useindex = ui->tableView_left->currentIndex();
    QString str_id = usemodel_left->data(usemodel_left->index(useindex.row(), 0), 0).toString();
    if (str_id.isEmpty()) return;

    int id = usemodel_left->data(usemodel_left->index(useindex.row(), 0), 0).toInt();
    qu = "UPDATE Arhiv SET pr = 2 WHERE id = %1";
    qu = qu.arg(id);
    sqlquery.exec(qu);
    LoadData();
}

void CopyDialog::to_left_one() {
    useindex = ui->tableView_right->currentIndex();
    QString str_id = usemodel_right->data(usemodel_right->index(useindex.row(), 0), 0).toString();
    if (str_id.isEmpty()) return;

    int id = usemodel_right->data(usemodel_right->index(useindex.row(), 0), 0).toInt();
    qu = "UPDATE Arhiv SET pr = 1 WHERE id = %1";
    qu = qu.arg(id);
    sqlquery.exec(qu);
    LoadData();
}

void CopyDialog::to_left_all() {
    qu = "UPDATE Arhiv SET pr = 1";
    sqlquery.exec(qu);
    LoadData();
}

void CopyDialog::dir_select()
{
    QString ddd = ui->dir_select_LE->text();
    if (ddd.isEmpty()) ddd = QDir::currentPath();
    QString dir = QFileDialog::getExistingDirectory(this, "Выбор директории",
                                                    ddd,
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if (dir!="") {
        ui->dir_select_LE->setText(dir);
        QSettings settings("config.ini", QSettings::IniFormat);
        settings.setValue("path_store", dir);
    }
}

QString CopyDialog::getZipName() {
    qu = "SELECT number FROM Files WHERE id IN (SELECT id From Arhiv WHERE pr = 2)"
            " Group By number Order By 1";
    sqlquery.exec(qu);
    QString name = "";
    while (sqlquery.next()) {
        QString number = sqlquery.value(0).toString();
        number.replace('/', '-').replace('\\', '-').replace('"', '-').replace('*', '-').replace(':', '-').replace('?', '-').replace('<', '-').replace('>', '-').replace('|', '-');
        if (!name.isEmpty()) name += "!-!";
        name += number;
    }
    return name + ".zip";
}

void CopyDialog::store_files() {

    QString zipdir = ui->dir_select_LE->text();
    if (zipdir.isEmpty()) {
        QMessageBox::warning(this, "Ошибка!!!", "Выберите директорию для сохранения.");
        return;
    }
    qu = "SELECT COUNT(id) from Arhiv Where pr = 2";
    sqlquery.exec(qu);
    if (!sqlquery.first()) return;
    int countFiles = sqlquery.value(0).toInt();
    if (countFiles==0) return;

    QString zipname = getZipName();
    if (zipname.length()>255) {
        QMessageBox::warning(this, "Ошибка!!!", "Слишком много выбрано файлов.");
        return;
    }
    QZipWriter zip(zipdir + "/" + zipname);
    if (zip.status() != QZipWriter::NoError)
       return;
    zip.setCompressionPolicy(QZipWriter::AutoCompress);

    QProgressDialog progress("Запись файлов...", "Прервать запись", 0, countFiles, this);
    progress.setWindowModality(Qt::WindowModal);
    qu = "SELECT fullname FROM Files WHERE id IN (SELECT id From Arhiv WHERE pr = 2)";
    sqlquery.exec(qu);
    QString fullname;
    int ii = 0; int pp = 0;
    while (sqlquery.next()) {
        progress.setValue(ii);
        if (progress.wasCanceled()) {
                pp = -1;
                break;
        }
        fullname = sqlquery.value(0).toString();
        QString file_name = fullname.section('/', -1);
        QFile file(fullname);
        if (file.open(QIODevice::ReadOnly)) {
            pp++;
            zip.setCreationPermissions(QFile::permissions(fullname));
            zip.addFile(file_name, file.readAll());
            file.close();
            store_dirs(zip, fullname);
        }
        ii++;
    }
    progress.setValue(countFiles);
    if (pp == countFiles) {
        QList<QString> list;
        list << "autorun.inf" << "camertone.ico" << "camertonecd.exe";
        for (int i = 0; i < list.size(); ++i) {
            QFile file(":/src/" + list.at(i));
            file.open(QIODevice::ReadOnly);
            zip.addFile(list.at(i), file.readAll());
            file.close();
        }
        zip.close();
        QMessageBox::information(this, "Сообщение", "Все выбранные дела успешно записаны.");
        return;
    }  else {
        zip.close();
        QFile file(zipdir + "/" + zipname);
        file.remove();
        QString mes;
        if (pp == -1) mes = "Запись была прервана.";
        else mes =  "Ошибка при записи.";
        QMessageBox::critical(this, "Ошибка!!!", mes);
    }
}

void CopyDialog::store_dirs(QZipWriter& zip, QString path) {
    path.chop(4);
    QString dir_name = path.section('/', -1);
    path += "/"; // в данном случае завершающий '/' очень важен
    QDirIterator it(path, QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot,
                      QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString file_path = it.next();
        if (it.fileInfo().isDir()) {
            zip.setCreationPermissions(QFile::permissions(file_path));
            zip.addDirectory(dir_name + '/' + file_path.remove(path));
        } else if (it.fileInfo().isFile()) {
            QFile file(file_path);
            if (!file.open(QIODevice::ReadOnly))
              continue;

            zip.setCreationPermissions(QFile::permissions(file_path));
            QByteArray ba = file.readAll();
            zip.addFile(dir_name + '/' + file_path.remove(path), ba);
            file.close();
        }
    }
}
