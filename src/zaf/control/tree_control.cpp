#include <zaf/control/tree_control.h>
#include <zaf/base/error/check.h>
#include <zaf/control/tree_control_delegate.h>
#include <zaf/control/tree_data_source.h>
#include <zaf/control/internal/tree_control/tree_control_implementation.h>

namespace zaf {

TreeControl::TreeControl() :
    data_source_(this),
    delegate_(this),
    implementation_(std::make_shared<internal::TreeControlImplementation>(*this)) {

}


TreeControl::~TreeControl() {

}


void TreeControl::Initialize() {

    __super::Initialize();

    item_container_ = Create<ListItemContainer>();
    data_source_ = Create<TreeDataSource>();
    delegate_ = Create<TreeControlDelegate>();

    internal::TreeControlImplementation::InitializeParameters initialize_parameters;
    initialize_parameters.item_container = item_container_;
    initialize_parameters.data_source = data_source_.GetSharedPointer();
    initialize_parameters.delegate = delegate_.GetSharedPointer();

    /*
    initialize_parameters.data_source_change_event =
        std::bind(&ListControl::DataSourceChange, this, std::placeholders::_1);
    initialize_parameters.delegate_change_event =
        std::bind(&ListControl::DelegateChange, this, std::placeholders::_1);
    initialize_parameters.item_container_change_event =
        std::bind(&ListControl::ItemContainerChange, this, std::placeholders::_1);
    initialize_parameters.selection_change_event = std::bind(&ListControl::SelectionChange, this);
    */

    implementation_->Initialize(initialize_parameters);
}


void TreeControl::SetDataSource(const std::shared_ptr<TreeDataSource>& data_source) {

    ZAF_EXPECT(data_source);

    data_source_ = data_source;
    implementation_->SetDataSource(data_source_.GetSharedPointer());
}


void TreeControl::SetDelegate(const std::shared_ptr<TreeControlDelegate>& delegate) {

    ZAF_EXPECT(delegate);

    delegate_ = delegate;
    implementation_->SetDelegate(delegate_.GetSharedPointer());
}


void TreeControl::SetSelectionMode(SelectionMode selection_mode) {
    implementation_->GetListImplementation().SetSelectionMode(selection_mode);
}

}