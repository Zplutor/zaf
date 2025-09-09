#pragma once

#include <optional>
#include <zaf/base/event/event_info.h>

namespace zaf {

class ListControl;
class PopupMenu;

class ListControlSelectionChangedInfo : public EventInfo {
public:
    explicit ListControlSelectionChangedInfo(const std::shared_ptr<ListControl>& source);
};


class ListControlItemDoubleClickInfo : public EventInfo {
public:
    ListControlItemDoubleClickInfo(
        const std::shared_ptr<ListControl>& source,
        std::size_t item_index);

    std::size_t ItemIndex() const {
        return item_index_;
    }

private:
    std::size_t item_index_{};
};


class ListControlContextMenuInfo : public EventInfo {
public:
    ListControlContextMenuInfo(
        std::shared_ptr<ListControl> source,
        std::size_t item_index,
        std::shared_ptr<dynamic::Object> item_data);

    std::size_t ItemIndex() const noexcept {
        return item_index_;
    }

    const std::shared_ptr<dynamic::Object>& ItemData() const noexcept {
        return item_data_;
    }

    const std::shared_ptr<PopupMenu>& Menu() const noexcept {
        return *menu_;
    }

    void SetMenu(std::shared_ptr<PopupMenu> menu) const noexcept {
        *menu_ = std::move(menu);
    }

private:
    std::size_t item_index_{};
    std::shared_ptr<dynamic::Object> item_data_;
    std::shared_ptr<std::shared_ptr<PopupMenu>> menu_;
};

}