#include "mainwindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QProgressBar>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Hollow OS Installer");
    setMinimumSize(800, 600);

    m_orc = new Orchestrator(this);
    m_orc->start();

    m_stack = new QStackedWidget(this);
    m_stack->addWidget(buildWelcomePage());
    m_stack->addWidget(buildWifiPage());
    m_stack->addWidget(buildDiskPage());
    m_stack->addWidget(buildUserPage());
    m_stack->addWidget(buildInstallPage());

    setCentralWidget(m_stack);
}

void MainWindow::nextPage() {
    m_stack->setCurrentIndex(m_stack->currentIndex() + 1);
}

QWidget *MainWindow::buildWelcomePage() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *title = new QLabel("<h1>Welcome to Hollow OS</h1>");
    title->setAlignment(Qt::AlignCenter);
    QLabel *sub = new QLabel("Let's get your system set up.");
    sub->setAlignment(Qt::AlignCenter);
    QPushButton *btn = new QPushButton("Get Started");
    btn->setFixedWidth(200);
    connect(btn, &QPushButton::clicked, this, &MainWindow::nextPage);

    layout->addWidget(title);
    layout->addWidget(sub);
    layout->addWidget(btn, 0, Qt::AlignCenter);
    return page;
}

QWidget *MainWindow::buildWifiPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);

    QLabel *title = new QLabel("<h2>Connect to Wi-Fi</h2>");
    QListWidget *netList = new QListWidget();
    QLabel *status = new QLabel("Press Scan to find networks.");
    QLineEdit *passInput = new QLineEdit();
    passInput->setPlaceholderText("Password (blank if open)");
    passInput->setEchoMode(QLineEdit::Password);
    passInput->setEnabled(false);

    QPushButton *scanBtn = new QPushButton("Scan");
    QPushButton *connectBtn = new QPushButton("Connect");
    QPushButton *skipBtn = new QPushButton("Skip (already connected)");
    connectBtn->setEnabled(false);

    layout->addWidget(title);
    layout->addWidget(scanBtn);
    layout->addWidget(status);
    layout->addWidget(netList);
    layout->addWidget(passInput);
    layout->addWidget(connectBtn);
    layout->addWidget(skipBtn);

    connect(skipBtn, &QPushButton::clicked, this, &MainWindow::nextPage);

    connect(netList, &QListWidget::itemClicked, [=](QListWidgetItem *) {
        passInput->setEnabled(true);
        connectBtn->setEnabled(true);
    });

    connect(scanBtn, &QPushButton::clicked, [=]() {
        status->setText("Scanning...");
        scanBtn->setEnabled(false);
        m_orc->send("wifi.scan", {}, [=](bool ok, QJsonValue data, QString error) {
            scanBtn->setEnabled(true);
            if (!ok) { status->setText("Scan failed: " + error); return; }
            netList->clear();
            QJsonArray networks = data.toArray();
            status->setText(QString("Found %1 network(s).").arg(networks.size()));
            for (const auto &n : networks) {
                QJsonObject net = n.toObject();
                netList->addItem(net["ssid"].toString() + "  (" + net["signal"].toString() + ")");
            }
        });
    });

    connect(connectBtn, &QPushButton::clicked, [=]() {
        auto *item = netList->currentItem();
        if (!item) return;
        QString ssid = item->text().split("  ").first().trimmed();
        QJsonObject payload;
        payload["ssid"] = ssid;
        if (!passInput->text().isEmpty()) payload["password"] = passInput->text();
        m_orc->send("wifi.connect", payload, [=](bool ok, QJsonValue, QString error) {
            if (ok) nextPage();
            else QMessageBox::critical(this, "Failed", "Connection failed: " + error);
        });
    });

    return page;
}

QWidget *MainWindow::buildDiskPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);

    QLabel *title = new QLabel("<h2>Select Installation Disk</h2>");
    QListWidget *diskList = new QListWidget();
    QLabel *status = new QLabel("Loading disks...");
    QPushButton *installBtn = new QPushButton("Install to selected disk");
    installBtn->setEnabled(false);

    layout->addWidget(title);
    layout->addWidget(status);
    layout->addWidget(diskList);
    layout->addWidget(installBtn);

    m_orc->send("disk.list", {}, [=](bool ok, QJsonValue data, QString error) {
        if (!ok) { status->setText("Failed to load disks: " + error); return; }
        QJsonArray disks = data.toArray();
        status->setText(QString("Found %1 disk(s). Select one.").arg(disks.size()));
        for (const auto &d : disks) {
            QJsonObject disk = d.toObject();
            diskList->addItem(disk["device"].toString() + "  " + disk["size"].toString());
        }
        installBtn->setEnabled(true);
    });

    connect(installBtn, &QPushButton::clicked, [=]() {
        auto *item = diskList->currentItem();
        if (!item) return;
        QString device = item->text().split("  ").first().trimmed();
        QJsonObject payload;
        payload["device"] = device;
        m_orc->send("disk.partition", payload, [=](bool ok, QJsonValue, QString error) {
            if (ok) nextPage();
            else QMessageBox::critical(this, "Failed", "Partitioning failed: " + error);
        });
    });

    return page;
}

QWidget *MainWindow::buildUserPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);

    QLabel *title = new QLabel("<h2>Create Your Account</h2>");
    QLineEdit *userInput = new QLineEdit();
    userInput->setPlaceholderText("Username");
    QLineEdit *passInput = new QLineEdit();
