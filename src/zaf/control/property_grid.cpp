#include <zaf/control/property_grid.h>
#include <zaf/base/container/utility/find.h>
#include <zaf/base/container/utility/range.h>
#include <zaf/control/property_grid/internal/data_source.h>
#include <zaf/control/property_grid/internal/delegate.h>
#include <zaf/control/property_grid/internal/expanded_node_visitor.h>
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

    //TextBox cannot use cached painting.
    this->ScrollContent()->SetIsCachedPaintingEnabled(false);
}


void PropertyGrid::Layout(const zaf::Rect& previous_rect) {

    __super::Layout(previous_rect);

    split_distance_manager_->UpdateDefaultDistance(this->ScrollContent()->Width() / 2);

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


void PropertyGrid::Reload() {
    tree_implementation_->Reload();
    RefreshValues();
}


PropertyGridNode PropertyGrid::GetExpandedNodeTree() const {

    property_grid::internal::ExpandedNodeVisitor visitor;
    tree_implementation_->VisitExpandedTree(visitor);
    return visitor.TakeResult();
}


void PropertyGrid::ExpandNodeTree(const PropertyGridNode& tree) {

    auto update_gurad = this->BeginUpdate();
    ExpandChildNodes(tree, nullptr);
}


void PropertyGrid::ExpandChildNodes(
    const PropertyGridNode& parent_node, 
    const std::shared_ptr<Object>& parent_data) {

    std::map<ObjectProperty*, const PropertyGridNode*> child_node_map;
    for (const auto& each_child : parent_node.Children()) {
        child_node_map[each_child.Property()] = &each_child;
    }

    auto child_count = data_source_->GetChildDataCount(parent_data);
    for (auto index : Range(0, child_count)) {

        auto child_data = As<property_grid::internal::Data>(
            data_source_->GetChildDataAtIndex(parent_data, index));

        auto child_node = Find(child_node_map, child_data->Property());
        if (child_node) {

            tree_implementation_->ExpandItem(child_data);

            ExpandChildNodes(**child_node, child_data);
        }
    }
}

}