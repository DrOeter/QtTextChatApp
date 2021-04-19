#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <chrono>
#include <ctime>
#include <iostream>
#include <QDebug>
#include "./socketprx.h"
#include <unistd.h>
#include <pthread.h>
#include <QThread>
#include <QMutex>
#include <QFileDialog>
#include <QScrollBar>
#include <fstream>
#include <string>
#include <cstddef>
#include <stdint.h>
#include <boost/algorithm/string.hpp>
#include <stdlib.h>
#include <time.h>
#include "base64.h"
#include "base64.cpp"
#include <QtWidgets>
#include "listviewdelegate.h"
#include <QtGui>


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);

    this->setFixedSize(600,800);

    Ui::main = this;

    //setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint );

    QWidget::setWindowTitle ("QtTextChat");

    ui->listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->listView->setSelectionRectVisible(1);
    ui->listView->setResizeMode(QListView::Adjust);
    ui->listView->setWordWrap(true);
    ui->listView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->listView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listView->setModel(&mymodel);
    //ui->listView->setMinimumSize(200,350);

    // NOW tell the view to rely on out custom delegate for drawing
    ui->listView->setItemDelegate(new ListViewDelegate());

    Ui::crypt.put_CryptAlgorithm("aes");
    Ui::crypt.put_CipherMode("cbc");
    Ui::crypt.put_KeyLength(256);
    Ui::crypt.put_PaddingScheme(0);
    //Ui::crypt.put_EncodingMode("hex");
    ui->textEdit->installEventFilter(this);

    QFont font = ui->lineEdit->font();
    font.setWeight(QFont::Bold);
    font.setPixelSize(13);
    ui->lineEdit->setFont(font);

    QFont font5 = ui->lineEdit_3->font();
    font5.setWeight(QFont::Bold);
    font5.setPixelSize(13);
    ui->lineEdit_3->setFont(font5);

    QFont font3 = ui->textEdit->font();
    font3.setWeight(QFont::Bold);
    font3.setPixelSize(13);
    ui->textEdit->setFont(font3);

    this->setStyleSheet("QMainWindow { background-color: #262626 }");
    ui->listView->setStyleSheet("QListView { background-color: #333333 }");
    ui->textEdit->setStyleSheet("QTextEdit { background-color: #333333; color : white; }");
    ui->lineEdit->setStyleSheet("QLineEdit { background-color: #333333; color : white;  }");
    ui->lineEdit_3->setStyleSheet("QLineEdit { background-color: #333333; color : white; }");
    ui->lineEdit_4->setStyleSheet("QLineEdit { background-color: #333333; color : white; }");
    ui->lineEdit_5->setStyleSheet("QLineEdit { background-color: #333333; color : white; }");
    ui->label->setStyleSheet("QLabel { color : white; font-size : 15px; }");
    ui->label_3->setStyleSheet("QLabel { color : white; font-size : 12px; }");
    ui->label_4->setStyleSheet("QLabel { color : white; font-size : 15px; }");
    ui->label_5->setStyleSheet("QLabel { color : white; font-size : 15px; }");
    ui->label_6->setStyleSheet("QLabel { color : white; font-size : 15px; }");
    ui->label_7->setStyleSheet("QLabel { color : white; font-size : 15px; }");
    ui->textBrowser->setStyleSheet("QTextBrowser { background-color : #262626; color : white; }");
    ui->textBrowser->setFrameStyle(QFrame::NoFrame);

    ui->lineEdit_4->setPlaceholderText("127.0.0.1");
    ui->lineEdit_5->setPlaceholderText("15000");
    ui->pushButton->hide();
    ui->textEdit->hide();

    ui->lineEdit->hide();
    ui->label->hide();

    ui->lineEdit_4->show();
    ui->lineEdit_5->show();
    ui->label_4->show();
    ui->label_5->show();

    ui->lineEdit_3->hide();
    ui->label_3->hide();


    ui->Send->setFocusPolicy(Qt::NoFocus);
    ui->Datei->setFocusPolicy(Qt::NoFocus);
    ui->textBrowser->setFocusPolicy(Qt::NoFocus);
    ui->checkBox->setFocusPolicy(Qt::NoFocus);
    ui->checkBox_2->setFocusPolicy(Qt::NoFocus);
    ui->pushButton->setFocusPolicy(Qt::NoFocus);
    this->setFocusPolicy(Qt::NoFocus);

    ui->lineEdit_4->setFocus();
}

