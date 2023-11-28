#ifndef RECORDER_H
#define RECORDER_H
#include <QStringList>

struct Service {
    QString policy_action;
    QString check;
    QString repair;
};

using ServiceMap = QMap<QString, Service>;


struct Conf{
    QString name;
    QStringList args;
};

using Confs = QList<Conf>;

class Recorder
{
public:
    ~ Recorder();

    static Recorder *instance();

    static void loadServices(ServiceMap &map);
    static void loadConfs(Confs &confs);

    static void add(const QString &name, const QStringList &args);
    static void remove(const QString &name);

private:
    explicit Recorder();
};

#endif // RECORDER_H
