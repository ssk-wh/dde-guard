#include "recorder.h"

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

#define CONF_DIR            ("/var/lib/deepin/dde-guard/services/")
#define OPERATION_DIR       (QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + QDir::separator() + qApp->applicationName() + QDir::separator())
#define OPERATION_FILE      (OPERATION_DIR + "conf.ini")

Q_LOGGING_CATEGORY(recorder, "recorder")

Recorder::Recorder()
{
    QDir dir(CONF_DIR);
    if (!dir.exists()) {
        qCDebug(recorder) << "Directory not exist, try to create:" << CONF_DIR;
        bool ret = dir.mkpath(CONF_DIR);
        qCDebug(recorder) << "create:" << ret;
    }

    QDir dir2(OPERATION_DIR);
    if (!dir2.exists()) {
        qCDebug(recorder) << "Directory not exist, try to create:" << OPERATION_DIR;
        bool ret = dir2.mkpath(OPERATION_DIR);
        qCDebug(recorder) << "create:" << ret;
    }
}

Recorder::~Recorder()
{
}

Recorder *Recorder::instance()
{
    return new Recorder;
}

void Recorder::loadServices(ServiceMap &map)
{
    qCDebug(recorder) << "load conf files";

    map.clear();

    QDir dir(CONF_DIR);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    const QFileInfoList &fileinfoList = dir.entryInfoList();

    for (const QFileInfo &fileInfo : fileinfoList) {
        qCDebug(recorder) << "read from file:" << fileInfo.filePath();
        QSettings settings(fileInfo.filePath(), QSettings::IniFormat);
        settings.beginGroup("Conf");

        Service s {
            settings.value("policy_action").toString(),
                    settings.value("check").toString(),
                    settings.value("repair").toString()
        };

        if (s.check.isEmpty() || s.repair.isEmpty()) {
            qCWarning(recorder) << "Invalid conf file:" << fileInfo.filePath();
            continue;
        }

        map.insert(fileInfo.baseName(), s);
    }
}

void Recorder::loadConfs(Confs &confs)
{
    qCDebug(recorder) << "load conf files";

    confs.clear();

    QSettings setting(OPERATION_FILE, QSettings::IniFormat);
    auto groups = setting.childGroups();
    for (const auto &group : groups) {
        setting.beginGroup(group);
        Conf c;
        c.name = group;
        c.args = setting.value("args").toStringList();
        confs.append(c);
    }
}

void Recorder::add(const QString &name, const QStringList &args)
{
    qCDebug(recorder) << "add operation:" << name << "args:" << args;

    QSettings setting(OPERATION_FILE, QSettings::IniFormat);
    setting.beginGroup(name);
    setting.setValue("args", args);
    setting.sync();
}

void Recorder::remove(const QString &name)
{
    qCDebug(recorder) << "remove operation:" << name;

    if (name.isEmpty()) {
        qWarning() << "ignore invalid para";
        return;
    }

    QSettings setting(OPERATION_FILE, QSettings::IniFormat);
    setting.remove(name);
    setting.sync();
}