MainWindow::~MainWindow(){
    delete ui;
    delete item1;
    delete ead;
    delete worker;
}

void MainWindow::onnprogress(QString info){
    ui->textBrowser->setText(info);
}

void MainWindow::onprogress(QString info){
    item1 = new QStandardItem(info);
    item1->setData("Incoming", Qt::UserRole + 1);
    mymodel.appendRow(item1);

    ui->listView->scrollToBottom();
}


void MainWindow::on_Send_clicked(){

    Ui::buffer.clear();

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(tbuffer,sizeof(tbuffer),"%H:%M:%S",timeinfo);

    if((!ui->textEdit->toPlainText().isEmpty())  ){//&& (!ui->textBrowser->toPlainText().isEmpty())){
        Ui::buffer+=tbuffer;
        Ui::buffer+="\n";
        Ui::buffer+=ui->textEdit->toPlainText();

        item1 = new QStandardItem(Ui::buffer);
        item1->setData("Outgoing", Qt::UserRole + 1);
        mymodel.appendRow(item1);
        ui->listView->scrollToBottom();
    }

    if(conf == 0){
        ip = ui->lineEdit_4->text().toStdString();
        port = ui->lineEdit_5->text().toStdString();

        if((ui->lineEdit_4->text().isEmpty()) && (ui->lineEdit_5->text().isEmpty())){
            Ui::sock = create_socket(AF_INET, SOCK_STREAM, 0);
            connect_socket(&Ui::sock, (char*)"127.0.0.1", 15000);
        }
        else {
            Ui::sock = create_socket(AF_INET, SOCK_STREAM, 0);
            connect_socket(&Ui::sock, (char*)ip.c_str(), atoi(port.c_str()));
        }

        ead= new QThread();
        worker= new Worker();
        worker->moveToThread(ead);
        connect(ead, SIGNAL (started()), worker, SLOT (process()));
        connect(worker, SIGNAL(progress(QString)), SLOT(onprogress(QString)));
        connect(worker, SIGNAL(nprogress(QString)), SLOT(onnprogress(QString)));
        connect(worker, SIGNAL (finished()), ead, SLOT (quit()));
        connect(worker, SIGNAL (finished()), worker, SLOT (deleteLater()));
        connect(ead, SIGNAL (finished()), ead, SLOT (deleteLater()));
        ead->start();

        conf = 1;
        goto C;
    }

    if( in == 0){
        start = ui->lineEdit->text().toStdString();

        QWidget::setWindowTitle (QString::fromLocal8Bit(start.c_str()) + " | QtTextChat");

        for(int i = 0;i<1000;i++)
            if(strcmp (start.c_str(), Ui::clients[i].c_str()) == 0) in = 1;

        if(in != 1)TCP_send (&Ui::sock, (char*)start.c_str(), strlen (start.c_str()));
        if(in == 1){
            in = 0;
            return;
        }

        else in=1;
    }

    if(line2 >= 3){
        sbuffer =ui->textEdit->toPlainText().toStdString();

        sbuffer = Ui::crypt.encryptStringENC( sbuffer.c_str() );

        TCP_send (&Ui::sock, (char*)sbuffer.c_str(), strlen(sbuffer.c_str()));
    }


    C:
    line2++;

    if(line2 == 1){
        ui->lineEdit_4->hide();
        ui->lineEdit_5->hide();

        ui->label_4->hide();
        ui->label_5->hide();

        ui->lineEdit->show();
        ui->label->show();
        ui->lineEdit->setFocus();
    }

    if(line2 == 2){
        ui->lineEdit->hide();
        ui->label->hide();

        ui->lineEdit_3->show();
        ui->label_3->show();
        ui->lineEdit_3->setFocus();
    }

    if(line2 == 3){
        group="$GroupHeader ";
        group += ui->lineEdit_3->text().toStdString();
        ogroup = ui->lineEdit_3->text().toStdString();

        if(!ui->lineEdit_3->text().isEmpty()){
            appon = 0;
            count = 0;
            end = 0;

            while ((starter = ogroup.find_first_not_of(delim, end)) != string::npos){
                end = ogroup.find(delim, starter);
                token = ogroup.substr(starter, end - starter);

                for(int i = 0;i<1000;i++)
                    if(strcmp (token.c_str(), Ui::clients[i].c_str()) == 0) appon++;

                count++;
            }

            if(appon == count){

                TCP_send (&Ui::sock, (char*)"$SuperxHeader", 13);
                //usleep(400000);
                TCP_send (&Ui::sock, (char*)group.c_str(), strlen (group.c_str()));
                release = 1;
            }
            else {
                line2 = 2;
                return;
            }

            ui->textBrowser->setText("Connected to: " + QString::fromLocal8Bit(ogroup.c_str()));
        }

        if(ui->lineEdit_3->text().isEmpty()){
            TCP_send (&Ui::sock, (char*)"$TargetHeader", 13);
            release = 1;
        }

        ui->lineEdit_3->hide();
        ui->label_3->hide();

        ui->textEdit->show();

        if(!ui->lineEdit_3->text().isEmpty()){
            srand(time(0));

            for(int i = 0; i < 64; i++) {
                sprintf(stri + i, "%x", rand() % 16);
            }

            b64 = boost::to_upper_copy<std::string>(stri);

            //qInfo()<<"b64i:" <<QString::fromLocal8Bit(b64.c_str())<<"\n";

            Ui::crypt.SetEncodedIV(b64.substr(0,32).c_str() ,"hex");

            Ui::crypt.SetEncodedKey(b64.c_str() ,"hex");

            b64i=base64_encode(b64);

            //TCP_send (&Ui::sock, (char*)b64i.c_str(), strlen(b64i.c_str()) );


            target = b64i + "$NameHeader " + ui->lineEdit_3->text().toStdString() + " " + ui->lineEdit->text().toStdString();
            //sleep(1);
            TCP_send (&Ui::sock, (char*)target.c_str(), strlen (target.c_str()));
            Ui::super=1;

            ui->pushButton->show();
        }
    }
}


