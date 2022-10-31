#include <gtest/gtest.h>
#include <zaf/control/control.h>
#include <zaf/creation.h>
#include <zaf/control/layout/linear_layouter.h>

namespace {

class LinearLayouterAxisAlignmentTest : public testing::Test {
protected:
    void RunTest(
        zaf::AxisAlignment alignment,
        float parent_length,
        float child_length,
        const std::vector<float>& expected_children_positions) {

        RunTestWithMargin(
            alignment, 
            parent_length, 
            child_length, 
            0,
            0, 
            expected_children_positions);
    }

    void RunTestWithMargin(
        zaf::AxisAlignment alignment,
        float parent_length,
        float child_length,
        float child_heading_margin,
        float child_tailing_margin,
        const std::vector<float>& expected_children_positions) {

        for (auto test_height : { true, false }) {
            InnerRunTest(
                alignment,
                parent_length,
                child_length,
                child_heading_margin,
                child_tailing_margin,
                expected_children_positions,
                test_height);
        }
    }

private:
    void InnerRunTest(
        zaf::AxisAlignment alignment,
        float parent_length,
        float child_length,
        float child_heading_margin,
        float child_tailing_margin,
        const std::vector<float>& expected_children_positions,
        bool test_height) {

        auto layouter = zaf::Create<zaf::LinearLayouter>();
        layouter->SetAxisAlignment(alignment);
        layouter->SetDirection(
            test_height ? zaf::LayoutDirection::TopToBottom : zaf::LayoutDirection::LeftToRight);

        auto parent = zaf::Create<zaf::Control>();
        if (test_height) {
            parent->SetHeight(parent_length);
        }
        else {
            parent->SetWidth(parent_length);
        }

        std::vector<std::shared_ptr<zaf::Control>> children;
        for (std::size_t index = 0; index < expected_children_positions.size(); ++index) {
            auto child = zaf::Create<zaf::Control>();
            if (test_height) {
                child->SetMaxHeight(child_length);
                child->SetMinHeight(child_length);
                child->SetMargin(zaf::Frame(0, child_heading_margin, 0, child_tailing_margin));
            }
            else {
                child->SetMaxWidth(child_length);
                child->SetMinWidth(child_length);
                child->SetMargin(zaf::Frame(child_heading_margin, 0, child_tailing_margin, 0));
            }
            children.push_back(child);
        }

        layouter->Layout(*parent, {}, children);

        for (std::size_t index = 0; index < children.size(); ++index) {
            if (test_height) {
                ASSERT_EQ(children[index]->Y(), expected_children_positions[index]);
            }
            else {
                ASSERT_EQ(children[index]->X(), expected_children_positions[index]);
            }
        }
    }
};


class LinearLayouterCrossAxisAlignmentTest : public testing::Test {
protected:
    void RunTest(
        zaf::AxisAlignment alignment, 
        float parent_length,
        float fixed_length,
        float expected_position) {

        RunTestWithMargin(alignment, parent_length, fixed_length, 0, 0, expected_position);
    }

    void RunTestWithMargin(
        zaf::AxisAlignment alignment,
        float parent_length,
        float fixed_length,
        float heading_margin,
        float tailing_margin,
        float expected_position) {

        for (auto test_height : { true,false }) {

            InnerRunTest(
                alignment, 
                parent_length, 
                fixed_length, 
                heading_margin, 
                tailing_margin, 
                expected_position, 
                test_height);
        }
    }

private:
    void InnerRunTest(
        zaf::AxisAlignment alignment,
        float parent_length,
        float fixed_length,
        float heading_margin,
        float tailing_margin,
        float expected_position,
        bool test_height) {

        auto layouter = zaf::Create<zaf::LinearLayouter>();
        layouter->SetCrossAxisAlignment(alignment);

        auto parent = zaf::Create<zaf::Control>();

        std::vector<std::shared_ptr<zaf::Control>> children;
        auto child = zaf::Create<zaf::Control>();
        children.push_back(child);

        if (test_height) {
            layouter->SetDirection(zaf::LayoutDirection::LeftToRight);
            parent->SetHeight(parent_length);
            child->SetMaxHeight(fixed_length);
            child->SetMinHeight(fixed_length);
            child->SetMargin(zaf::Frame(0, heading_margin, 0, tailing_margin));
        }
        else {
            layouter->SetDirection(zaf::LayoutDirection::TopToBottom);
            parent->SetWidth(parent_length);
            child->SetMaxWidth(fixed_length);
            child->SetMinWidth(fixed_length);
            child->SetMargin(zaf::Frame(heading_margin, 0, tailing_margin, 0));
        }

        layouter->Layout(*parent, {}, children);

        if (test_height) {
            ASSERT_EQ(child->Y(), expected_position);
        }
        else {
            ASSERT_EQ(child->X(), expected_position);
        }
    }
};

}

