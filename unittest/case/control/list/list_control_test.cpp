#include <gtest/gtest.h>
#include <zaf/base/as.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/base/none.h>
#include <zaf/control/list_control.h>
#include <zaf/control/list_control_delegate.h>
#include <zaf/control/list_data_source.h>

using namespace zaf;

TEST(ListControlTest, DataSource) {

    auto list = Create<ListControl>();

    //Data source won't be null.
    ASSERT_NE(list->DataSource(), nullptr);

    //Not allow to set a null data source.
    ASSERT_THROW(list->SetDataSource(nullptr), PreconditionError);

    //Can still get the data source after setting a temporary instance.
    list->SetDataSource(Create<ListDataSource>());
    ASSERT_NE(list->DataSource(), nullptr);
}


TEST(ListControlTest, Delegate) {

    auto list = Create<ListControl>();

    //Delegate won't be null.
    ASSERT_NE(list->Delegate(), nullptr);

    //Not allow to set a null delegate.
    ASSERT_THROW(list->SetDelegate(nullptr), PreconditionError);

    //Can still get the delegate after setting a temporary instance.
    list->SetDelegate(Create<ListControlDelegate>());
    ASSERT_NE(list->Delegate(), nullptr);
}


TEST(ListControlTest, SelfReference) {

    class TestListControl : public ListControl, public ListDataSource, public ListControlDelegate {
    public:
        std::shared_ptr<None> life = std::make_shared<None>();

    protected:
        void Initialize() override {
            __super::Initialize();
            this->SetDataSource(As<ListDataSource>(shared_from_this()));
            this->SetDelegate(As<ListControlDelegate>(shared_from_this()));
        }
    };

    auto list = Create<TestListControl>();
    ASSERT_EQ(list->DataSource(), list);
    ASSERT_EQ(list->Delegate(), list);

    std::weak_ptr<None> life = list->life;
    list.reset();
    ASSERT_TRUE(life.expired());
}