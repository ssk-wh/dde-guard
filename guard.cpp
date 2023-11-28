#include "guard.h"
#include "recorder.h"

#include <QDebug>
#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include <QLoggingCategory>
#include <QApplication>

#include <polkit-qt5-1/PolkitQt1/Authority>

Q_LOGGING_CATEGORY(guard, "guard")

#define RECORD Recorder::instance()

int executeProcess(const QString& command) {
    QProcess p;
    p.start(command);
    if (!p.waitForStarted()) {
        qDebug() << "Failed to start the process.";
        return -1; // Or any error code indicating the failure.
    }
    if (!p.waitForFinished()) {
        qDebug() << "Failed to finish the process.";
        return -1; // Or any error code indicating the failure.
    }
    return p.exitCode();
}

using namespace PolkitQt1;

Guard::Guard(QObject *parent)
    : QObject(parent)
{
    // 配置文件列表
    ServiceMap maps;
    RECORD->loadServices(maps);

    // 用户操作记录
    Confs ops;
    RECORD->loadConfs(ops);
    for (const auto &op : ops) {
        if (maps.contains(op.name)) {
            auto check = maps[op.name].check;
            auto repair = maps[op.name].repair;
            auto policy_action = maps[op.name].policy_action;

            // 进行修复操作
            QFuture<void> future = QtConcurrent::run([ = ] () {
                auto code = executeProcess(check + " " + op.args.join(" "));
                if (code != 0) {
                    qDebug().noquote() << "Failed to check";
                    // 部分操作需要提权处理
                    if (!policy_action.isEmpty()) {
                        Authority::Result result;
                        result = Authority::instance()->checkAuthorizationSync(policy_action
                                                                               , UnixProcessSubject(getpid())
                                                                               , Authority::AllowUserInteraction);
                        if (result != Authority::Yes) {
                            qWarning() << "Permission denied!";

                            RECORD->remove(op.name);
                            return;
                        }
                    }

                    auto code = executeProcess(repair + " " + op.args.join(" "));
                    if (code != 0) {
                        qDebug().noquote() << "Failed to repair";
                    }
                }

                RECORD->remove(op.name);
            });

            auto watcher = new QFutureWatcher<void>();
            connect(watcher, &QFutureWatcher<void>::finished, this, &Guard::onWatcherFinished);
            watcher->setFuture(future);
            m_watchers.append(watcher);
        }
    }

    maybeQuit();
}

void Guard::add(const QString &operation, const QStringList &args)
{
    // TODO 检测是否存在对应的配置文件
    RECORD->add(operation, args);
}

void Guard::remove(const QString &operation)
{
    RECORD->remove(operation);
}

void Guard::onWatcherFinished()
{
    Q_ASSERT(sender());
    auto watcher = dynamic_cast<QFutureWatcher<void> *>(sender());
    m_watchers.removeAll(watcher);

    maybeQuit();
}

void Guard::maybeQuit()
{
    // There is no need to run it all the time to save resources.
    if (m_watchers.isEmpty()) {
//        QMetaObject::invokeMethod(QCoreApplication::instance(), &QCoreApplication::quit, Qt::QueuedConnection);
    }
}
