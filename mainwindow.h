#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pb_addDB_clicked();

    void on_cb_chooseDB_activated(int index);

    void on_pb_addtoDB_clicked();

    void on_del_list_triggered();

    void on_del_point_triggered();

    void on_list_Main_itemSelectionChanged();

    void on_pb_random_clicked();

    void on_list_Main_itemDoubleClicked(QListWidgetItem *item);

    void on_about_triggered();

    void on_help_triggered();

    void on_search_triggered();

private:
    Ui::MainWindow *ui;
    void initDB(QSqlDatabase db);
    void updateDB(QSqlDatabase db);
    void createTable(QSqlDatabase db, QString name);
    void changeState(bool state);
};
#endif // MAINWINDOW_H
