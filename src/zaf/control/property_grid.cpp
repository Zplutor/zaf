#include <zaf/control/property_grid.h>
#include <zaf/control/internal/property_grid/property_grid_tree_data_source.h>
#include <zaf/control/internal/property_grid/property_grid_tree_delegate.h>
#include <zaf/control/internal/tree_control/tree_control_implementation.h>
#include <zaf/control/list_item_container.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_TYPE(PropertyGrid)
ZAF_DEFINE_TYPE_END;


PropertyGrid::PropertyGrid() : 
    tree_implementation_(std::make_shared<internal::TreeControlImplementation>(*this)) {

}


void PropertyGrid::Initialize() {

    __super::Initialize();

    target_object_ = Create<Object>();
    filter_ = std::make_shared<PropertyGridFilter>();

    tree_data_source_ = std::make_shared<internal::PropertyGridTreeDataSource>(
        target_object_,
        filter_);

    tree_delegate_ = std::make_shared<internal::PropertyGridTreeDelegate>();

    internal::TreeControlImplementation::InitializeParameters initialize_parameters;
    initialize_parameters.item_container = Create<ListItemContainer>();
    initialize_parameters.data_source = tree_data_source_;
    initialize_parameters.delegate = tree_delegate_;

    tree_implementation_->Initialize(initialize_parameters);
}


void PropertyGrid::Layout(const zaf::Rect& previous_rect) {

    __super::Layout(previous_rect);

    tree_implementation_->GetListImplementation().OnLayout();
}


void PropertyGrid::SetTargetObject(const std::shared_ptr<Object>& object) {
    target_object_ = object;
    ReCreateDataSource();
}


void PropertyGrid::SetFilter(const std::shared_ptr<PropertyGridFilter>& filter) {
    filter_ = filter;
    ReCreateDataSource();
}


void PropertyGrid::ReCreateDataSource() {

    tree_data_source_ = std::make_shared<internal::PropertyGridTreeDataSource>(
        target_object_,
        filter_);

    tree_implementation_->SetDataSource(tree_data_source_);
}


void PropertyGrid::Reload() {

    tree_implementation_->Reload();
}

}