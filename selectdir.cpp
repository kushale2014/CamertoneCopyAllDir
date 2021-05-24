#include "selectdir.h"
#include "ui_selectdir.h"
#include <QSettings>
#include <QDir>
#include <QSqlError>
#include <QMessageBox>

SelectDir::SelectDir(QString currentDir, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SelectDir)
{
    ui->setupUi(this);
    sqlquery.exec("DELETE from Subdir");

    QDir dir(currentDir);
    dir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs );
    dir.setSorting(QDir::Name);
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i)
    {
         QFileInfo fileInfo = list.at(i);
         QString name = fileInfo.fileName();
         if (name.length() < 20) {
             qu = QString("INSERT into Subdir(id, title) values('%1', '%2')").arg(i).arg(name);
             sqlquery.exec(qu);
         }
    }

    usemodel_left = new DataModel();
    usemodel_right = new DataModel();

    connect(ui->right_all_PB, SIGNAL(clicked()), this, SLOT(to_right_all()));
    connect(ui->right_one_PB, SIGNAL(clicked()), this, SLOT(to_right_one()));
    connect(ui->left_one_PB, SIGNAL(clicked()), this, SLOT(to_left_one()));
    connect(ui->left_all_PB, SIGNAL(clicked()), this, SLOT(to_left_all()));
    connect(ui->tableView_left, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(to_right_one()));
    connect(ui->tableView_right, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(to_left_one()));
    connect(ui->update_PB, SIGNAL(clicked()), this, SIGNAL(update()));

    QFont font("Helvetica", 9, QFont::Bold);
    ui->tableView_left->horizontalHeader()->setFont(font);
    ui->tableView_left->horizontalHeader()->setSortIndicator(1, Qt::AscendingOrder);
    ui->tableView_right->horizontalHeader()->setFont(font);
    ui->tableView_right->horizontalHeader()->setSortIndicator(1, Qt::AscendingOrder);

    LoadData();

}

void SelectDir::LoadData() {
    LoadData_left();
    LoadData_right();
}

void SelectDir::LoadData_left()
{
    qu = "SELECT id, title from Subdir Where pr = 1";

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

void SelectDir::LoadData_right()
{
    qu = "SELECT id, title from Subdir Where pr = 2";

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

void SelectDir::SetModelsParam(DataModel*& usemodel) {
    int ii = 0;

    ii++;
    usemodel->setHeaderData(ii, Qt::Horizontal, "Директория");
    usemodel->setHeaderData(ii, Qt::Horizontal, QSize(250, 24), Qt::SizeHintRole);
    usemodel->setColor(ii, "blue");
    usemodel->setPointSize(ii, 10);

}

void SelectDir::to_right_all() {
    qu = "UPDATE Subdir SET pr = 2";
    sqlquery.exec(qu);
    LoadData();
}

void SelectDir::to_right_one() {
    useindex = ui->tableView_left->currentIndex();
    QString str_id = usemodel_left->data(usemodel_left->index(useindex.row(), 0), 0).toString();
    if (str_id.isEmpty()) return;

    int id = usemodel_left->data(usemodel_left->index(useindex.row(), 0), 0).toInt();
    qu = "UPDATE Subdir SET pr = 2 WHERE id = %1";
    qu = qu.arg(id);
    sqlquery.exec(qu);
    LoadData();
}

void SelectDir::to_left_one() {
    useindex = ui->tableView_right->currentIndex();
    QString str_id = usemodel_right->data(usemodel_right->index(useindex.row(), 0), 0).toString();
    if (str_id.isEmpty()) return;

    int id = usemodel_right->data(usemodel_right->index(useindex.row(), 0), 0).toInt();
    qu = "UPDATE Subdir SET pr = 1 WHERE id = %1";
    qu = qu.arg(id);
    sqlquery.exec(qu);
    LoadData();
}

void SelectDir::to_left_all() {
    qu = "UPDATE Subdir SET pr = 1";
    sqlquery.exec(qu);
    LoadData();
}

SelectDir::~SelectDir()
{
    delete ui;
}
