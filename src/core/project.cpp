#include "project.h"

#include <QFileInfo>

QString Project::effectiveCommand() const {
    if (!customCmd.trimmed().isEmpty())
        return customCmd.trimmed();
    if (runScript.isEmpty())
        return devCommand;
    // Ví dụ: manager = "npm", runScript = "dev" -> "npm run dev"
    return manager + " run " + runScript + (extraArgs.trimmed().isEmpty() ? QString() : " " + extraArgs.trimmed());
}

QString Project::displayName() const {
    return name.isEmpty() ? QFileInfo(path).fileName() : name;
}

bool projectLessThan(const Project& a, const Project& b) {
    if (a.favorite != b.favorite)
        return a.favorite;
    if (a.lastRun != b.lastRun)
        return a.lastRun > b.lastRun;
    return a.displayName().compare(b.displayName(), Qt::CaseInsensitive) < 0;
}
