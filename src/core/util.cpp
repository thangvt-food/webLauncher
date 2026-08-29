#include "util.h"

#include <QDir>
#include <QFile>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>

namespace util {

QString stableId(const QString& input) {
    const QByteArray hash = QCryptographicHash::hash(input.toUtf8(), QCryptographicHash::Md5);
    return QString::fromLatin1(hash.toHex());
}

QString readFile(const QString& path) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
        return QString();
    const QByteArray data = f.readAll();
    f.close();
    return QString::fromUtf8(data);
}

bool hasPackageJson(const QString& dir) {
    return QFileInfo::exists(QDir(dir).filePath(QStringLiteral("package.json")));
}

QString detectManager(const QString& dir) {
    if (QFileInfo::exists(QDir(dir).filePath(QStringLiteral("pnpm-lock.yaml"))))
        return QStringLiteral("pnpm");
    if (QFileInfo::exists(QDir(dir).filePath(QStringLiteral("yarn.lock"))))
        return QStringLiteral("yarn");
    if (QFileInfo::exists(QDir(dir).filePath(QStringLiteral("bun.lockb"))) ||
        QFileInfo::exists(QDir(dir).filePath(QStringLiteral("bun.lock"))))
        return QStringLiteral("bun");
    return QStringLiteral("npm");
}

int detectPort(const QString& scriptsJson, const QString& depsJson, const QString& packageName) {
    // 1) port cố định khai báo trong scripts, vd: "dev": "next dev -p 3001"
    QRegularExpression re(QStringLiteral("(?:-p|--port|PORT)\\s*[=:]?\\s*(\\d{3,5})"));
    const QRegularExpressionMatch m = re.match(scriptsJson);
    if (m.hasMatch())
        return m.captured(1).toInt();

    // 2) heuristic theo framework / tên package
    if (scriptsJson.contains(QStringLiteral("next")))
        return 3000;
    if (scriptsJson.contains(QStringLiteral("vite")))
        return 5173;
    if (scriptsJson.contains(QStringLiteral("react-scripts")) ||
        scriptsJson.contains(QStringLiteral("react-dev-utils")) ||
        scriptsJson.contains(QStringLiteral("craco")))
        return 3000;
    if (scriptsJson.contains(QStringLiteral("nuxt")))
        return 3000;
    if (scriptsJson.contains(QStringLiteral("svelte-kit")))
        return 5173;
    if (scriptsJson.contains(QStringLiteral("gatsby")))
        return 8000;
    if (scriptsJson.contains(QStringLiteral("vue-cli-service")))
        return 8080;
    if (scriptsJson.contains(QStringLiteral("angular")))
        return 4200;
    if (packageName.contains(QStringLiteral("docusaurus")))
        return 3000;
    (void)depsJson;
    return 0;
}

QString packageName(const QString& json) {
    const QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    if (!doc.isObject())
        return QString();
    return doc.object().value(QStringLiteral("name")).toString();
}

QString shellQuote(const QString& s) {
    QString out = QStringLiteral("'");
    for (const QChar& c : s) {
        if (c == QLatin1Char('\''))
            out += QStringLiteral("'\\''");
        else
            out += c;
    }
    out += QLatin1Char('\'');
    return out;
}

} // namespace util
