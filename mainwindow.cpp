#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialoginfo.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QSettings>
#include <QDebug>
#include <QCloseEvent>
#include <QProgressDialog>
#include <QSqlRecord>
#include <QSqlError>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowState(Qt::WindowMaximized);

    usemodel = new DataModel();

    createTable("Subdir");
    createTable("Files");
    createTable("Arhiv");

    QSettings settings("config.ini", QSettings::IniFormat);
    QString path = settings.value("path").toString();
    if (path.isEmpty()) path = QDir::currentPath();
    currentDir = path;
    ui->selectDir_LE->setText(currentDir);

    dirview_widget = new SelectDir(currentDir);
    ui->verticalLayout_dir->addWidget(dirview_widget);

    connect(ui->filter1_LE, SIGNAL(textChanged(QString)), this, SLOT(textChanged()));
    connect(ui->filter2_LE, SIGNAL(textChanged(QString)), this, SLOT(textChanged()));
    connect(ui->filter3_CB, SIGNAL(currentIndexChanged(int)), this, SLOT(textChanged()));
    connect(ui->filterClear1_TB, SIGNAL(clicked()), this, SLOT(clearFilter1()));
    connect(ui->filterClear2_TB, SIGNAL(clicked()), this, SLOT(clearFilter2()));
    connect(ui->filterClear3_TB, SIGNAL(clicked()), this, SLOT(clearFilter3()));
    connect(ui->filterClearAll_TB, SIGNAL(clicked()), this, SLOT(clearFilterAll()));
    connect(ui->createArhiv_PB, SIGNAL(clicked()), this, SLOT(createArhiv()));
    connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(show_name()));
    connect(ui->tableView->horizontalHeader(),SIGNAL(sectionClicked(int)), this, SLOT(sortByColumn(int)));
    connect(dirview_widget, SIGNAL(update()), this, SLOT(update()));

    QFont font("Helvetica", 9, QFont::Bold);
    ui->tableView->horizontalHeader()->setFont(font);
    ui->tableView->horizontalHeader()->setSortIndicator(1, Qt::AscendingOrder);

    ui->statusbar->addWidget(new QLabel("Всего строк: ", this));
    vsego = new QLabel(this);
    ui->statusbar->addWidget(vsego);

    LoadData();

    usemodel_dirname = new QSqlQueryModel();
    setInit_dirname();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
}

void MainWindow::createTable(QString table) {

    qu = "";
    if (table=="Subdir") qu = "CREATE TABLE IF NOT EXISTS `Subdir` ("
            " `id` INTEGER PRIMARY KEY AUTOINCREMENT,"
            " `title` TEXT, `pr` INTEGER DEFAULT 1)";
    if (table=="Files") qu = "CREATE TABLE IF NOT EXISTS `Files` ("
            " `id` INTEGER PRIMARY KEY AUTOINCREMENT,"
            " `fullname` TEXT, `number` TEXT, `inputdate` TEXT, `reason` TEXT,"
            " `room` TEXT, `lastchange` TEXT, `dirname` TEXT, `pr_report` TEXT)";
    if (table=="Arhiv") qu = "CREATE TABLE IF NOT EXISTS `Arhiv` ("
            " `id` INTEGER PRIMARY KEY AUTOINCREMENT,"
            " `pr` INTEGER DEFAULT 1)";
    if (!qu.isEmpty()) sqlquery.exec(qu);

}

void MainWindow::show_name() {
    useindex = ui->tableView->currentIndex();
    QString str_id = usemodel->data(usemodel->index(useindex.row(), 0), 0).toString();
    if (str_id.isEmpty()) return;

    QString fullname = usemodel->data(usemodel->index(useindex.row(), 1), 0).toString();
    DialogInfo *dlg = new DialogInfo(this);
    dlg->setPath(fullname.section('/',0,-2));
    dlg->setFile(fullname.section('/',-1));
    dlg->exec();
    delete dlg;
}

void MainWindow::textChanged() {
    LoadData();
}

void MainWindow::clearFilter1()  { ui->filter1_LE->setText(""); }
void MainWindow::clearFilter2()  { ui->filter2_LE->setText(""); }
void MainWindow::clearFilter3()  { ui->filter3_CB->setCurrentIndex(-1); }

