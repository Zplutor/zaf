#pragma once

#include <zaf/control/control.h>
#include <zaf/control/tree_data_source.h>
#include <zaf/window/window.h>

namespace zaf::internal {

class InspectDataSource : public TreeDataSource {
public:
    InspectDataSource(const std::shared_ptr<Window>& window);

    void ControlAddChild(const std::shared_ptr<Control>& parent, std::size_t added_index);
    void ControlRemoveChild(const std::shared_ptr<Control>& parent, std::size_t removed_index);

    bool DoesDataHasChildren(const std::shared_ptr<dynamic::Object>& data) override;

    std::size_t GetChildDataCount(const std::shared_ptr<dynamic::Object>& parent_data) override;

    std::shared_ptr<dynamic::Object> GetChildDataAtIndex(
        const std::shared_ptr<dynamic::Object>&parent_data,
        std::size_t index) override;

private:
    std::shared_ptr<Window> window_;
};

}