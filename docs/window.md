# MainWindow & UI (src/app/window.cpp)

File lớn nhất (~510 dòng) — chứa toàn bộ giao diện chính.

## Bố cục

```
MainWindow (QMainWindow)
├── Toolbar "Chính": ▶ Chạy | ■ Dừng | ☆ Yêu thích | 🗑 Xóa | ⧉ Copy log
├── QSplitter (ngang)
│   ├── QListWidget m_list     # danh sách dự án (trái, min 300px)
│   └── QTabWidget m_tabs      # tab terminal (phải)
├── Menu bar: Tệp / Xem / Trợ giúp
├── Status bar: QLabel m_status
└── TrayIcon (thu nhỏ vào khay)
```

## Trạng thái & dữ liệu chính

| Member | Ý nghĩa |
|---|---|
| `m_db` | Database (JSON ~/.config/weblauncher/projects.json) |
| `m_projects` | `QVector<Project>` đang hiển thị (đã sort: yêu thích → lastRun) |
| `m_tabForPath` | `QHash<path, tabIndex>` — map dự án ↔ tab terminal |
| `m_settings` | QSettings ("WebLauncher"/"WebLauncher"): darkTheme, minimizeToTray, geometry |

## Luồng chính

- **Chạy dự án** (`runProject`): nếu chưa có tab → `openTerminal(p)` + tự mở browser sau 1.5s (nếu `p.port > 0`); nếu có → chuyển tới tab đó.
- **Mở terminal** (`openTerminal`): tạo QTermWidget, `sendText` lệnh khởi động, thêm tab, ghi `lastRun`. Khi shell kết thúc (`finished`) → đóng tab + dọn `m_tabForPath`.
- **Đóng tab** (`closeTab`): gửi `exit\n` cho shell, xóa khỏi map, `removeTab` + `deleteLater`.
- **Dừng dự án** (`stopProject`): `closeTab` tab tương ứng (shell thoát → server dừng).
- **Làm mới** (`refreshList`): quét lại từng dự án, giữ favorite/lastRun, cập nhật DB.

## Context menu

- **Danh sách dự án**: `customContextMenuRequested` → Chạy / Dừng / Mở trình duyệt / Mở thư mục / Yêu thích / Xóa.
- **Terminal**: event filter cấp QApplication (xem [terminal.md](terminal.md)) — không dùng `customContextMenuRequested` trên tab (chỉ bắt được click trên tab bar, không phải vùng terminal).

## Cài đặt (SettingsDialog)

- **Dark theme**: apply palette Fusion trong `applySettings()`, đọc từ QSettings key `darkTheme`.
- **Minimize to tray**: `minimizeToTray` — đóng cửa sổ thì ẩn thay vì thoát (`closeEvent`).

## Ghi chú

- `updateRunButtonState()` bật/tắt nút theo trạng thái list + tab hiện tại.
- `closeEvent` lưu geometry; destructor đóng mọi tab còn lại (dừng dev server).
- Khi sửa, nhớ cập nhật `window.h` (slots + members) — AUTOMOC cần khai báo đúng.
