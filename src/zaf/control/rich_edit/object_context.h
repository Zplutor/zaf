#pragma once

namespace zaf::rich_edit {

class ObjectContext {
public:
    ObjectContext(std::size_t position, bool is_selected, bool is_in_selection_range) :
        position_(position),
        is_selected_(is_selected),
        is_in_selection_range_(is_in_selection_range) {

    }

    std::size_t Position() const {
        return position_;
    }

    bool IsSelected() const {
        return is_selected_;
    }

    bool IsInSelectionRange() const {
        return is_in_selection_range_;
    }

private:
    std::size_t position_{};
    bool is_selected_{};
    bool is_in_selection_range_{};
};

}