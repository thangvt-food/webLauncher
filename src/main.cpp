#include <QApplication>
#include <QFont>

#include "app/window.h"

int main(int argc, char* argv[]) {
    // Giảm số arena malloc của glibc -> giảm bộ nhớ ảo (Qt tạo nhiều thread)
    qputenv("MALLOC_ARENA_MAX", "2");

    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("WebLauncher"));
    QApplication::setOrganizationName(QStringLiteral("WebLauncher"));
    QApplication::setApplicationVersion(QStringLiteral("1.0.0"));
    QApplication::setQuitOnLastWindowClosed(false); // cho phép thu nhỏ vào khay

    // Font mặc định hệ thống (Linux Mint: Noto Sans) đã hỗ trợ tiếng Việt.
    // Không quét toàn bộ font families để tránh nạp cơ sở dữ liệu font nặng.
    QApplication::setFont(QFont(QStringLiteral("Noto Sans"), 10));

    MainWindow w;
    w.show();

    return app.exec();
}
