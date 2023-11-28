#include "guard.h"

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    if (!QDBusConnection::sessionBus().registerService("org.deepin.dde.Guard")) {
        qWarning() << "Failed to register dbus service, error:" << QDBusConnection::sessionBus().lastError().message();
        exit(-1);
    }

    Guard d;
    if (!QDBusConnection::sessionBus().registerObject("/org/deepin/dde/Guard", &d, QDBusConnection::ExportAllSlots)) {
        qWarning() << "Failed to register dbus object, error:" << QDBusConnection::sessionBus().lastError().name();
        exit(-1);
    }

    return QCoreApplication::exec();
}
