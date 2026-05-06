#pragma once
#include <QObject>
#include <QProcess>
#include <QJsonObject>
#include <QMap>
#include <functional>

class Orchestrator : public QObject {
    Q_OBJECT

public:
    explicit Orchestrator(QObject *parent = nullptr);
    void start();

    void send(const QString &action,
              const QJsonObject &payload,
              std::function<void(bool ok, QJsonValue data, QString error)> callback);

private slots:
    void onReadyRead();
    void onProcessError(QProcess::ProcessError error);

private:
    QProcess *m_proc;
    QMap<QString, std::function<void(bool, QJsonValue, QString)>> m_pending;
    int m_idCounter = 0;
};