void MainWindow::clearFilterAll()
{
    clearFilter1();
    clearFilter2();
    clearFilter3();
}

QString MainWindow::getFromTag(QString &line, QString naimtag) {
    QString ttt = "</"+naimtag+">";
    if (line.contains(ttt)) {
        QString str = line.section(ttt, 0, 0);
        str = str.section("<"+naimtag, 1);
        str = str.section(">", 1);
        if (naimtag=="InputDate" || naimtag=="LastChange") {
            QString dd, tt;
            dd = str.section("T", 0, 0);
            tt = str.section("T", -1);
            tt = tt.section(".", 0, 0);
            str = dd + " " + tt;
        }
        if (str.contains("\'")) str.replace('\'','"');
        return str;
    } else return "";
}

void MainWindow::update() {

    sqlquery.exec("SELECT COUNT(title) from Subdir Where pr = 2");
    if (!sqlquery.first()) return;
    if (sqlquery.value(0).toInt()==0) {
        QMessageBox::warning(this, "Ошибка!!!", "Выберите директорию для обновления.");
        return;
    }

    sqlquery.exec("SELECT COUNT(title) from Subdir Where pr = 1");
    sqlquery.first();
    all_dirs_to_update = ( sqlquery.value(0).toInt() == 0 )  ? true :  false;
    if (all_dirs_to_update) {
        sqlquery.exec("DROP TABLE Files");
        createTable("Files");
    } else {
        sqlquery.exec("DELETE from Files WHERE dirname IN (SELECT title from Subdir Where pr = 2)");
    }
    sqlquery.exec("SELECT title from Subdir Where pr = 2");
    while (sqlquery.next()) {
        QString subdir =  sqlquery.value(0).toString();
        if ( !scanedFiles_dir(currentDir + "/" + subdir) ) {
            break;
        }
    }
    LoadData();
    setInit_dirname();
}

bool MainWindow::scanedFiles_dir(QString dir_name_full) {
    QDir dir(dir_name_full);
    QStringList lstFiles = dir.entryList(QDir::Files);
    int count = lstFiles.count();
    int ii = 0;
    QProgressDialog progress("Поиск файлов...", "Прервать поиск", 0, count, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setLabelText(QString("Директория:  ") + dir_name_full);

    QSqlQuery  query;
    QSqlDatabase::database().transaction();
    foreach (QString fileName, lstFiles)
    {
        if (fileName.endsWith(".cmt")) {
            progress.setValue(ii);
            if (progress.wasCanceled()) {
                    ii = -1;
                    break;
            }
            QString fullname = dir_name_full+"/"+fileName;
            QFile file(fullname);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                in.setCodec("UTF-8");
                QString line = in.readLine();
                file.close();
                QString dir_report_path = fullname;
                dir_report_path.chop(4);
                dir_report_path += "/report";
                QDir dir(dir_report_path);
                QString dir_report = dir.exists() ? "есть" : "нет";
                qu = "INSERT into Files(fullname, number, inputdate, reason, room, lastchange, dirname, pr_report)"
                        " values('%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8')";
                qu = qu.arg(fullname).arg(getFromTag(line, "Number")).arg(getFromTag(line, "InputDate"))
                        .arg(getFromTag(line, "Reason")).arg(getFromTag(line, "Room"))
                        .arg(getFromTag(line, "LastChange")).arg(dir_name_full.section('/', -1)).arg(dir_report);
                query.exec(qu);
            }
        }
        ii++;
    }
//    progress.setValue(count);
    if (ii == -1) {
        QSqlDatabase::database().rollback();
        return false;
    } else {
        QSqlDatabase::database().commit();
        return true;
    }
}

void MainWindow::sortByColumn(int nn) {
    QString order = ui->tableView->horizontalHeader()->sortIndicatorOrder() == Qt::AscendingOrder ? "" : "DESC";
    useOrder = QString(" ORDER by %1 %2, 3 %2").arg(nn+1).arg(order);
    LoadData();
}

void MainWindow::createArhiv() {
    sqlquery.exec("DELETE from Arhiv");
    qu = "INSERT into Arhiv  SELECT id, 1 FROM Files";  getWhere(qu);
    sqlquery.exec(qu);

    CopyDialog *dlg = new CopyDialog(this);
    dlg->exec();
    delete dlg;
}

