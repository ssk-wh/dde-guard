#ifndef GUARD_H
#define GUARD_H

#include <QObject>
#include <QDBusContext>
#include <QFutureWatcher>

class Guard : public QObject, public QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.deepin.dde.Guard")

public:
    explicit Guard(QObject *parent = nullptr);

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
};

#endif // GUARD_H