void Worker::process(){
    while(1){
        B:
        memset(scbuffer, '\0', 1024);
        recv (Ui::sock, scbuffer, 1024, 0);

        qInfo()<<"b64:" <<QString::fromLocal8Bit(scbuffer)<<"\n";

        if(strncmp (scbuffer, "$NewGroupHeader", 15) == 0 && Ui::super == 1){
            delstr.clear();
            end = 0;
            delstr=scbuffer;

            while ((start = delstr.find_first_not_of(' ', end)) != string::npos){
                end = delstr.find(' ', start);
                token = delstr.substr(start, end - start);

                if(at == 1)newclients[nclit] = token;

                if(at == 2){
                    sdarr[nclit]= atoi(token.c_str());
                    nclit++;
                }
                at++;
            }
            at = 0;

            emit progress("New Client: " + QString::fromLocal8Bit(newclients[nclit-1].c_str()));

            qInfo()<<QString::fromLocal8Bit(newclients[0].c_str())<<"  "<<sdarr[0];
            goto B;
        }

        if(strncmp (scbuffer, "$KeepHeader", 11) == 0 && Ui::super == 0){
            TCP_send (&Ui::sock, (char*)scbuffer, strlen(scbuffer));
            goto B;
        }

        if(strncmp (scbuffer, "$NewTargetHeader", 16) == 0 && Ui::super == 0){
            TCP_send (&Ui::sock, (char*)scbuffer, strlen(scbuffer));
            goto B;
        }

       /* if(strncmp (scbuffer, "$QuitHeader", 11) == 0){
            Ui::quit = 1;
            emit nprogress("");
            goto B;
        }*/

        if(strncmp (scbuffer, "$ExitHeader", 11) == 0){
            TCP_send (&Ui::sock, scbuffer, strlen(scbuffer));
            goto B;
        }

        if((strcmp (scbuffer, "$StandbyHeader") == 0) && (stby == 0) && (Ui::super == 0)){
            TCP_send (&Ui::sock, (char*)"$StandbyHeader", 14);
            stby = 1;
            goto B;
        }

        if(strncmp (scbuffer, "Client: ",8) == 0){
            delstr.clear();
            end = 0;
            delstr=scbuffer;

            while ((start = delstr.find_first_not_of(delim, end)) != string::npos){
                end = delstr.find(delim, start);
                token = delstr.substr(start, end - start);

                Ui::clients[clit] = token.substr(8, (strlen(token.c_str())-8) );

                //qInfo()<<"cli:" <<QString::fromLocal8Bit(Ui::clients[clit].c_str())<<"\n";
                clit++;
            }

            goto A;        }

        if((trig == 0) && (Ui::super == 0)){
            namstr = scbuffer;

            namstr = namstr.substr(100, strlen(namstr.c_str())-100);

            emit nprogress("Connected to: " + QString::fromLocal8Bit(namstr.c_str()));

            //scbuffer = namstr;

            for(unsigned int i=88;i < strlen(scbuffer);i++)
                scbuffer[i] = '\0';

            //qInfo()<<"nam:" <<QString::fromLocal8Bit(namstr.c_str())<<"\n";
            //qInfo()<<"scb:" <<QString::fromLocal8Bit(scbuffer)<<"\n";

            Ui::keystr = scbuffer;
            Ui::keystr = base64_decode(Ui::keystr);
            Ui::crypt.SetEncodedIV(Ui::keystr.substr(0,32).c_str() ,"hex");

            Ui::crypt.SetEncodedKey(Ui::keystr.c_str() ,"hex");
            qInfo()<<"ssee" <<QString::fromLocal8Bit(Ui::keystr.c_str())<<"\n";
            trig = 1;
        }

        if(strcmp (scbuffer, "$FileHeader") == 0){

            memset(scbuffer, '\0', 1024);
            recv (Ui::sock, scbuffer, 1024, 0);

            system("mkdir output");

            locate="./output/";
            locate +=scbuffer;

            FILE *file = fopen(locate.c_str(), "wb");

            memset(scbuffer, '\0', 1024);
            recv (Ui::sock, scbuffer, 1024, 0);

            //qInfo()<<"size: " <<QString::fromLocal8Bit(scbuffer)<<"\n";

            if (file != NULL){
                if(!readfile(Ui::sock, file, atol(scbuffer) )) qInfo()<<"error1\n";

                fclose(file);
            }

            Ui::crypt.CkDecryptFile(locate.c_str(), locate.substr(0, strlen(locate.c_str())-4).c_str() );

            remove(locate.c_str());
            goto B;
        }

        else if(((trig == 2) || (Ui::super == 1)) && std::string(scbuffer).find("Header") == std::string::npos && std::string(scbuffer).find("$") != 0){
            A:

            Ui::buffer.clear();
            time (&rawtime);
            timeinfo = localtime(&rawtime);

            strftime(tbuffer,sizeof(tbuffer),"%H:%M:%S",timeinfo);

            if(strncmp (scbuffer, "Client: ",8) != 0){
                Ui::buffer+=tbuffer;
                Ui::buffer+="\n";
            }

            dec = Ui::crypt.decryptStringENC(scbuffer);

            if(strncmp (scbuffer, "Client: ",8) == 0) Ui::buffer+=QString::fromLocal8Bit(scbuffer);
            else Ui::buffer+=QString::fromLocal8Bit(dec);

            emit progress(Ui::buffer);
        }
        if(trig == 1) trig = 2;
    }
    emit finished();
}



