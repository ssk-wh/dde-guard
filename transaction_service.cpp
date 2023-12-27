#include "transaction_service.h"
#include "protection_manager.h"

#include <QDebug>
#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include <QLoggingCategory>
#include <QApplication>
#include <QDBusConnection>
#include <QDBusConnectionInterface>

#include <polkit-qt5-1/PolkitQt1/Authority>

Q_LOGGING_CATEGORY(service, "service");

#define PMANAGER ProtectionManager::instance()

int executeProcess(const QString& command) {
    QProcess p;
    p.start(command);
    if (!p.waitForStarted()) {
        qCDebug(service) << "Failed to start the process, command:" << command;
        return -1; // Or any error code indicating the failure.
    }
    if (!p.waitForFinished()) {
        qCDebug(service) << "Failed to finish the process, command:" << command;
        return -1; // Or any error code indicating the failure.
    }
    return p.exitCode();
}

using namespace PolkitQt1;

TransactionService::TransactionService(QObject *parent)
    : QObject(parent)
{
    // 读取应用安装的配置文件，配置文件需要有policy_action、check_exec、repair_exec三个字段，不可缺少任意字段
    PMANAGER->loadServices(m_serciceMap);

    // 用户操作记录
    Protections pts;
    PMANAGER->loadProtections(pts);
    for (const auto &op : pts) {
        if (m_serciceMap.contains(op.name)) {
            auto check = m_serciceMap[op.name].check;
            auto repair = m_serciceMap[op.name].repair;
            auto policy_action = m_serciceMap[op.name].policy_action;

            // 进行修复操作
            QFuture<void> future = QtConcurrent::run([ = ] () {
                auto code = executeProcess(check + " " + op.args.join(" "));
                if (code != 0) {
                    qCDebug(service) << "Failed to check operation:" << op.name << ", begin repair...";
                    auto code = executeProcess(repair + " " + op.args.join(" "));
                    if (code != 0) {
                        qCDebug(service) << "Failed to repair";
                    } else {
                        qCDebug(service) << "Repair success";
                    }
                }

                PMANAGER->remove(op.name);
            });

            auto watcher = new QFutureWatcher<void>();
            connect(watcher, &QFutureWatcher<void>::finished, this, &TransactionService::onWatcherFinished);
            watcher->setFuture(future);
            m_watchers.append(watcher);
        }
    }

    maybeQuit();
}

void TransactionService::add(const QString &operation, const QStringList &args)
{
    if (!calledFromDBus()) {
        qCWarning(service) << "Only allowed to call via dbus.";
        return;
    }

    if (!m_serciceMap.contains(operation)) {
        qCWarning(service) << "Invalid operation:" << operation;
        return;
    }

    auto policy_action = m_serciceMap[operation].policy_action;
    if (!policy_action.isEmpty()) {
        Authority::Result result;
        result = Authority::instance()->checkAuthorizationSync(policy_action
                                                               , UnixProcessSubject(connection().interface()->servicePid(message().service()))
                                                               , Authority::AllowUserInteraction);
        if (result != Authority::Yes) {
            qCWarning(service) << "Permission denied!";
            return;
        }
    }

    PMANAGER->add(operation, args);
}

void TransactionService::remove(const QString &operation)
{
    if (!calledFromDBus()) {
        qCWarning(service) << "Only allowed to call via dbus.";
        return;
    }

    PMANAGER->remove(operation);
}

void TransactionService::onWatcherFinished()
{
    Q_ASSERT(sender());
    auto watcher = dynamic_cast<QFutureWatcher<void> *>(sender());
    m_watchers.removeAll(watcher);

    maybeQuit();
}

void TransactionService::maybeQuit()
{
    // There is no need to run it all the time to save resources.
    if (m_watchers.isEmpty()) {
//        QMetaObject::invokeMethod(QCoreApplication::instance(), &QCoreApplication::quit, Qt::QueuedConnection);
    }
}
