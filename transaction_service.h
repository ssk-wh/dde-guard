#ifndef TRANSACTION_SERVICE_H
#define TRANSACTION_SERVICE_H

#include <QObject>
#include <QDBusContext>
#include <QFutureWatcher>

#include "protection_manager.h"

class TransactionService : public QObject, public QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.deepin.dde.TransactionProtect")

public:
    explicit TransactionService(QObject *parent = nullptr);

public Q_SLOTS:
    void add(const QString &operation, const QStringList &args);
    void remove(const QString &operation);

private Q_SLOTS:
    void onWatcherFinished();

private:
    void maybeQuit();

private:
    using WatcherList = QList<QFutureWatcher<void> *>;
    WatcherList m_watchers;

    ServiceMap m_serciceMap;
};

#endif // TRANSACTION_SERVICE_H