void MainWindow::setInit_dirname() {
    sqlquery.exec("SELECT dirname FROM Files GROUP by dirname ORDER by dirname");
    usemodel_dirname->setQuery(sqlquery);
    ui->filter3_CB->setModel(usemodel_dirname);
    ui->filter3_CB->setCurrentIndex(-1);
}

void MainWindow::getWhere(QString &qu) {
    qu +=  " WHERE 1";
    if (!ui->filter1_LE->text().isEmpty()) {  qu += QString(" and number LIKE '%%1%'").arg(ui->filter1_LE->text());  }
    if (!ui->filter2_LE->text().isEmpty()) {  qu += QString(" and reason LIKE '%%1%'").arg(ui->filter2_LE->text());  }
    if (!ui->filter3_CB->currentText().isEmpty()) {  qu += QString(" and dirname LIKE '%%1%'").arg(ui->filter3_CB->currentText());  }
}

void MainWindow::LoadData() {

    qu = "SELECT * from Files"; getWhere(qu);
    qu += useOrder;
    usemodel->setQuery(qu);
    if (usemodel->lastError().isValid()) {
        QMessageBox::warning(this, "Ошибка!!!", usemodel->lastError().databaseText());
        return;
    }
    while (usemodel->canFetchMore()) {
         usemodel->fetchMore();
    }

    SetModelsParam();
    ui->tableView->setModel(usemodel);
    ui->tableView->hideColumn(0);
    ui->tableView->hideColumn(1);
    ui->tableView->resizeColumnsToContents();
    vsego->setText(QString().setNum(usemodel->rowCount()));
}

void MainWindow::SetModelsParam() {
    int ii = 1;

    ii++;
    usemodel->setHeaderData(ii, Qt::Horizontal, "Номер справи");
    usemodel->setHeaderData(ii, Qt::Horizontal, QSize(150, 24), Qt::SizeHintRole);
    usemodel->setColor(ii, "red");
    usemodel->setPointSize(ii, 10);
    usemodel->setPrefix(ii, "   ");

    ii++;
    usemodel->setHeaderData(ii, Qt::Horizontal, "Дата та час\nзасідання");
    usemodel->setHeaderData(ii, Qt::Horizontal, QSize(150, 36), Qt::SizeHintRole);
    usemodel->setHAlign(ii, "center");
    usemodel->setPointSize(ii, 9);
    usemodel->setColor(ii, "blue");

    ii++;
    usemodel->setHeaderData(ii, Qt::Horizontal, "За обвинуваченням\nза позовом");
    usemodel->setHeaderData(ii, Qt::Horizontal, QSize(350, 36), Qt::SizeHintRole);
    usemodel->setHAlign(ii, "left");
    usemodel->setPointSize(ii, 10);

    ii++;
    usemodel->setHeaderData(ii, Qt::Horizontal, "Номер зали");
    usemodel->setHeaderData(ii, Qt::Horizontal, QSize(80, 36), Qt::SizeHintRole);
    usemodel->setHAlign(ii, "center");
    usemodel->setPointSize(ii, 10);

    ii++;
    usemodel->setHeaderData(ii, Qt::Horizontal, "Дата останньої\nзміни");
    usemodel->setHeaderData(ii, Qt::Horizontal, QSize(150, 36), Qt::SizeHintRole);
    usemodel->setHAlign(ii, "center");
    usemodel->setPointSize(ii, 10);
    usemodel->setColor(ii, "blue");

    ii++;
    usemodel->setHeaderData(ii, Qt::Horizontal, "Директория");
    usemodel->setHeaderData(ii, Qt::Horizontal, QSize(100, 36), Qt::SizeHintRole);
    usemodel->setColor(ii, "red");
    usemodel->setHAlign(ii, "center");
    usemodel->setPointSize(ii, 10);

    ii++;
    usemodel->setHeaderData(ii, Qt::Horizontal, "Report");
    usemodel->setHeaderData(ii, Qt::Horizontal, QSize(80, 36), Qt::SizeHintRole);
    usemodel->setHAlign(ii, "center");
}

