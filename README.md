# Web Launcher 🚀

Ứng dụng **native C++ (Qt5)** giúp khởi chạy nhanh các dự án web (React, Next.js, Vite, ...) mà **không cần gõ lệnh thủ công**. Quét thư mục dự án, đọc `package.json`, lưu vào cơ sở dữ liệu JSON và chạy dev server ngay trong terminal tích hợp — hỗ trợ tiếng Việt đầy đủ.

## ✨ Tính năng

- **Quét & lưu dự án**: thêm thư mục (hoặc quét đệ quy), app tự tìm `package.json`, phân tích scripts (`dev`, `start`, `build`...) và lưu vào cơ sở dữ liệu JSON
- **Khởi chạy 1 cú nhấp**: chọn dự án → bấm **▶ Chạy** → app tự `cd` vào dự án và chạy lệnh dev server
- **Tự nhận diện trình quản lý gói**: npm / yarn / pnpm / bun (dựa trên lock file)
- **Tự nhận diện port**: Next.js (3000), Vite (5173), CRA (3000), Nuxt (3000), Gatsby (8000), Vue CLI (8080), Angular (4200)... hoặc đọc từ `-p`/`--port` trong scripts
- **Terminal tích hợp**: widget QTermWidget, mã hóa UTF-8, font tiếng Việt — log dev server hiển thị ngay trong app
- **Nhiều tab**: mỗi dự án chạy một tab terminal riêng, đóng tab = dừng server
- **Yêu thích & gần đây**: dự án ★ ưu tiên lên đầu, sắp xếp theo lần chạy gần nhất
- **Khay hệ thống**: thu nhỏ vào tray, mở lại nhanh
- **Mở trình duyệt 1 chạm**: tự mở đúng `http://localhost:<port>` khi server sẵn sàng
- **Giao diện tối** & tiếng Việt toàn giao diện

## 📸 Giao diện

```
┌──────────────────────────────────────────────────────┐
│ ▶ Chạy   ■ Dừng   |  ☆ Yêu thích   🗑 Xóa            │
├──────────────────────────┬───────────────────────────┤
│ ★ my-vite-app  [dev]     │ ┌─ my-vite-app ────────┐  │
│   my-next-app  [dev]     │ │ $ cd /home/me/app    │  │
│   landing-page [start]   │ │ ▶ Đang chạy: npm run │  │
│                          │ │ dev                  │  │
│                          │ │ VITE v5.0.0 ready in│  │
│                          │ │ 300 ms               │  │
│                          │ └──────────────────────┘  │
├──────────────────────────┴───────────────────────────┤
│ 3 dự án                                               │
└──────────────────────────────────────────────────────┘
```

## 🔧 Cài đặt

### Dependencies (Linux Mint / Ubuntu / Debian)

```bash
sudo apt-get update
sudo apt-get install -y cmake qtbase5-dev libqt5svg5-dev libqtermwidget5-1-dev libutf8proc-dev
```

### Build & chạy

```bash
cmake -B build
cmake --build build -j$(nproc)
./build/weblauncher
```

## 🗂️ Cấu trúc dự án

```
src/
  main.cpp                  # Entry point
  app/
    window.cpp/.h           # Cửa sổ chính, danh sách dự án, tab terminal
    scanner.cpp/.h          # Quét thư mục & phân tích package.json
    trayicon.cpp/.h         # Khay hệ thống
    settingsdialog.cpp/.h   # Hộp thoại cài đặt
  core/
    project.cpp/.h          # Cấu trúc dữ liệu dự án
    database.cpp/.h         # Cơ sở dữ liệu JSON
    util.cpp/.h             # Tiện ích (hash, phân tích port, shell quote)
```

## 💾 Cơ sở dữ liệu

Dự án được lưu tại `~/.config/WebLauncher/WebLauncher/projects.json`. Mỗi bản ghi gồm: tên, đường dẫn, lệnh chạy, port, trình quản lý gói, trạng thái yêu thích và lần chạy gần nhất.

## 🤖 Khai báo sử dụng AI

Dự án này có sử dụng trợ lý AI (Command Code) trong quá trình phát triển. Xem chi tiết tại [AI.md](AI.md).

## 📄 Giấy phép

[MIT](LICENSE)
