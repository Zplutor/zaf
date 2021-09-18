#include <zaf/window/tooltip_window.h>
#include <zaf/base/container/utility/range.h>
#include <zaf/control/label.h>
#include <zaf/creation.h>

namespace zaf {
namespace {

int CalculateCursorHeight(const std::uint8_t* buffer, int byte_count, const BITMAP& cursor_bitmap) {

    int line_length = (cursor_bitmap.bmWidth * cursor_bitmap.bmBitsPixel) / sizeof(std::uint8_t);
    int line_count = byte_count / line_length;

    int result{};

    for (auto index : zaf::Range(0, line_count)) {

        auto line_start = buffer + (index * line_length);
        auto line_end = buffer + ((index + 1) * line_length);

        for (auto current = line_start; current < line_end; ++current) {

            if (*current != 0xff) {
                ++result;
                break;
            }
        }
    }

    return result;
}


std::optional<int> GetCursorHeightByCursorInfo(const ICONINFO& cursor_info) {

    BITMAP cursor_bitmap{};
    BOOL is_succeeded = GetObject(cursor_info.hbmMask, sizeof(cursor_bitmap), &cursor_bitmap);
    if (!is_succeeded) {
        return std::nullopt;
    }

    int bitmap_height = cursor_bitmap.bmHeight;
    if (!cursor_info.hbmColor) {
        bitmap_height /= 2;
    }

    int bit_count = cursor_bitmap.bmWidth * bitmap_height * cursor_bitmap.bmBitsPixel;
    int byte_count = bit_count / 8;
    if (bit_count % 8 != 0) {
        byte_count += 1;
    }

    auto buffer = std::make_unique<std::uint8_t[]>(byte_count);

    BITMAPINFOHEADER bitmap_header{};
    bitmap_header.biSize = sizeof(bitmap_header);
    bitmap_header.biWidth = cursor_bitmap.bmWidth;
    bitmap_header.biHeight = bitmap_height;
    bitmap_header.biPlanes = cursor_bitmap.bmPlanes;
    bitmap_header.biBitCount = cursor_bitmap.bmBitsPixel;
    bitmap_header.biCompression = BI_RGB;

    HDC dc = GetDC(nullptr);
    is_succeeded = GetDIBits(
        dc, 
        cursor_info.hbmMask, 
        0, 
        bitmap_height,
        buffer.get(), 
        reinterpret_cast<BITMAPINFO*>(&bitmap_header),
        DIB_RGB_COLORS);

    ReleaseDC(nullptr, dc);

    if (!is_succeeded) {
        return std::nullopt;
    }

    return CalculateCursorHeight(buffer.get(), byte_count, cursor_bitmap);
}


std::optional<int> GetCursorHeight() {

    HCURSOR cursor_handle = GetCursor();

    ICONINFO cursor_info{};
    BOOL is_succeeded = GetIconInfo(cursor_handle, &cursor_info);
    if (!is_succeeded) {
        return std::nullopt;
    }

    auto result = GetCursorHeightByCursorInfo(cursor_info);

    if (cursor_info.hbmColor) {
        DeleteObject(cursor_info.hbmColor);
    }

    if (cursor_info.hbmMask) {
        DeleteObject(cursor_info.hbmMask);
    }

    return result;
}

}

void TooltipWindow::Initialize() {

    __super::Initialize();

    this->SetIsPopup(true);
    this->SetHasTitleBar(false);
    this->SetIsSizable(false);
    this->SetIsToolWindow(true);
    this->SetActivateOption(ActivateOption::NoActivate);
    this->SetInitialRectStyle(InitialRectStyle::Custom);

    label_ = zaf::Create<zaf::Label>();
    label_->SetPadding(Frame{ 5, 5, 5, 5});
    this->SetRootControl(label_);
}


void TooltipWindow::OnWindowCreated() {

    __super::OnWindowCreated();

    DWORD ex_style = GetWindowLong(this->Handle(), GWL_EXSTYLE);
    ex_style |= WS_EX_TRANSPARENT | WS_EX_LAYERED;
    SetWindowLong(this->Handle(), GWL_EXSTYLE, ex_style);

    SetLayeredWindowAttributes(this->Handle(), 0, 255, LWA_ALPHA);
}


void TooltipWindow::ShowBelowMouseCursor() {

    auto cursor_height = GetCursorHeight();
    if (!cursor_height) {
        return;
    }

    POINT cursor_position{};
    if (!GetCursorPos(&cursor_position)) {
        return;
    }

    Point window_position;
    window_position.x = static_cast<float>(cursor_position.x);
    window_position.y = static_cast<float>(cursor_position.y + *cursor_height);
    this->SetPosition(window_position);
    this->Show();
}


void TooltipWindow::SetText(const std::wstring& text) {

    label_->SetText(text);

    auto preferred_size = label_->GetPreferredSize();
    this->SetContentSize(preferred_size);
}

}