#include <zaf/control/property_grid.h>
#include <zaf/base/container/utility/find.h>
#include <zaf/base/range.h>
#include <zaf/internal/property_grid/property_grid_data_source.h>
#include <zaf/internal/property_grid/property_grid_tree_delegate.h>
#include <zaf/internal/property_grid/expanded_node_visitor.h>
#include <zaf/internal/property_grid/split_distance_manager.h>
#include <zaf/control/internal/tree_control/tree_control_implementation.h>
#include <zaf/control/tree_item_container.h>

namespace zaf {

ZAF_OBJECT_IMPL(PropertyGrid)


PropertyGrid::PropertyGrid() : 
    split_distance_manager_(std::make_shared<internal::SplitDistanceManager>()),
    tree_implementation_(std::make_shared<internal::TreeControlImplementation>(*this)) {

}


void PropertyGrid::Initialize() {

    __super::Initialize();

    target_object_ = Create<Object>();
    delegate_.Assign(std::make_shared<PropertyGridDelegate>(), this);

    data_source_ = std::make_shared<internal::PropertyGridDataSource>(delegate_.ToSharedPtr());
    data_source_->SetTargetObject(target_object_);

    tree_delegate_ = std::make_shared<internal::PropertyGridTreeDelegate>(
        delegate_.ToSharedPtr(),
        split_distance_manager_,
        tree_implementation_);

    internal::TreeControlImplementation::InitializeParameters initialize_parameters;
    initialize_parameters.item_container = Create<TreeItemContainer>();
    initialize_parameters.data_source = data_source_;
    initialize_parameters.delegate = tree_delegate_;

    tree_implementation_->Initialize(initialize_parameters);

    //RichEdit cannot use cached painting.
    this->ScrollContent()->SetIsCachedPaintingEnabled(false);
}


void PropertyGrid::Layout(const zaf::Rect& previous_rect) {

    __super::Layout(previous_rect);

    split_distance_manager_->UpdateDefaultDistance(this->ScrollContent()->Width() / 2);

    tree_implementation_->ListParts().Core().OnLayout();
}


void PropertyGrid::OnMouseDown(const MouseDownInfo& event_info) {
    __super::OnMouseDown(event_info);
    tree_implementation_->ListParts().InputHandler().HandleMouseDownEvent(event_info);
}


void PropertyGrid::OnMouseMove(const MouseMoveInfo& event_info) {
    __super::OnMouseMove(event_info);
    tree_implementation_->ListParts().InputHandler().HandleMouseMoveEvent(event_info);
}


void PropertyGrid::OnMouseUp(const MouseUpInfo& event_info) {
    __super::OnMouseUp(event_info);
    tree_implementation_->ListParts().InputHandler().HandleMouseUpEvent(event_info);
}


void PropertyGrid::OnKeyDown(const KeyDownInfo& event_info) {
    __super::OnKeyDown(event_info);
    tree_implementation_->ListParts().InputHandler().HandleKeyDownEvent(event_info);
}


void PropertyGrid::OnFocusGained(const FocusGainedInfo& event_info) {
    __super::OnFocusGained(event_info);
    tree_implementation_->ListParts().Core().HandleFocusGainedEvent(event_info);
}


void PropertyGrid::OnFocusLost(const FocusLostInfo& event_info) {
    __super::OnFocusLost(event_info);
    tree_implementation_->ListParts().Core().HandleFocusLostEvent(event_info);
}


void PropertyGrid::SetTargetObject(std::shared_ptr<Object> object) {
    ZAF_EXPECT(object);
    target_object_ = std::move(object);
    ReCreateDataSource();
}


void PropertyGrid::SetDelegate(std::shared_ptr<PropertyGridDelegate> delegate) {

    ZAF_EXPECT(delegate);

    delegate_.Assign(std::move(delegate), this);
    ReCreateDataSource();
    ReCreateDelegate();
}


void PropertyGrid::ReCreateDataSource() {

    data_source_ = std::make_shared<internal::PropertyGridDataSource>(delegate_.ToSharedPtr());
    data_source_->SetTargetObject(target_object_);

    tree_implementation_->SetDataSource(data_source_);
}


void PropertyGrid::ReCreateDelegate() {

    tree_delegate_ = std::make_shared<internal::PropertyGridTreeDelegate>(
        delegate_.ToSharedPtr(),
        split_distance_manager_,
        tree_implementation_);

    tree_implementation_->SetDelegate(tree_delegate_);
}


void PropertyGrid::RefreshValues() {
    data_source_->RefreshValues();
}


void PropertyGrid::Reload() {
    tree_implementation_->Reload();
    RefreshValues();
}


PropertyGridNode PropertyGrid::GetExpandedNodeTree() const {

    internal::ExpandedNodeVisitor visitor;
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

        auto child_data = As<internal::PropertyData>(
            data_source_->GetChildDataAtIndex(parent_data, index));

        auto child_node = Find(child_node_map, child_data->Property());
        if (child_node) {

            tree_implementation_->ExpandItem(child_data);

            ExpandChildNodes(**child_node, child_data);
        }
    }
}

}