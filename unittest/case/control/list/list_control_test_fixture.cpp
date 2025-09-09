#include "case/control/list/list_control_test_fixture.h"
#include <zaf/control/linear_box.h>
#include <zaf/control/list_control_delegate.h>

using namespace zaf;

namespace zaf::test {
namespace {

class TestDelegate : public ListControlDelegate {
public:
    float EstimateItemHeight(
        std::size_t item_index,
        const std::shared_ptr<dynamic::Object>& item_data) override {

        return 10;
    }
};

}

ListControlTestFixture::ListControlTestFixture() {

    InitializeWindow();
    InitializeListControl();
}


ListControlTestFixture::~ListControlTestFixture() {

    window_->Destroy();
}


void ListControlTestFixture::InitializeWindow() {

    window_ = Create<zaf::Window>();
    window_->SetInitialRectStyle(zaf::InitialRectStyle::Custom);
    window_->SetRect(zaf::Rect{ 0, 0, 100, 200 });
    window_->Show();
}


void ListControlTestFixture::InitializeListControl() {

    list_control_ = Create<zaf::ListControl>();
    list_control_->SetBorder({});
    list_control_->SetPadding({});
    list_control_->SetFixedHeight(200);

    data_source_ = Create<TestListDataSource>();
    list_control_->SetDataSource(data_source_);

    delegate_ = Create<TestDelegate>();
    list_control_->SetDelegate(delegate_);

    auto vertical_box = Create<VerticalBox>();
    vertical_box->AddChild(list_control_);

    window_->SetRootControl(vertical_box);
}


TestListDataSource::TestListDataSource() {

    for (auto index : Range(0, 10)) {
        data_list_.push_back(Box(static_cast<int>(index)));
    }
}


std::size_t TestListDataSource::GetDataCount() const {
    return data_list_.size();
}


std::shared_ptr<zaf::dynamic::Object> TestListDataSource::GetDataAtIndex(std::size_t index) const {
    return data_list_[index];
}


void TestListDataSource::AddAt(int value, std::size_t index) {
    data_list_.insert(std::next(data_list_.begin(), index), Box(value));
    NotifyDataAdded(index, 1);
}


void TestListDataSource::RemoveAt(std::size_t index) {
    data_list_.erase(std::next(data_list_.begin(), index));
    NotifyDataRemoved(index, 1);
}


void test::TestListDataSource::Move(std::size_t from, std::size_t to) {
    auto data = data_list_[from];
    data_list_.erase(std::next(data_list_.begin(), from));
    data_list_.insert(std::next(data_list_.begin(), to), data);
    NotifyDataMoved(from, to);
}

}