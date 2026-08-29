#include <QApplication>
#include <QFontDatabase>
#include <QTranslator>
#include <QLibraryInfo>

#include "app/window.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("WebLauncher"));
    QApplication::setOrganizationName(QStringLiteral("WebLauncher"));
    QApplication::setApplicationVersion(QStringLiteral("1.0.0"));
    QApplication::setQuitOnLastWindowClosed(false); // cho phép thu nhỏ vào khay

    // Ưu tiên font có hỗ trợ tiếng Việt (Linux: Noto Sans / DejaVu)
    const QFontDatabase fontDb;
    const QStringList preferred = {
        QStringLiteral("Noto Sans"), QStringLiteral("Noto Sans CJK SC"),
        QStringLiteral("DejaVu Sans"), QStringLiteral("Liberation Sans")
    };
    for (const QString& family : preferred) {
        if (fontDb.families().contains(family)) {
            QApplication::setFont(QFont(family, 10));
            break;
        }
    }

    MainWindow w;
    w.show();

    return app.exec();
}