void MainWindow::on_Datei_clicked(){

    ui->Send->hide();

    QFileDialog* f_dlg = new QFileDialog(this);

    if(ui->checkBox_2->isChecked()) f_dlg->setFileMode(QFileDialog::Directory);

    f_dlg->setOption(QFileDialog::DontUseNativeDialog, true);

    QListView *l = f_dlg->findChild<QListView*>("listView");

    if(l) l->setSelectionMode(QAbstractItemView::ExtendedSelection);

    QTreeView *t = f_dlg->findChild<QTreeView*>();

    if(t) t->setSelectionMode(QAbstractItemView::ExtendedSelection);


    f_dlg->exec();
    QStringList file1 = f_dlg->selectedFiles();

    fit=0;

    for ( auto it = file1.constBegin(); it != file1.constEnd(); ++it,++fit ){
        yee.clear();
        D:
        if(QFileInfo(file1[fit]).isDir() && folder == 0){
            QDirIterator it(file1[fit], QStringList() << "*", QDir::Files, QDirIterator::Subdirectories);

            while (it.hasNext())
                yee.append(it.next());

            folder = 1;
        }

        else sfile = file1[fit];

        if(folder == 1){
            aes = yee[foldit] + ".aes";
            Ui::crypt.CkEncryptFile(yee[foldit].toStdString().c_str() ,aes.toStdString().c_str());
        }
        else {
            aes = sfile + ".aes";
            Ui::crypt.CkEncryptFile(sfile.toStdString().c_str() ,aes.toStdString().c_str());
        }

        qInfo()<<yee<<aes<<foldit;

        if((folder == 1) && ((foldit + 1) < yee.size())) foldit++;

        else folder = 0 ,foldit = 0;

        FILE *file = fopen(aes.toStdString().c_str(), "rb");

        fseek(file, 0, SEEK_END);
        long filesize = ftell(file);
        rewind(file);

        fsize = Num(filesize);

        TCP_send (&Ui::sock, (char*)"$FileHeader", 11);
        sleep(1);

        TCP_send (&Ui::sock, (char*)split(aes.toStdString()).c_str(), strlen (split(aes.toStdString()).c_str()));
        sleep(1);

        TCP_send (&Ui::sock,(char*)fsize.c_str(), strlen(fsize.c_str()) );
        sleep(1);

        if (file != NULL){
            if(!sendfile(Ui::sock, file)) qInfo()<<"error2\n";
            fclose(file);
        }

        remove(aes.toStdString().c_str());
        sleep(1);

        if(folder == 1)goto D;
    }
    ui->Send->show();
}

