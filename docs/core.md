# Core: database, project, scanner, util

## project.h — struct Project

Trường chính: `id` (hash theo path), `name`, `path`, `packagePath`, `devCommand`/`startCommand`/`buildCommand`, `runScript`, `extraArgs`, `customCmd`, `manager` (npm/yarn/pnpm/bun), `port`, `favorite`, `lastRun`.

- `effectiveCommand()`: lệnh thực tế khi bấm Chạy — ưu tiên `customCmd`, nếu không thì `runScript` + `extraArgs` qua manager tương ứng.
- `displayName()`: tên hiển thị.
- `projectLessThan(a, b)`: sort — yêu thích trước, rồi lastRun mới nhất.

## database.h — Database

- Lưu JSON tại `~/.config/weblauncher/projects.json` (`databasePath()`).
- API: `projects()`, `find(id)`, `addProject`, `updateProject`, `removeProject`, `containsPath(path)`, `load()`, `save()`.
- `addProject` trả `false` nếu path đã tồn tại (tránh trùng lặp).
- Tất cả thao tác đều sửa `QVector<Project>` trong bộ nhớ + `save()` ghi đĩa.

## scanner.h — ProjectScanner

- `scanDirectory(dir, db)`: quét 1 thư mục, trả Project rỗng nếu không có package.json.
- `scanRecursive(root, maxDepth=3)`: quét đệ quy, bỏ qua thư mục ẩn / `node_modules` / `build` / `.git` (`shouldSkipDir`).
- `analyzePackageJson(p)`: đọc package.json → điền manager, scripts, port (heuristic từ scripts/devDependencies).

## util.h — namespace util

| Hàm | Chức năng |
|---|---|
| `stableId(input)` | Hash ổn định (không phụ thuộc Qt5.14+ 64-bit hash) |
| `readFile(path)` | Đọc file, rỗng nếu lỗi |
| `hasPackageJson(dir)` | Kiểm tra tồn tại package.json |
| `detectManager(dir)` | Từ file khóa: pnpm-lock.yaml → pnpm, yarn.lock → yarn, bun.lockb → bun, còn lại npm |
| `detectPort(...)` | Heuristic tìm port từ scripts/deps |
| `packageName(json)` | Tên dự án từ package.json |
| `shellQuote(s)` | Bọc nháy đơn an toàn POSIX cho lệnh shell |
