#include <zaf/control/property_grid.h>
#include <zaf/control/property_grid/internal/property_grid_split_distance_manager.h>
#include <zaf/control/property_grid/internal/property_grid_tree_data_source.h>
#include <zaf/control/property_grid/internal/property_grid_tree_delegate.h>
#include <zaf/control/internal/tree_control/tree_control_implementation.h>
#include <zaf/control/list_item_container.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_TYPE(PropertyGrid)
ZAF_DEFINE_TYPE_END;


PropertyGrid::PropertyGrid() : 
    split_distance_manager_(std::make_shared<internal::PropertyGridSplitDistanceManager>()),
    tree_implementation_(std::make_shared<internal::TreeControlImplementation>(*this)) {

}


void PropertyGrid::Initialize() {

    __super::Initialize();

    target_object_ = Create<Object>();
    type_config_factory_ = std::make_shared<property_grid::TypeConfigFactory>();

    tree_data_source_ = std::make_shared<internal::PropertyGridTreeDataSource>(
        target_object_,
        type_config_factory_);

    tree_delegate_ = std::make_shared<internal::PropertyGridTreeDelegate>(
        type_config_factory_,
        split_distance_manager_);

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


void PropertyGrid::SetTypeConfigFactory(
    const std::shared_ptr<property_grid::TypeConfigFactory>& factory) {

    type_config_factory_ = factory;
    ReCreateDataSource();
    ReCreateDelegate();
}


void PropertyGrid::ReCreateDataSource() {

    tree_data_source_ = std::make_shared<internal::PropertyGridTreeDataSource>(
        target_object_,
        type_config_factory_);

    tree_implementation_->SetDataSource(tree_data_source_);
}


void PropertyGrid::ReCreateDelegate() {

    tree_delegate_ = std::make_shared<internal::PropertyGridTreeDelegate>(
        type_config_factory_, 
        split_distance_manager_);

    tree_implementation_->SetDelegate(tree_delegate_);
}


void PropertyGrid::Reload() {

    tree_implementation_->Reload();
}

}