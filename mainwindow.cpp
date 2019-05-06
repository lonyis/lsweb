#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include <stdlib.h>
#include "kill.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //
    p_cmd = new QProcess;
    p_php = new QProcess;
    p_php1 = new QProcess;
    p_php2 = new QProcess;
    p_php3 = new QProcess;
    p_php4 = new QProcess;
    p_nginx = new QProcess;

    connect(p_cmd, SIGNAL(readyRead()), this, SLOT(readOutput()));
    connect(p_php, SIGNAL(readyRead()), this, SLOT(phpReadOutput()));
    connect(p_php1, SIGNAL(readyRead()), this, SLOT(phpReadOutput()));
    connect(p_php2, SIGNAL(readyRead()), this, SLOT(phpReadOutput()));
    connect(p_php3, SIGNAL(readyRead()), this, SLOT(phpReadOutput()));
    connect(p_php4, SIGNAL(readyRead()), this, SLOT(phpReadOutput()));
    connect(p_nginx, SIGNAL(readyRead()), this, SLOT(nginxReadOutput()));

    connect(p_php, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(phpAbnormal(p_php, QProcess::ExitStatus)));
    connect(p_php1, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(phpAbnormal(p_php1, QProcess::ExitStatus)));
    connect(p_php2, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(phpAbnormal(p_php2, QProcess::ExitStatus)));
    connect(p_php3, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(phpAbnormal(p_php3, QProcess::ExitStatus)));
    connect(p_php4, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(phpAbnormal(p_php4, QProcess::ExitStatus)));

    runPath = QCoreApplication::applicationDirPath();

    ui->setupUi(this);
    //去掉最大化按钮
    this->setWindowFlags(this->windowFlags()&~Qt::WindowMaximizeButtonHint);
    setWindowIcon(QIcon(QPixmap(":/img/zddic.ico")));
    myTrayIcon = new QSystemTrayIcon(this);
    lmenu = new QMenu();
    connect(lmenu,SIGNAL(triggered(QAction *)),this,SLOT(lmenudo(QAction *)));
    setFixedSize(640, 426);

    r_nginx = new QAction(lmenu);
    r_phpc = new QAction(lmenu);
    r_phpm = new QAction(lmenu);
    r_phpini = new QAction(lmenu);
    r_nginxini = new QAction(lmenu);
    r_phpinfo = new QAction(lmenu);

    lmenu->addAction(r_nginx);
    lmenu->addAction(r_phpc);
    lmenu->addAction(r_phpm);
    lmenu->addAction(r_phpini);
    lmenu->addAction(r_nginxini);
    lmenu->addAction(r_phpinfo);

    r_nginx->setText("重启nginx");
    r_phpc->setText("重启php-cgi");
    r_phpm->setText("查看php加载扩展");
    r_nginxini->setText("nginx.conf");
    r_phpini->setText("php.ini");
    r_phpinfo->setText("php info");

    ui->pushButton_3->setMenu(lmenu);
    this->ui->pushButton_stop->setDisabled(true);
    this->ui->textEdit->setReadOnly(true);

    //fpm
    configIni = new QSettings("server_conf.ini", QSettings::IniFormat);
    QString fpm_addr = configIni->value("/server/fpm").toString();
    if(fpm_addr.isEmpty()) {
        configIni->setValue("/server/fpm", "127.0.0.1:9000");
        this->ui->php_cgi->setText("127.0.0.1:9000");
    } else {
        this->ui->php_cgi->setText(fpm_addr);
    }
    dir_nginx = runPath + "/nginx/nginx.exe";
    CreatTrayIcon();

    //auto
    application_name = QApplication::applicationName();
    QString is_auto = configIni->value("/run/is_auto").toString();
    if(is_auto.isEmpty()) {
        //
        auto_settings = new QSettings(REG_RUN, QSettings::NativeFormat);
        QString application_path = QApplication::applicationFilePath();
        auto_settings->setValue(application_name, application_path.replace("/", "\\"));
        configIni->setValue("/run/is_auto", "1");
        this->is_auto_config = 1;
        this->ui->checkBox_auto->setCheckState(Qt::CheckState::Checked);
        this->on_pushButton_start_clicked();
    } else if(is_auto == "1") {
        this->ui->checkBox_auto->setCheckState(Qt::CheckState::Checked);
        this->is_auto_config = 1;
        this->on_pushButton_start_clicked();
    } else {
        this->is_auto_config = 0;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::CreatTrayMenu()
{
   miniSizeAction = new QAction(tr("最小化(&N)"), this);
   restoreWinAction = new QAction(tr("还 原(&R)"), this);
   quitAction = new QAction(tr("退出(&Q)"), this);

   this->connect(miniSizeAction, SIGNAL(triggered()),this, SLOT(hide()));
   this->connect(restoreWinAction, SIGNAL(triggered()),this, SLOT(showNormal()));
   this->connect(quitAction, SIGNAL(triggered()), this, SLOT(closeTrayIcons()));

   myMenu = new QMenu((QWidget*)QApplication::desktop());

   myMenu->addAction(miniSizeAction);
   myMenu->addAction(restoreWinAction);
   myMenu->addSeparator();
   myMenu->addAction(quitAction);
}

void MainWindow::CreatTrayIcon()
{
    CreatTrayMenu();

    if (!QSystemTrayIcon::isSystemTrayAvailable()) //isSystemTrayAvailable
    {
       return;
    }

    myTrayIcon->setIcon(QIcon((":/img/zhidd.png")));
    myTrayIcon->setToolTip("lonyis server tools V0.94");
//    myTrayIcon->showMessage("server tools","最小化到托盘，从这里恢复窗口",QSystemTrayIcon::Information,10000);

    myTrayIcon->setContextMenu(myMenu);

    myTrayIcon->show();
    this->connect(myTrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
    case QSystemTrayIcon::Trigger:
        break;
    case QSystemTrayIcon::DoubleClick:
        this->show();
        this->showNormal();
        this->activateWindow();
        break;
    case QSystemTrayIcon::MiddleClick:
        break;

    default:
        break;
    }
}

void MainWindow::changeEvent(QEvent *event)
{
    if(this->is_auto_config == 1) {
        hide();
        event->ignore();
        this->is_auto_config = 0;
    }
    if(event->type()==QEvent::WindowStateChange){
        if(windowState() & Qt::WindowMinimized){
           hide();
//           myTrayIcon->showMessage("server tools","最小化到托盘，从这里恢复窗口",QSystemTrayIcon::Information,10000);
           myTrayIcon->show();

        }
        QMainWindow::changeEvent(event);
    }

}
void MainWindow::closeEvent(QCloseEvent *event)
{

    if (myTrayIcon->isVisible()) {
//       myTrayIcon->showMessage("server tools","最小化到托盘，从这里恢复窗口",QSystemTrayIcon::Information,5000);
       hide();
       event->ignore();
    }
    else {
        //event->accept();
        this->show();
    }
}

void MainWindow::closeTrayIcons()
{
    QMessageBox::StandardButton button;
    button = QMessageBox::question(this, tr("退出程序!"),
        QString(tr("警告：关闭程序会关闭nginx服务器，是否结束操作退出？")),
        QMessageBox::Yes | QMessageBox::No);

    if (button == QMessageBox::No) {
        this->show();

    } else if (button == QMessageBox::Yes) {
        QString str = dir_nginx + " -s stop";

        char*  ch;
        QByteArray ba = str.toLatin1();
        ch=ba.data();
        //system(ch);
        //close nginx php
        p_nginx->close();
        p_php->close();
        p_php1->close();
        p_php2->close();
        p_php3->close();
        p_php4->close();

        qApp->quit(); //quit
    }
}

void MainWindow::on_pushButton_start_clicked()
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss");
    this->ui->textEdit->append(current_date + " start nginx..." + dir_nginx + " -c " + runPath + "/conf/nginx.conf");
    p_nginx->start(dir_nginx + " -c " + runPath + "/conf/nginx.conf");

    QString phpcgipost = this->ui->php_cgi->text();
    configIni->setValue("/server/fpm", phpcgipost);

    current_date_time = QDateTime::currentDateTime();
    current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss");

    dir_phpcgi = runPath + "/php/php-cgi.exe -b "+ phpcgipost +" -c " + runPath + "/conf/php.ini";

    this->ui->textEdit->append(current_date + " start php-cgi... " + dir_phpcgi);
    p_php->start(dir_phpcgi);
    p_php1->start(dir_phpcgi);
    p_php2->start(dir_phpcgi);
    p_php3->start(dir_phpcgi);
    p_php4->start(dir_phpcgi);
    this->ui->pushButton_stop->setDisabled(false);
    this->ui->pushButton_start->setDisabled(true);
}

