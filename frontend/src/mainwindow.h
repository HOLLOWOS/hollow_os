#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include "orchestrator.h"
#include "desktoppage.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QStackedWidget *m_stack;
    Orchestrator *m_orc;
    DesktopPage *m_desktopPage;
    QString m_selectedDisk;
    QString m_username;
    QString m_password;

    QWidget *buildWelcomePage();
    QWidget *buildWifiPage();
    QWidget *buildDiskPage();
    QWidget *buildUserPage();
    QWidget *buildInstallPage();

    void nextPage();
    void startInstall();
};
