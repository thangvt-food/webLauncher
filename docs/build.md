# Build & chạy

## Dependencies (Ubuntu/Mint)

```bash
sudo apt install build-essential cmake qt5-qmake qtbase5-dev \
                 libqtermwidget5-0 libqtermwidget5-0-dev qtermwidget5-data
```

## Build

```bash
cmake -S . -B build
cmake --build build
./build/weblauncher
```

## Lưu ý build

- `CMAKE_CXX_STANDARD 17`, `AUTOMOC ON` — thêm file `.h` có `Q_OBJECT` thì phải thêm vào `add_executable` trong `CMakeLists.txt`.
- Link `Qt5::Widgets` + `qtermwidget5`. Include dir: `/usr/include/qtermwidget5` (đã có trong CMake).
- **Không cần** Qt5::Svg (đã bỏ; icon dùng emoji text).
- Build nhanh: `cmake --build build -j$(nproc)`.

## Test nhanh không cần hiển thị

```bash
./build/weblauncher -platform offscreen
```

Cảnh báo `systemTrayWindowChanged` khi chạy offscreen là bình thường (không có tray trong môi trường ảo).
