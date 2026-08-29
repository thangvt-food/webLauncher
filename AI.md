# Khai báo sử dụng AI trong quá trình phát triển

Dự án này được phát triển có sự hỗ trợ của **trợ lý AI** (Command Code), theo tinh thần minh bạch về việc sử dụng công cụ AI trong mã nguồn.

## Mức độ sử dụng

- **Viết mã**: phần lớn mã nguồn C++ (giao diện Qt, bộ quét dự án, cơ sở dữ liệu JSON) được trợ lý AI hỗ trợ viết và hoàn thiện dưới sự giám sát, kiểm tra và chỉnh sửa của tác giả.
- **Sửa lỗi & build**: AI hỗ trợ chẩn đoán và sửa các lỗi biên dịch/liên kết trong quá trình build thực tế.
- **Tài liệu**: README, khai báo này và các chú thích trong mã được soạn có sự hỗ trợ của AI.

## Cam kết

- Tất cả mã được **tác giả con người xem xét, kiểm chứng bằng build thực tế** trước khi phát hành.
- AI không tự quyết định kiến trúc; các quyết định thiết kế chính (Qt5 + QTermWidget, lưu JSON, cấu trúc module) do tác giả lựa chọn.
- Mã nguồn thuộc sở hữu của tác giả và được cấp phép theo [MIT](LICENSE).

*Cập nhật lần cuối: 2026-08-29*
