#include "transaction_service.h"

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    if (!QDBusConnection::systemBus().registerService("org.deepin.dde.TransactionProtect")) {
        qWarning() << "Failed to register dbus service, error:" << QDBusConnection::systemBus().lastError().message();
        exit(-1);
    }

    TransactionService d;
    if (!QDBusConnection::systemBus().registerObject("/org/deepin/dde/TransactionProtect", &d, QDBusConnection::ExportAllSlots)) {
        qWarning() << "Failed to register dbus object, error:" << QDBusConnection::systemBus().lastError().name();
        exit(-1);
    }

    return QCoreApplication::exec();
}
