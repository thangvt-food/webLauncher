#pragma once

#include <QVector>

#include "core/database.h"
#include "core/project.h"

// Quét thư mục tìm dự án web (có package.json) và phân tích scripts.
class ProjectScanner {
public:
    // Quét 1 thư mục (không đệ quy). Trả về dự án tìm được (rỗng nếu không có package.json)
    static Project scanDirectory(const QString& dir, const Database& db);

    // Quét đệ quy tối đa `maxDepth` cấp, bỏ qua thư mục ẩn / node_modules / build...
    static QVector<Project> scanRecursive(const QString& root, int maxDepth = 3);

    // Phân tích package.json -> điền lệnh, port, manager cho dự án
    static void analyzePackageJson(Project& p);

private:
    static bool shouldSkipDir(const QString& dirName);
};
