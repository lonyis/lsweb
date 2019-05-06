#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QMenu>
#include <QDateTime>
#include <QDate>
#include <QMessageBox>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QSettings>

#define REG_RUN "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QProcess *p_cmd;
    QProcess *p_nginx;
    QProcess *p_php;
    QProcess *p_php1;
    QProcess *p_php2;
    QProcess *p_php3;
    QProcess *p_php4;

    QMenu *lmenu;
    QAction *r_nginx;
    QAction *r_phpc;
    QAction *r_phpm;
    QAction *r_nginxini;
    QAction *r_phpini;
    QAction *r_phpinfo;

    QString runPath;
    QString dir_phpcgi;
    QString dir_nginx;

    void CreatTrayMenu();
    void CreatTrayIcon();
    QSystemTrayIcon *myTrayIcon;

    QMenu *myMenu;

    QAction *miniSizeAction;
    QAction *maxSizeAction;
    QAction *restoreWinAction;
    QAction *quitAction;

    explicit MainWindow(QWidget *parent = 0);
    void closeEvent(QCloseEvent *event);
    void changeEvent(QEvent *event);
    int is_run_server;

    ~MainWindow();

private slots:
    void readOutput();
    void nginxReadOutput();
    void phpReadOutput();
    void finish_process(int exitCode, QProcess::ExitStatus exitStatus);
    void finish_process_p1(int exitCode, QProcess::ExitStatus exitStatus);
    void finish_process_p2(int exitCode, QProcess::ExitStatus exitStatus);
    void finish_process_p3(int exitCode, QProcess::ExitStatus exitStatus);
    void finish_process_p4(int exitCode, QProcess::ExitStatus exitStatus);
    void lmenudo(QAction *action);
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void closeTrayIcons();
    void on_pushButton_3_clicked();
    void on_pushButton_start_clicked();
    void on_pushButton_stop_clicked();

    void on_checkBox_auto_clicked();

private:
    Ui::MainWindow *ui;
    QString application_name;
    QSettings *configIni;
    QSettings *auto_settings;
    int is_auto_config;
};

#endif // MAINWINDOW_H