void MainWindow::on_pushButton_clicked(){
    if(Ui::super == 1){

        if(splits == 0){
            ui->textEdit->hide();
            ui->lineEdit_3->clear();
            ui->lineEdit_3->show();
            ui->lineEdit_3->setFocus();
            ui->label_3->show();
        }

        if(splits == 1){
            addit = ui->lineEdit_3->text().toStdString();

            if(!ui->lineEdit_3->text().isEmpty()){
                for(int i=0;i<1000;i++){
                    if(strcmp (addit.c_str(), Ui::work->newclients[i].c_str()) == 0){
                        addit = "$NewTargetHeader " + Num(Ui::work->sdarr[i]);

                        target += " " + ui->lineEdit_3->text().toStdString();

                        TCP_send (&Ui::sock, (char*)addit.c_str(), strlen(addit.c_str()));
                        sleep(1);
                        TCP_send (&Ui::sock, (char*)target.c_str(), strlen(target.c_str()));
                    }
                }
            }
            ui->lineEdit_3->hide();
            ui->label_3->hide();
            ui->textEdit->show();
        }

        if(splits == 1) splits = 0;
        else if(splits == 0) splits = 1;
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event){
    if(!ui->checkBox->isChecked()){
        QKeyEvent *e = static_cast<QKeyEvent*>(event);

        if (e->key() == 16777220){
            event->ignore();
            return true;
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::closeEvent (QCloseEvent *event){
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Beenden",
                                                                    tr("Are you sure?\n"),
                                                                    QMessageBox::No | QMessageBox::Yes,
                                                                    QMessageBox::Yes);
        if (resBtn != QMessageBox::Yes) {
            event->ignore();
        } else {
            if(in == 0){
                TCP_send (&Ui::sock, (char*)"!:!", 3);
                usleep(400000);
                TCP_send (&Ui::sock, (char*)"$TargetHeader", 13);
                usleep(400000);
            }

            else if((in == 1) && (release == 0)){
                TCP_send (&Ui::sock, (char*)"$TargetHeader", 13);
                usleep(400000);
            }

            TCP_send (&Ui::sock, (char*)"$QuitHeader", 11);
            event->accept();
        }
}


