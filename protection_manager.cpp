#include "protection_manager.h"

#include <stdio.h>
#include <unistd.h>

#include <QFile>
#include <QDebug>
#include <QLoggingCategory>
#include <QDataStream>
#include <QDir>
#include <QSettings>
#include <QApplication>
#include <QStandardPaths>
#include <QSettings>

#define CONF_DIR            QString("/var/lib/deepin/dde-transactionprotect/services/")
#define OPERATION_DIR       QString("/usr/share/dde-transactionprotect/")

Q_LOGGING_CATEGORY(manager, "manager")

ProtectionManager::ProtectionManager()
{
    QDir dir(CONF_DIR);
    if (!dir.exists()) {
        qCDebug(manager) << "Directory not exist, try to create:" << CONF_DIR;
        bool ret = dir.mkpath(CONF_DIR);
        qCDebug(manager) << "create:" << ret;
    }

    QDir dir2(OPERATION_DIR);
    if (!dir2.exists()) {
        qCDebug(manager) << "Directory not exist, try to create:" << OPERATION_DIR;
        bool ret = dir2.mkpath(OPERATION_DIR);
        qCDebug(manager) << "create:" << ret;
    }
}

ProtectionManager::~ProtectionManager()
{
}

ProtectionManager *ProtectionManager::instance()
{
    return new ProtectionManager;
}

void ProtectionManager::loadServices(ServiceMap &map)
{
    qCDebug(manager) << "load service config, dir:" << CONF_DIR;

    map.clear();

    QDir dir(CONF_DIR);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    const QFileInfoList &fileinfoList = dir.entryInfoList();

    for (const QFileInfo &fileInfo : fileinfoList) {
        qCDebug(manager) << fileInfo.fileName();
        QSettings settings(fileInfo.filePath(), QSettings::IniFormat);
        settings.beginGroup("Conf");

        Service s {
            settings.value("policy_action").toString(),
                    settings.value("check").toString(),
                    settings.value("repair").toString()
        };

        if (s.check.isEmpty() || s.repair.isEmpty()) {
            qCWarning(manager) << "Invalid conf file:" << fileInfo.filePath();
            continue;
        }

        map.insert(fileInfo.baseName(), s);
    }
}

void ProtectionManager::loadProtections(Protections &ops)
{
    qCDebug(manager) << "load protection config, dir:" << OPERATION_DIR;
    ops.clear();

    QDir dir(OPERATION_DIR);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    const QFileInfoList &fileinfoList = dir.entryInfoList();
    for (const auto &fileInfo : fileinfoList) {
        qCDebug(manager) << fileInfo.filePath();
        QSettings setting(fileInfo.filePath(), QSettings::IniFormat);
        Protection c;
        c.name = fileInfo.fileName();
        c.args = setting.value("args").toStringList();
        ops.append(c);
    }
}

void ProtectionManager::add(const QString &name, const QStringList &args)
{
    qCDebug(manager) << "add operation:" << name << "args:" << args;

    QSettings setting(OPERATION_DIR + name, QSettings::IniFormat);
    setting.setValue("args", args);
    setting.sync();
}

void ProtectionManager::remove(const QString &name)
{
    qCDebug(manager) << "remove operation:" << name;

    if (name.isEmpty()) {
        qWarning() << "ignore invalid para";
        return;
    }

    QFile file(OPERATION_DIR + name);
    bool removed = file.remove();
    if (!removed) {
        qCWarning(manager) << "Failed to remove file:" << file.fileName();
    }
}
