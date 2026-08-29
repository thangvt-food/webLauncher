#pragma once

#include <QString>
#include <QStringList>

namespace util {

// Băm chuỗi thành id ổn định (không cần Qt5.14+ cho hashing 64-bit)
QString stableId(const QString& input);

// Đọc nội dung file (trả về QString rỗng nếu lỗi)
QString readFile(const QString& path);

// Kiểm tra thư mục có package.json không
bool hasPackageJson(const QString& dir);

// Nhận diện trình quản lý gói dựa trên file khóa: pnpm-lock.yaml, yarn.lock, bun.lockb, ...
QString detectManager(const QString& dir);

// Tìm port dựa trên scripts / devDependencies (heuristic)
int detectPort(const QString& scriptsJson, const QString& depsJson, const QString& packageName);

// Lấy tên dự án từ package.json
QString packageName(const QString& json);

// Bọc chuỗi trong dấu nháy đơn an toàn cho shell (POSIX)
QString shellQuote(const QString& s);

} // namespace util
