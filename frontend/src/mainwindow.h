#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include "orchestrator.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QStackedWidget *m_stack;
    Orchestrator *m_orc;

    QWidget *buildWelcomePage();
    QWidget *buildWifiPage();
    QWidget *buildDiskPage();
    QWidget *buildUserPage();
    QWidget *buildInstallPage();

    void nextPage();
};
