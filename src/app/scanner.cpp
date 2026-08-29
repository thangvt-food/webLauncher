#include "scanner.h"

#include <QDir>
#include <QDirIterator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSet>

#include "core/util.h"

Project ProjectScanner::scanDirectory(const QString& dir, const Database& db) {
    Project p;
    p.path = QDir(dir).absolutePath();
    p.packagePath = QDir(p.path).filePath(QStringLiteral("package.json"));
    if (!QFileInfo::exists(p.packagePath))
        return Project();

    p.id = util::stableId(p.path);
    analyzePackageJson(p);
    // Giữ nguyên trạng thái yêu thích / lastRun nếu đã có trong DB
    if (const Project* existing = db.find(p.id)) {
        p.favorite = existing->favorite;
        p.lastRun = existing->lastRun;
        p.runScript = existing->runScript.isEmpty() ? p.runScript : existing->runScript;
        p.extraArgs = existing->extraArgs;
        p.customCmd = existing->customCmd;
    }
    return p;
}

void ProjectScanner::analyzePackageJson(Project& p) {
    const QString raw = util::readFile(p.packagePath);
    const QJsonDocument doc = QJsonDocument::fromJson(raw.toUtf8());
    if (!doc.isObject())
        return;

    const QJsonObject root = doc.object();
    p.name = root.value(QStringLiteral("name")).toString();
    p.manager = util::detectManager(p.path);

    QJsonObject scripts = root.value(QStringLiteral("scripts")).toObject();
    const QString dev = scripts.value(QStringLiteral("dev")).toString();
    const QString start = scripts.value(QStringLiteral("start")).toString();
    const QString build = scripts.value(QStringLiteral("build")).toString();

    p.devCommand = dev.isEmpty() ? QString() : p.manager + " run dev";
    p.startCommand = start.isEmpty() ? QString() : p.manager + " run start";
    p.buildCommand = build.isEmpty() ? QString() : p.manager + " run build";

    // Chọn script mặc định: dev > start
    if (!dev.isEmpty())
        p.runScript = QStringLiteral("dev");
    else if (!start.isEmpty())
        p.runScript = QStringLiteral("start");
    else if (!build.isEmpty())
        p.runScript = QStringLiteral("build");

    // Port heuristic
    const QJsonObject deps = root.value(QStringLiteral("dependencies")).toObject();
    const QJsonObject devDeps = root.value(QStringLiteral("devDependencies")).toObject();
    const QString scriptsJson = QString::fromUtf8(QJsonDocument(scripts).toJson());
    const QString depsJson = QString::fromUtf8(QJsonDocument(deps).toJson()) +
                             QString::fromUtf8(QJsonDocument(devDeps).toJson());
    p.port = util::detectPort(scriptsJson, depsJson, p.name);
}

bool ProjectScanner::shouldSkipDir(const QString& dirName) {
    static const QSet<QString> skip = {
        QStringLiteral("node_modules"), QStringLiteral(".git"), QStringLiteral("dist"),
        QStringLiteral("build"), QStringLiteral(".next"), QStringLiteral("out"),
        QStringLiteral("coverage"), QStringLiteral(".cache"), QStringLiteral(".vscode"),
        QStringLiteral(".idea"), QStringLiteral("target"), QStringLiteral(".venv"),
        QStringLiteral("venv"), QStringLiteral(".nuxt"), QStringLiteral(".svelte-kit"),
        QStringLiteral(".turbo"), QStringLiteral(".yarn"), QStringLiteral(".pnpm-store"),
    };
    if (skip.contains(dirName))
        return true;
    // thư mục ẩn khác cũng bỏ qua
    return dirName.startsWith(QLatin1Char('.'));
}

QVector<Project> ProjectScanner::scanRecursive(const QString& root, int maxDepth) {
    QVector<Project> out;
    Database db; // dùng để giữ trạng thái yêu thích/lastRun khi quét lại

    QDirIterator it(root,
                    QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable,
                    QDirIterator::Subdirectories);
    while (it.hasNext()) {
        const QString dir = it.next();
        const QString relative = QDir(root).relativeFilePath(dir);
        const int depth = relative.count(QLatin1Char('/')) + 1;
        if (depth > maxDepth)
            continue;

        const QString name = QFileInfo(dir).fileName();
        if (shouldSkipDir(name))
            continue;

        if (util::hasPackageJson(dir)) {
            Project p = scanDirectory(dir, db);
            if (!p.id.isEmpty())
                out.append(p);
        }
    }
    return out;
}