TEST_F(LinearLayouterAxisAlignmentTest, Start) {

    RunTest(zaf::AxisAlignment::Start, 100, 10, { 0 });
    RunTest(zaf::AxisAlignment::Start, 100, 200, { 0 });
    RunTest(zaf::AxisAlignment::Start, 100, 10, { 0, 10 });
    RunTest(zaf::AxisAlignment::Start, 100, 50, { 0, 50 });
    RunTest(zaf::AxisAlignment::Start, 100, 40, { 0, 40, 80, 120 });

    RunTestWithMargin(zaf::AxisAlignment::Start, 100, 10, 5, 10, { 5 });
    RunTestWithMargin(zaf::AxisAlignment::Start, 100, 10, 5, 10, { 5, 30 });
}


TEST_F(LinearLayouterAxisAlignmentTest, End) {

    RunTest(zaf::AxisAlignment::End, 100, 10, { 90 });
    RunTest(zaf::AxisAlignment::End, 100, 200, { -100 });
    RunTest(zaf::AxisAlignment::End, 100, 10, { 70, 80, 90 });
    RunTest(zaf::AxisAlignment::End, 100, 50, { 0, 50 });
    RunTest(zaf::AxisAlignment::End, 100, 40, { -60, -20, 20, 60 });

    RunTestWithMargin(zaf::AxisAlignment::End, 100, 10, 5, 10, { 80 });
    RunTestWithMargin(zaf::AxisAlignment::End, 100, 10, 5, 10, { 55, 80 });
}


TEST_F(LinearLayouterAxisAlignmentTest, Center) {

    RunTest(zaf::AxisAlignment::Center, 100, 10, { 45 });
    RunTest(zaf::AxisAlignment::Center, 100, 200, { -50 });
    RunTest(zaf::AxisAlignment::Center, 100, 10, { 40, 50 });
    RunTest(zaf::AxisAlignment::Center, 100, 50, { 0, 50 });
    RunTest(zaf::AxisAlignment::Center, 100, 40, { -30, 10, 50, 90 });

    RunTestWithMargin(zaf::AxisAlignment::Center, 100, 10, 5, 10, { 42.5 });
    RunTestWithMargin(zaf::AxisAlignment::Center, 100, 10, 5, 10, { 30, 55 });
}


TEST_F(LinearLayouterCrossAxisAlignmentTest, Start) {

    RunTest(zaf::AxisAlignment::Start, 50, 10, 0);
    RunTest(zaf::AxisAlignment::Start, 50, 50, 0);
    RunTest(zaf::AxisAlignment::Start, 50, 90, 0);

    RunTestWithMargin(zaf::AxisAlignment::Start, 50, 10, 5, 10, 5);
}


TEST_F(LinearLayouterCrossAxisAlignmentTest, End) {

    RunTest(zaf::AxisAlignment::End, 50, 10, 40);
    RunTest(zaf::AxisAlignment::End, 50, 50, 0);
    RunTest(zaf::AxisAlignment::End, 50, 90, -40);

    RunTestWithMargin(zaf::AxisAlignment::End, 50, 10, 5, 10, 30);
}


TEST_F(LinearLayouterCrossAxisAlignmentTest, Center) {

    RunTest(zaf::AxisAlignment::Center, 50, 10, 20);
    RunTest(zaf::AxisAlignment::Center, 50, 50, 0);
    RunTest(zaf::AxisAlignment::Center, 50, 90, -20);

    RunTestWithMargin(zaf::AxisAlignment::Center, 50, 10, 5, 10, 17.5);
}