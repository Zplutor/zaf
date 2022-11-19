#include <zaf/control/property_grid.h>
#include <zaf/control/property_grid/internal/data_source.h>
#include <zaf/control/property_grid/internal/delegate.h>
#include <zaf/control/property_grid/internal/split_distance_manager.h>
#include <zaf/control/internal/tree_control/tree_control_implementation.h>
#include <zaf/control/list_item_container.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_TYPE(PropertyGrid)
ZAF_DEFINE_TYPE_END;


PropertyGrid::PropertyGrid() : 
    split_distance_manager_(std::make_shared<property_grid::internal::SplitDistanceManager>()),
    tree_implementation_(std::make_shared<internal::TreeControlImplementation>(*this)) {

}


void PropertyGrid::Initialize() {

    __super::Initialize();

    target_object_ = Create<Object>();
    type_config_factory_ = std::make_shared<property_grid::TypeConfigFactory>();

    data_source_ = std::make_shared<property_grid::internal::DataSource>(type_config_factory_);
    data_source_->SetTargetObject(target_object_);

    delegate_ = std::make_shared<property_grid::internal::Delegate>(
        type_config_factory_,
        split_distance_manager_);

    internal::TreeControlImplementation::InitializeParameters initialize_parameters;
    initialize_parameters.item_container = Create<ListItemContainer>();
    initialize_parameters.data_source = data_source_;
    initialize_parameters.delegate = delegate_;

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

    data_source_ = std::make_shared<property_grid::internal::DataSource>(type_config_factory_);
    data_source_->SetTargetObject(target_object_);

    tree_implementation_->SetDataSource(data_source_);
}


void PropertyGrid::ReCreateDelegate() {

    delegate_ = std::make_shared<property_grid::internal::Delegate>(
        type_config_factory_, 
        split_distance_manager_);

    tree_implementation_->SetDelegate(delegate_);
}


void PropertyGrid::RefreshValues() {

    data_source_->RefreshValues();
}

}