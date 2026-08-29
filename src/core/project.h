#pragma once

#include <QString>
#include <QStringList>
#include <QMetaType>

// Một dự án web đã được quét và lưu trong cơ sở dữ liệu.
struct Project {
    QString id;          // mã định danh (hash theo đường dẫn)
    QString name;        // tên hiển thị
    QString path;        // đường dẫn tuyệt đối tới thư mục dự án
    QString packagePath; // đường dẫn tới package.json
    QString devCommand;  // lệnh chạy dev (vd: "npm run dev")
    QString startCommand;// lệnh start (nếu có)
    QString buildCommand;// lệnh build (nếu có)
    QString runScript;   // script được chọn để chạy ("dev" / "start" / ...)
    QString extraArgs;   // tham số phụ (vd: --port 3001)
    QString customCmd;   // lệnh tùy chỉnh (nếu người dùng nhập tay)
    QString manager;     // trình quản lý gói: npm | yarn | pnpm | bun
    int     port      = 0;    // port dev server (0 = chưa biết)
    bool    favorite  = false;// yêu thích
    qint64  lastRun   = 0;    // thời gian chạy gần nhất (epoch ms)

    // Lệnh thực tế sẽ chạy khi bấm "Chạy"
    QString effectiveCommand() const;
    QString displayName() const;
};

Q_DECLARE_METATYPE(Project)

// Sắp xếp: yêu thích trước, sau đó theo lần chạy gần nhất
bool projectLessThan(const Project& a, const Project& b);
