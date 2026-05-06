#include "orchestrator.h"
#include <QJsonDocument>
#include <QJsonValue>
#include <QDebug>

Orchestrator::Orchestrator(QObject *parent) : QObject(parent) {
    m_proc = new QProcess(this);
    connect(m_proc, &QProcess::readyReadStandardOutput, this, &Orchestrator::onReadyRead);
    connect(m_proc, &QProcess::errorOccurred, this, &Orchestrator::onProcessError);
}

void Orchestrator::start() {
    m_proc->start("bun", { "run", "/usr/share/hollow-install/orchestrator/index.ts" });
    if (!m_proc->waitForStarted(3000)) {
        qFatal("hollow-install: failed to start hollow-orc");
    }
}

void Orchestrator::send(const QString &action,
                        const QJsonObject &payload,
                        std::function<void(bool, QJsonValue, QString)> callback)
{
    QString id = QString::number(++m_idCounter);
    QJsonObject msg;
    msg["id"] = id;
    msg["action"] = action;
    if (!payload.isEmpty()) msg["payload"] = payload;
    m_pending[id] = callback;
    QByteArray line = QJsonDocument(msg).toJson(QJsonDocument::Compact) + "\n";
    m_proc->write(line);
}

void Orchestrator::onReadyRead() {
    while (m_proc->canReadLine()) {
        QByteArray line = m_proc->readLine().trimmed();
        if (line.isEmpty()) continue;
        QJsonDocument doc = QJsonDocument::fromJson(line);
        if (doc.isNull()) continue;
        QJsonObject resp = doc.object();
        QString id = resp["id"].toString();
        bool ok = resp["ok"].toBool();
        QJsonValue data = resp["data"];
        QString error = resp["error"].toString();
        if (m_pending.contains(id)) {
            auto cb = m_pending.take(id);
            cb(ok, data, error);
        }
    }
}

void Orchestrator::onProcessError(QProcess::ProcessError error) {
    qCritical() << "hollow-orc error:" << error;
}
