# Lỗi thường gặp & quyết định kỹ thuật

## Terminal copy/paste

**Triệu chứng**: bôi đen log trong terminal rồi copy không được; không có menu chuột phải.

**Nguyên nhân** (đã xác minh source QTermWidget 1.4.0):
- Không có phím tắt copy/paste mặc định.
- TerminalDisplay override `mousePressEvent` → Qt không sinh ContextMenu event → `customContextMenuRequested` không bao giờ chạy.
- Event filter trên QTermWidget không nhận event của TerminalDisplay (con).

**Giải pháp** (đã làm):
- Event filter cấp `QApplication` bắt RightButton, tìm tổ tiên QTermWidget, hiện menu.
- QShortcut cấp window (Ctrl+Shift+C/V/L) — TerminalDisplay không chặn các tổ hợp này trong `handleShortcutOverrideEvent`.
- Nút toolbar "⧉ Copy log" dùng `saveHistory`.

> ⚠️ Khi sửa: đừng quay lại `customContextMenuRequested` cho terminal — nó không hoạt động. Giữ event filter cấp app.

## Build

| Lỗi | Cách xử lý |
|---|---|
| `find_package(QTermWidget5)` fail | Cài `libqtermwidget5-0-dev` |
| Thiếu symbol Qt5::Svg | Đã bỏ Svg khỏi CMakeLists (icon dùng emoji) |
| `Q_OBJECT` mới không được MOC | Thêm file vào `add_executable` trong CMakeLists.txt |

## Quyết định kỹ thuật (đừng đảo ngược nếu không có lý do)

1. **Không dùng Ctrl+C/V cho copy/paste terminal** — xung đột với interrupt signal / "literal next char" của shell.
2. **Nút toolbar dùng emoji text** (▶ ■ ☆ 🗑 ⧉) — không cần resource icon, giảm phụ thuộc.
3. **Lưu JSON cục bộ, không server** — đơn giản, đủ dùng cho launcher cá nhân.
4. **`MALLOC_ARENA_MAX=2` trong main.cpp** — giảm bộ nhớ ảo của glibc (Qt tạo nhiều thread).
5. **Font Noto Sans 10 cố định** — tránh quét QFontDatabase nặng; Noto Sans có sẵn trên Linux Mint, hỗ trợ tiếng Việt.
6. **`setQuitOnLastWindowClosed(false)`** — cho phép thu nhỏ vào khay; đóng cửa sổ chỉ ẩn nếu `minimizeToTray` bật.

## Cấu hình người dùng

| Nơi lưu | Nội dung |
|---|---|
| `~/.config/weblauncher/projects.json` | Danh sách dự án |
| QSettings "WebLauncher" | `darkTheme`, `minimizeToTray`, `geometry` |

Xóa `~/.config/weblauncher` để reset hoàn toàn về mặc định.
