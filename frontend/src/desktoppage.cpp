#include "desktoppage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>
#include <QRadioButton>

DesktopPage::DesktopPage(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *title = new QLabel("<h2>Choose Your Desktop</h2>");
    title->setAlignment(Qt::AlignCenter);
    QLabel *sub = new QLabel("Don't worry, you can change this later.");
    sub->setAlignment(Qt::AlignCenter);

    QButtonGroup *group = new QButtonGroup(this);

    QRadioButton *kdeBtn = new QRadioButton("KDE Plasma  —  Modern, feature-rich, Qt-based");
    QRadioButton *xfceBtn = new QRadioButton("XFCE  —  Lightweight, fast, classic feel");
    QRadioButton *gnomeBtn = new QRadioButton("GNOME  —  Clean, minimal, touch-friendly");

    kdeBtn->setChecked(true);
    group->addButton(kdeBtn);
    group->addButton(xfceBtn);
    group->addButton(gnomeBtn);

    QPushButton *nextBtn = new QPushButton("Next");
    nextBtn->setFixedWidth(200);

    layout->addWidget(title);
    layout->addWidget(sub);
    layout->addSpacing(20);
    layout->addWidget(kdeBtn);
    layout->addWidget(xfceBtn);
    layout->addWidget(gnomeBtn);
    layout->addSpacing(20);
    layout->addWidget(nextBtn, 0, Qt::AlignCenter);

    connect(kdeBtn, &QRadioButton::clicked, [=]() { m_selected = "kde"; });
    connect(xfceBtn, &QRadioButton::clicked, [=]() { m_selected = "xfce"; });
    connect(gnomeBtn, &QRadioButton::clicked, [=]() { m_selected = "gnome"; });
    connect(nextBtn, &QPushButton::clicked, this, &DesktopPage::nextPage);
}

QString DesktopPage::selectedDesktop() const {
    return m_selected;
}
