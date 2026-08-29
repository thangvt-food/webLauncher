# Tài liệu dự án Web Launcher

App Qt5/C++ launcher dự án web: chọn dự án → chạy dev server trong terminal tích hợp (QTermWidget).

## Đọc nhanh theo việc cần làm

| Nếu bạn cần... | Đọc file |
|---|---|
| Build / chạy / cài dependency | [docs/build.md](build.md) |
| Terminal, copy/paste log | [docs/terminal.md](terminal.md) |
| Cửa sổ chính, toolbar, menu, tab | [docs/window.md](window.md) |
| Database, Project, Scanner, util | [docs/core.md](core.md) |
| Lỗi thường gặp, quyết định kỹ thuật | [docs/troubleshooting.md](troubleshooting.md) |

## Cấu trúc mã nguồn

```
src/
├── main.cpp              # entry point: QApplication, font, tray flag
├── app/
│   ├── window.{h,cpp}    # MainWindow: toolbar, list, tabs, terminal (file lớn nhất)
│   ├── scanner.{h,cpp}   # quét thư mục tìm package.json
│   ├── trayicon.{h,cpp}  # khay hệ thống
│   └── settingsdialog.{h,cpp} # hộp thoại cài đặt (dark theme, tray)
└── core/
    ├── database.{h,cpp}  # lưu danh sách dự án JSON (~/.config/weblauncher/projects.json)
    ├── project.{h,cpp}   # struct Project + lệnh chạy
    └── util.{h,cpp}      # hàm tiện ích (id, port, manager, shell quote)
```

## Nguyên tắc chung

- **Qt5 Widgets + QTermWidget** (không QML), C++17, AUTOMOC.
- UI tiếng Việt (toàn bộ chuỗi qua `tr()`).
- Dữ liệu dự án lưu JSON cục bộ, không có server.
- Terminal dùng QTermWidget 1.4.0 — **không có phím tắt/context menu mặc định**, mọi copy/paste do MainWindow tự cài (xem [terminal.md](terminal.md)).
