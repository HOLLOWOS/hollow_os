#pragma once
#include <QWidget>
#include <QString>

class DesktopPage : public QWidget {
    Q_OBJECT

public:
    explicit DesktopPage(QWidget *parent = nullptr);
    QString selectedDesktop() const;

signals:
    void nextPage();

private:
    QString m_selected = "kde";
};