void MainWindow::readOutput()
{
    QString cc = p_cmd->readAll();
    this->ui->textEdit->append(cc);
}



void MainWindow::nginxReadOutput()
{
    QString cc = p_nginx->readAll();
    this->ui->textEdit->append(cc);
    qDebug() << cc;
    this->ui->textEdit->append("ok");
}

void MainWindow::phpReadOutput()
{
    QString cc = p_php->readAll();
    this->ui->textEdit->append(cc);
    this->ui->textEdit->append("ok");
}

void MainWindow::phpAbnormal(QProcess php_process, QProcess::ExitStatus exitStatus)
{
//    if(status < 0) {
//        php_process->start();
//    } else {
//        this->ui->textEdit->append("php finished");
//    }
    QString str = QString::number(exitStatus);
    this->ui->textEdit->append(str);
}

void MainWindow::on_pushButton_stop_clicked()
{
    p_nginx->close();

    p_php->close();
    p_php1->close();
    p_php2->close();
    p_php3->close();
    p_php4->close();

    std::string name = "nginx.exe";
    kill *nginx_kill = new kill();
    nginx_kill->kills(name);

    this->ui->pushButton_stop->setDisabled(true);
    this->ui->pushButton_start->setDisabled(false);
}

void MainWindow::on_pushButton_3_clicked()
{

}

