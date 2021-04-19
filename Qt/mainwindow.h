#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string>
#include <QThread>
#include <QDebug>
#include <QObject>
#include <QWidget>
#include <QTextBrowser>
#include <QCloseEvent>
#include <QMessageBox>
#include <CkCrypt2.h>
#include <QtWidgets>
#include <stdlib.h>
#include <stddef.h>

class MainWindow;
class Worker;

QT_BEGIN_NAMESPACE
namespace Ui {
    static int sock,super=0,quit=0;
    static QString buffer;
    static CkCrypt2 crypt;
    static std::string keystr, clients[1000];
    static Worker *work;
    static MainWindow *main;
    class MainWindow;
    class Worker;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    friend class Worker;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void on_textEdit_textChanged(){}

    void on_Send_clicked();

    void onprogress(QString info);

    void onnprogress(QString info);

    void closeEvent(QCloseEvent *event);

    bool eventFilter(QObject *watched, QEvent *event);

private slots:
    void on_Datei_clicked();

    void on_pushButton_clicked();

private:
    QThread *ead;
    Worker *worker;
    QStandardItem *item1;
    QStandardItemModel mymodel;
    QStringList yee;
    Ui::MainWindow *ui;
    time_t rawtime;
    struct tm * timeinfo;
    char tbuffer[80];
    std::string ip, port, start,target, sbuffer,fsize , rfsize,b64i,b64,group,token,ogroup,addit;
    int in=0, fit=0, appon = 0,count = 0,conf = 0;
    int line2 = 0, release = 0, folder = 0,foldit = 0,splits=0;
    QString sfile,aes;
    char stri[64 + 1],delim = ' ';
    size_t starter;
    size_t end = 0;
};

class Worker : public QObject {
    Q_OBJECT
public:
    std::string newclients[1000];
    int sdarr[1000];

    Worker(){
        Ui::work = this;
    }

    ~Worker(){
        delete[] scbuffer;
    }

public slots:
    void process();


signals:
    void progress(QString info);
    void nprogress(QString info);
    void finished();
    void started();
    void error(QString err);
private:
    QTextBrowser *rbrow;
    time_t rawtime;
    struct tm * timeinfo;
    char tbuffer[80], delim = '\n';
    char *scbuffer= new char[1024];
    std::string locate,namstr,token,delstr;
    int on=0, trig=0, clit=0, stby = 0,nclit = 0,at=0,newt = 0;
    const char *dec;
    size_t start;
    size_t end = 0;
};



#endif // MAINWINDOW_H
