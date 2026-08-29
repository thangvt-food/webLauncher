# Terminal (QTermWidget) & copy/paste

## Kiến thức cốt lõi (đã xác minh từ source QTermWidget 1.4.0)

1. **QTermWidget 1.4.0 KHÔNG có phím tắt copy/paste mặc định.** `TerminalDisplay::keyPressEvent` chỉ chuyển phím tới shell, không xử lý Ctrl+Shift+C/V.
2. **TerminalDisplay nuốt right-click**: nó override `mousePressEvent`, nên Qt **không sinh ContextMenu event** — `setContextMenuPolicy(Qt::CustomContextMenu)` + `customContextMenuRequested` trên QTermWidget **KHÔNG hoạt động**.
3. Event filter cài trên QTermWidget **không bắt** event gửi tới TerminalDisplay (con).
4. `TerminalDisplay::handleShortcutOverrideEvent` chỉ chặn Tab/Delete/Home/End/Backspace/arrows/Escape — **Ctrl+Shift+C/V không bị chặn**, nên QShortcut cấp cửa sổ hoạt động được.

## Các cách copy đã cài (window.cpp)

| Cách | Chức năng |
|---|---|
| Nút toolbar **⧉ Copy log** | Copy toàn bộ history của tab hiện tại (`saveHistory`) |
| **Menu chuột phải** trong terminal | Copy vùng chọn / Copy toàn bộ log / Paste / Tìm kiếm |
| **Ctrl+Shift+C** | Copy vùng chọn (bôi đen bằng chuột) |
| **Ctrl+Shift+L** | Copy toàn bộ log |
| **Ctrl+Shift+V** | Paste từ clipboard |

Không dùng Ctrl+C/Ctrl+V — trong terminal chúng là interrupt signal và "literal next char" của shell.

## Cách triển khai (quan trọng — đừng phá vỡ)

### 1. Context menu qua event filter cấp QApplication

Trong constructor của MainWindow:
```cpp
QApplication::instance()->installEventFilter(this);
```

Trong `MainWindow::eventFilter`:
- Bắt `QEvent::MouseButtonPress` + `Qt::RightButton`.
- `qobject_cast<QWidget*>(watched)` → duyệt `parentWidget()` tìm tổ tiên là `QTermWidget`.
- Nếu tìm thấy → hiện `QMenu`, `menu.exec(w->mapToGlobal(me->pos()))`, `event->accept()`, `return true` (chặn để TerminalDisplay không emit mouseSignal xuống shell).

### 2. Phím tắt (setupUi)

```cpp
QShortcut* sc = new QShortcut(QKeySequence("Ctrl+Shift+C"), this);
sc->setContext(Qt::WidgetWithChildrenShortcut);
connect(sc, &QShortcut::activated, this, [this] { copyTerminalSelection(); });
```

`WidgetWithChildrenShortcut` — active khi focus ở bất kỳ widget con (kể cả terminal).

### 3. Copy toàn bộ log

```cpp
void MainWindow::copyHistoryToClipboard(QTermWidget* term) {
    QBuffer buffer;
    buffer.open(QIODevice::ReadWrite);
    term->saveHistory(&buffer);            // public slot, ghi history ra device
    const QString text = QString::fromUtf8(buffer.data());
    if (!text.isEmpty())
        QApplication::clipboard()->setText(text);
}
```

- `saveHistory` là **public slot** (dòng ~322 qtermwidget.h) — gọi được trực tiếp.
- `selectedText()` / `copyClipboard()` / `pasteClipboard()` cũng public.
- Nút toolbar "⧉ Copy log" chỉ enabled khi có tab đang mở (`updateRunButtonState`).

## Sự kiện terminal khác

- `QTermWidget::finished` → tab tự đóng, xóa khỏi `m_tabForPath` (xem [window.md](window.md)).
- Terminal tạo trong `openTerminal()`: font monospace 11, color scheme "Linux", scrollbar phải, UTF-8 codec.
- Lệnh khởi động: `cd <path> && clear && echo '▶ Đang chạy: <cmd>' && <cmd>`.
