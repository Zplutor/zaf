#include <zaf/control/property_grid.h>
#include <zaf/control/internal/property_grid/property_grid_data_source.h>
#include <zaf/control/internal/property_grid/property_grid_delegate.h>
#include <zaf/control/internal/tree_control/tree_control_implementation.h>
#include <zaf/control/list_item_container.h>

namespace zaf {

PropertyGrid::PropertyGrid() : 
    implementation_(std::make_shared<internal::TreeControlImplementation>(*this)) {

}


void PropertyGrid::Initialize() {

    __super::Initialize();

    data_source_ = std::make_shared<internal::PropertyGridDataSource>(Object::Dumb());
    delegate_ = std::make_shared<internal::PropertyGridDelegate>();

    auto item_container = zaf::Create<ListItemContainer>();

    internal::TreeControlImplementation::InitializeParameters initialize_parameters;
    initialize_parameters.item_container = item_container;
    initialize_parameters.data_source = data_source_;
    initialize_parameters.delegate = delegate_;

    implementation_->Initialize(initialize_parameters);
}


void PropertyGrid::Layout(const zaf::Rect& previous_rect) {

    __super::Layout(previous_rect);

    implementation_->GetListImplementation().OnLayout();
}


void PropertyGrid::SetTargetObject(const std::shared_ptr<Object>& object) {

    data_source_ = std::make_shared<internal::PropertyGridDataSource>(object);
    implementation_->SetDataSource(data_source_);
}

}