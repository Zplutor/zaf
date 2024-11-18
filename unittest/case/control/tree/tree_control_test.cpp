#include <gtest/gtest.h>
#include <zaf/base/none.h>
#include <zaf/control/tree_control.h>
#include <zaf/control/tree_control_delegate.h>
#include <zaf/control/tree_data_source.h>

using namespace zaf;

TEST(TreeControlTest, DataSource) {

    auto tree = Create<TreeControl>();

    //Data source won't be null.
    ASSERT_NE(tree->DataSource(), nullptr);

    //Not allow to set a null data source.
    ASSERT_THROW(tree->SetDataSource(nullptr), PreconditionError);

    //Can still get the data source after setting a temporary instance.
    tree->SetDataSource(Create<TreeDataSource>());
    ASSERT_NE(tree->DataSource(), nullptr);
}


TEST(TreeControlTest, Delegate) {

    auto tree = Create<TreeControl>();

    //Delegate won't be null.
    ASSERT_NE(tree->Delegate(), nullptr);

    //Not allow to set a null delegate.
    ASSERT_THROW(tree->SetDelegate(nullptr), PreconditionError);

    //Can still get the delegate after setting a temporary instance.
    tree->SetDelegate(Create<TreeControlDelegate>());
    ASSERT_NE(tree->Delegate(), nullptr);
}


TEST(TreeControlTest, SelfReference) {

    class TestTreeControl : public TreeControl, public TreeDataSource, public TreeControlDelegate {
    public:
        std::shared_ptr<None> life = std::make_shared<None>();

    protected:
        void Initialize() override {
            __super::Initialize();
            this->SetDataSource(As<TreeDataSource>(shared_from_this()));
            this->SetDelegate(As<TreeControlDelegate>(shared_from_this()));
        }
    };

    auto list = Create<TestTreeControl>();
    ASSERT_EQ(list->DataSource(), list);
    ASSERT_EQ(list->Delegate(), list);

    std::weak_ptr<None> life = list->life;
    list.reset();
    ASSERT_TRUE(life.expired());
}