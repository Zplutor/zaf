#include <gtest/gtest.h>
#include <zaf/control/control.h>
#include <zaf/creation.h>
#include <zaf/control/layout/linear_layouter.h>

namespace {

class LinearLayouterControlAlignmentTest : public testing::Test {
protected:
    void RunTest(
        zaf::ControlAlignment alignment,
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
        zaf::ControlAlignment alignment,
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
        zaf::ControlAlignment alignment,
        float parent_length,
        float child_length,
        float child_heading_margin,
        float child_tailing_margin,
        const std::vector<float>& expected_children_positions,
        bool test_height) {

        auto layouter = zaf::Create<zaf::LinearLayouter>();
        layouter->SetControlAlignment(alignment);
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
                child->SetMaximumHeight(child_length);
                child->SetMinimumHeight(child_length);
                child->SetMargin(zaf::Frame(0, child_heading_margin, 0, child_tailing_margin));
            }
            else {
                child->SetMaximumWidth(child_length);
                child->SetMinimumWidth(child_length);
                child->SetMargin(zaf::Frame(child_heading_margin, 0, child_tailing_margin, 0));
            }
            children.push_back(child);
        }

        layouter->Layout(*parent, {}, children);

        for (std::size_t index = 0; index < children.size(); ++index) {
            if (test_height) {
                ASSERT_EQ(children[index]->GetY(), expected_children_positions[index]);
            }
            else {
                ASSERT_EQ(children[index]->GetX(), expected_children_positions[index]);
            }
        }
    }
};


class LinearLayouterAxisAlignmentTest : public testing::Test {
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
        layouter->SetAxisAlignment(alignment);

        auto parent = zaf::Create<zaf::Control>();

        std::vector<std::shared_ptr<zaf::Control>> children;
        auto child = zaf::Create<zaf::Control>();
        children.push_back(child);

        if (test_height) {
            layouter->SetDirection(zaf::LayoutDirection::LeftToRight);
            parent->SetHeight(parent_length);
            child->SetMaximumHeight(fixed_length);
            child->SetMinimumHeight(fixed_length);
            child->SetMargin(zaf::Frame(0, heading_margin, 0, tailing_margin));
        }
        else {
            layouter->SetDirection(zaf::LayoutDirection::TopToBottom);
            parent->SetWidth(parent_length);
            child->SetMaximumWidth(fixed_length);
            child->SetMinimumWidth(fixed_length);
            child->SetMargin(zaf::Frame(heading_margin, 0, tailing_margin, 0));
        }

        layouter->Layout(*parent, {}, children);

        if (test_height) {
            ASSERT_EQ(child->GetY(), expected_position);
        }
        else {
            ASSERT_EQ(child->GetX(), expected_position);
        }
    }
};

}

TEST_F(LinearLayouterControlAlignmentTest, LeadingControlAlignment) {

    RunTest(zaf::ControlAlignment::Leading, 100, 10, { 0 });
    RunTest(zaf::ControlAlignment::Leading, 100, 200, { 0 });
    RunTest(zaf::ControlAlignment::Leading, 100, 10, { 0, 10 });
    RunTest(zaf::ControlAlignment::Leading, 100, 50, { 0, 50 });
    RunTest(zaf::ControlAlignment::Leading, 100, 40, { 0, 40, 80, 120 });

    RunTestWithMargin(zaf::ControlAlignment::Leading, 100, 10, 5, 10, { 5 });
    RunTestWithMargin(zaf::ControlAlignment::Leading, 100, 10, 5, 10, { 5, 30 });
}


TEST_F(LinearLayouterControlAlignmentTest, TailingControlAlignment) {

    RunTest(zaf::ControlAlignment::Tailing, 100, 10, { 90 });
    RunTest(zaf::ControlAlignment::Tailing, 100, 200, { -100 });
    RunTest(zaf::ControlAlignment::Tailing, 100, 10, { 70, 80, 90 });
    RunTest(zaf::ControlAlignment::Tailing, 100, 50, { 0, 50 });
    RunTest(zaf::ControlAlignment::Tailing, 100, 40, { -60, -20, 20, 60 });

    RunTestWithMargin(zaf::ControlAlignment::Tailing, 100, 10, 5, 10, { 80 });
    RunTestWithMargin(zaf::ControlAlignment::Tailing, 100, 10, 5, 10, { 55, 80 });
}


TEST_F(LinearLayouterControlAlignmentTest, CenterControlAlignment) {

    RunTest(zaf::ControlAlignment::Center, 100, 10, { 45 });
    RunTest(zaf::ControlAlignment::Center, 100, 200, { -50 });
    RunTest(zaf::ControlAlignment::Center, 100, 10, { 40, 50 });
    RunTest(zaf::ControlAlignment::Center, 100, 50, { 0, 50 });
    RunTest(zaf::ControlAlignment::Center, 100, 40, { -30, 10, 50, 90 });

    RunTestWithMargin(zaf::ControlAlignment::Center, 100, 10, 5, 10, { 42.5 });
    RunTestWithMargin(zaf::ControlAlignment::Center, 100, 10, 5, 10, { 30, 55 });
}


TEST_F(LinearLayouterAxisAlignmentTest, NearAxisAlignment) {

    RunTest(zaf::AxisAlignment::Near, 50, 10, 0);
    RunTest(zaf::AxisAlignment::Near, 50, 50, 0);
    RunTest(zaf::AxisAlignment::Near, 50, 90, 0);

    RunTestWithMargin(zaf::AxisAlignment::Near, 50, 10, 5, 10, 5);
}


TEST_F(LinearLayouterAxisAlignmentTest, FarAxisAlignment) {

    RunTest(zaf::AxisAlignment::Far, 50, 10, 40);
    RunTest(zaf::AxisAlignment::Far, 50, 50, 0);
    RunTest(zaf::AxisAlignment::Far, 50, 90, -40);

    RunTestWithMargin(zaf::AxisAlignment::Far, 50, 10, 5, 10, 30);
}


TEST_F(LinearLayouterAxisAlignmentTest, CenterAxisAlignment) {

    RunTest(zaf::AxisAlignment::Center, 50, 10, 20);
    RunTest(zaf::AxisAlignment::Center, 50, 50, 0);
    RunTest(zaf::AxisAlignment::Center, 50, 90, -20);

    RunTestWithMargin(zaf::AxisAlignment::Center, 50, 10, 5, 10, 17.5);
}