#include "database.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

Database::Database() { load(); }

QString Database::databasePath() {
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(dir);
    return dir + QStringLiteral("/projects.json");
}

void Database::load() {
    m_projects.clear();
    QFile f(databasePath());
    if (!f.open(QIODevice::ReadOnly))
        return;
    const QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();
    if (!doc.isArray())
        return;

    const QJsonArray arr = doc.array();
    for (const QJsonValue& v : arr) {
        const QJsonObject o = v.toObject();
        Project p;
        p.id           = o.value("id").toString();
        p.name         = o.value("name").toString();
        p.path         = o.value("path").toString();
        p.packagePath  = o.value("packagePath").toString();
        p.devCommand   = o.value("devCommand").toString();
        p.startCommand = o.value("startCommand").toString();
        p.buildCommand = o.value("buildCommand").toString();
        p.runScript    = o.value("runScript").toString();
        p.extraArgs    = o.value("extraArgs").toString();
        p.customCmd    = o.value("customCmd").toString();
        p.manager      = o.value("manager").toString();
        p.port         = o.value("port").toInt();
        p.favorite     = o.value("favorite").toBool();
        p.lastRun      = o.value("lastRun").toVariant().toLongLong();
        if (p.id.isEmpty() || p.path.isEmpty())
            continue;
        m_projects.append(p);
    }
}

void Database::save() const {
    QJsonArray arr;
    for (const Project& p : m_projects) {
        QJsonObject o;
        o["id"]           = p.id;
        o["name"]         = p.name;
        o["path"]         = p.path;
        o["packagePath"]  = p.packagePath;
        o["devCommand"]   = p.devCommand;
        o["startCommand"] = p.startCommand;
        o["buildCommand"] = p.buildCommand;
        o["runScript"]    = p.runScript;
        o["extraArgs"]    = p.extraArgs;
        o["customCmd"]    = p.customCmd;
        o["manager"]      = p.manager;
        o["port"]         = p.port;
        o["favorite"]     = p.favorite;
        o["lastRun"]      = qint64(p.lastRun);
        arr.append(o);
    }
    QFile f(databasePath());
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;
    f.write(QJsonDocument(arr).toJson(QJsonDocument::Indented));
    f.close();
}

Project* Database::find(const QString& id) {
    for (Project& p : m_projects)
        if (p.id == id)
            return &p;
    return nullptr;
}

const Project* Database::find(const QString& id) const {
    for (const Project& p : m_projects)
        if (p.id == id)
            return &p;
    return nullptr;
}

bool Database::addProject(const Project& p) {
    if (containsPath(p.path))
        return false;
    m_projects.append(p);
    save();
    return true;
}

void Database::updateProject(const Project& p) {
    for (int i = 0; i < m_projects.size(); ++i) {
        if (m_projects[i].id == p.id) {
            m_projects[i] = p;
            save();
            return;
        }
    }
}

void Database::removeProject(const QString& id) {
    for (int i = 0; i < m_projects.size(); ++i) {
        if (m_projects[i].id == id) {
            m_projects.removeAt(i);
            save();
            return;
        }
    }
}

bool Database::containsPath(const QString& path) const {
    for (const Project& p : m_projects)
        if (QDir(p.path).absolutePath() == QDir(path).absolutePath())
            return true;
    return false;
}