void MainWindow::lmenudo(QAction *action)
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss");

    QString runPath = QCoreApplication::applicationDirPath();
    if (action == r_nginx)
    {
        this->ui->textEdit->append(current_date + " restart nginx... " + dir_nginx + " -s stop" );
        QString str = dir_nginx + " -s stop";

        char*  ch;
        QByteArray ba = str.toLatin1();
        ch=ba.data();
        //system(ch);

        p_nginx->close();
        p_nginx->start(dir_nginx);
    } else if (action == r_phpc)
    {
        this->ui->textEdit->append(current_date + " restart php-cgi... " + dir_phpcgi);
        QString phpcgipost = this->ui->php_cgi->text();
        dir_phpcgi = runPath + "/php/php-cgi.exe -b "+ phpcgipost +" -c " + runPath + "/php.ini";
        p_php->close();
        p_php1->close();
        p_php2->close();
        p_php3->close();
        p_php4->close();

        p_php->start(dir_phpcgi);
        p_php1->start(dir_phpcgi);
        p_php2->start(dir_phpcgi);
        p_php3->start(dir_phpcgi);
        p_php4->start(dir_phpcgi);

    } else if(action == r_phpm) {
        this->ui->textEdit->append(current_date + " php -m");
        p_cmd->start(runPath + "/php/php.exe -m");
    } else if(action == r_phpinfo) {
        QDesktopServices::openUrl(QUrl("http://localhost/info.php"));
    } else if(action == r_nginxini) {
        this->ui->textEdit->append(current_date + " open " + runPath + "/conf/nginx.conf");
        QDesktopServices::openUrl(QUrl("file:///" + runPath + "/conf/nginx.conf", QUrl::TolerantMode));
    } else if(action == r_phpini) {
        this->ui->textEdit->append(current_date + " open " + runPath + "/php.ini");
        QDesktopServices::openUrl(QUrl("file:///" + runPath + "/php.ini", QUrl::TolerantMode));
    }
}

void MainWindow::on_checkBox_auto_clicked()
{
    if(ui->checkBox_auto->checkState() == Qt::Checked) {
        auto_settings = new QSettings(REG_RUN, QSettings::NativeFormat);
        QString application_path = QApplication::applicationFilePath();
        auto_settings->setValue(application_name, application_path.replace("/", "\\"));
        configIni->setValue("/run/is_auto", "1");
    } else {
        auto_settings = new QSettings(REG_RUN, QSettings::NativeFormat);
        auto_settings->remove(application_name);
        configIni->setValue("/run/is_auto", "0");
    }
}
