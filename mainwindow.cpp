#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSql>
#include <QInputDialog>
#include <QMessageBox>
#include <vector>
#include <random>
#include <QStyle>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("zerorand");
    if (!db.open()) {
        qDebug() << db.lastError().text();
    }
    ui->setupUi(this);
    this->setGeometry(

       QStyle::alignedRect(
           Qt::LeftToRight,
           Qt::AlignCenter,
           this->size(),
           QGuiApplication::screens().at(0)->availableGeometry()
       )
    );
    initDB(db);
    updateDB(db);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initDB(QSqlDatabase db)
{
    QSqlQuery query(db);
    query.exec("create table if not exists lists (id INTEGER PRIMARY KEY, list TEXT UNIQUE NOT NULL)");
    query.exec("create table if not exists points (id INTEGER PRIMARY KEY, point TEXT NOT NULL, list_id INTEGER NOT NULL)");

}

void MainWindow::updateDB(QSqlDatabase db)
{
    ui->list_Main->clear();
    ui->cb_chooseDB->clear();
    QSqlQuery query(db);
    query.exec("SELECT * FROM lists");
    while (query.next()){
        ui->cb_chooseDB->addItem(query.value(1).toString());
    }
    changeState(false);
    ui->del_point->setDisabled(true);
}

void MainWindow::changeState(bool state)
{
    ui->le_addtoDB->setDisabled(!state);
    ui->pb_addtoDB->setDisabled(!state);
    ui->pb_random->setDisabled(!state);
    ui->del_list->setDisabled(!state);
}


void MainWindow::on_pb_addDB_clicked()
{
    QString dbname =  QInputDialog::getText(this,"Добавление списка", "Введите название добавляемого списка:");
    if (!dbname.isEmpty()){
        QSqlQuery query("INSERT INTO lists VALUES (null, '"+dbname+"')");
        updateDB(QSqlDatabase::database());
        ui->cb_chooseDB->setCurrentIndex(ui->cb_chooseDB->count()-1);
        on_cb_chooseDB_activated(ui->cb_chooseDB->count()-1);
    }
}

void MainWindow::on_cb_chooseDB_activated(int index)
{
    Q_UNUSED(index);
    ui->list_Main->clear();
    QSqlQuery query("SELECT id FROM lists WHERE(list = '"+ui->cb_chooseDB->currentText()+"')");
    query.first();
    QString list = query.value(0).toString();
    query.exec("SELECT point FROM points WHERE(list_id = "+list+")");
    while (query.next()){
        ui->list_Main->addItem(query.value(0).toString());
    }
    changeState(true);

}

void MainWindow::on_pb_addtoDB_clicked()
{
    QString pos_name = ui->le_addtoDB->text();
    if (!pos_name.isEmpty()){
        QSqlQuery query("SELECT id FROM lists WHERE(list = '"+ui->cb_chooseDB->currentText()+"')");
        query.first();
        QString list = query.value(0).toString();
        query.exec("INSERT INTO points VALUES (null, '"+pos_name+"', "+list+")");
        ui->list_Main->addItem(pos_name);
        ui->le_addtoDB->clear();
    }

}

void MainWindow::on_del_list_triggered()
{
    if (QMessageBox::question(this, "Удаление списка", "Вы действительно хотите удалить активный список?") == QMessageBox::StandardButton::Yes){
        QSqlQuery query("SELECT id FROM lists WHERE(list = '"+ui->cb_chooseDB->currentText()+"')");
        query.first();
        QString list = query.value(0).toString();
        query.exec("DELETE FROM lists WHERE id = "+list);
        query.exec("DELETE FROM points WHERE list_id ="+list);
        updateDB(QSqlDatabase::database());
        ui->cb_chooseDB->setCurrentIndex(ui->cb_chooseDB->count()-1);
        on_cb_chooseDB_activated(ui->cb_chooseDB->count()-1);
    }
}

void MainWindow::on_del_point_triggered()
{
     QSqlQuery query("SELECT id FROM lists WHERE(list = '"+ui->cb_chooseDB->currentText()+"')");
     query.first();
     QString list = query.value(0).toString();
     query.exec("DELETE FROM points WHERE point = '"+ui->list_Main->currentItem()->text()+"' AND list_id = "+list);
     qDebug()<<ui->list_Main->currentRow();
     if (ui->list_Main->count()==1) ui->list_Main->clear();
     else delete ui->list_Main->currentItem();
}

void MainWindow::on_list_Main_itemSelectionChanged()
{
    if(!ui->list_Main->currentItem()->text().isEmpty())ui->del_point->setDisabled(false);
}

void MainWindow::on_pb_random_clicked()
{
    std::vector<QString> randlist;
    for(int i = 0; i<ui->list_Main->count();++i){
        randlist.push_back(ui->list_Main->item(i)->text());
    }
    std::random_device dev;
    std::mt19937 r(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0,randlist.size()-1);
    ui->lbl_random->setText(randlist[dist(r)]);
}

void MainWindow::on_list_Main_itemDoubleClicked(QListWidgetItem *item)
{
    QString changename =  QInputDialog::getText(this,"Изменение пункта", "Введите новое название для пункта:",QLineEdit::Normal,item->text());
    if (!changename.isEmpty()){
        QSqlQuery query("SELECT id FROM lists WHERE(list = '"+ui->cb_chooseDB->currentText()+"')");
        query.first();
        QString list = query.value(0).toString();
        query.exec("INSERT INTO points VALUES (null, '"+changename+"', "+list+")");
        query.exec("DELETE FROM points WHERE point = '"+item->text()+"' AND list_id = "+list);
        item->setText(changename);
    }
}

void MainWindow::on_about_triggered()
{
    QMessageBox::about(this,"О программе","ZeroRand v1.2\nПрограмма для ведения списков с функцией рандомайзера.\nDeveloped by hatchup-dev [zer0hi]\n(https://github.com/hatchup-dev).");
}


void MainWindow::on_help_triggered()
{
    QMessageBox::information(this,"Помощь","Данная программа позволяет вести списки, а также выбирать случайный элемент из данных списков.\nДля создания списка нажмите кнопку (+).\nДля выбора списка используйте меню выбора списка.\nДанные сохраняются в sqlite базу в рабочей папке. Удалить пункт/список можно с помощью подменю \"удалить\", либо используя горячие клавиши Del и Shift+Del соответственно.\n По всем вопросам обращайтесь к разработчику программы, контакты можно найти в подменю \"О программе\".");
}

