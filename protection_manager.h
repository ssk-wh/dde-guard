#ifndef PROTECTION_MANAGER_H
#define PROTECTION_MANAGER_H
#include <QStringList>

struct Service {
    QString policy_action;
    QString check;
    QString repair;
};

using ServiceMap = QMap<QString, Service>;


struct Protection{
    QString name;
    QStringList args;
};

using Protections = QList<Protection>;

class ProtectionManager
{
public:
    ~ ProtectionManager();

    static ProtectionManager *instance();

    static void loadServices(ServiceMap &map);
    static void loadProtections(Protections &ops);

    static void add(const QString &name, const QStringList &args);
    static void remove(const QString &name);

private:
    explicit ProtectionManager();
};

#endif // PROTECTION_MANAGER_H
