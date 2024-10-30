#include <gtest/gtest.h>
#include <zaf/control/list_control.h>
#include <zaf/control/list_item_container.h>
#include <zaf/creation.h>
#include "case/control/list/list_control_test_fixture.h"

using namespace zaf;
using namespace zaf::test;

TEST(ListControlTest, DefaultFocus) {

    auto list = Create<ListControl>();
    ASSERT_TRUE(list->CanFocused());
    ASSERT_FALSE(list->ItemContainer()->CanFocused());
}


/*
Focus will be set to items when left-clicking on items.
*/
TEST(ListControlTest, Focus_LeftClickOnItems) {

    ListControlTestFixture fixture;

    fixture.Window().Messager().SendWMLBUTTONDOWN(Point{10, 35});
    ASSERT_EQ(fixture.ListControl().GetVisibleItemAtIndex(3)->IsFocused(), true);

    fixture.Window().Messager().SendWMMOUSEMOVE(Point{ 10, 15 });
    ASSERT_EQ(fixture.ListControl().GetVisibleItemAtIndex(1)->IsFocused(), true);

    fixture.Window().Messager().SendWMMOUSEMOVE(Point{ 10, 45 });
    ASSERT_EQ(fixture.ListControl().GetVisibleItemAtIndex(4)->IsFocused(), true);
}


/*
Focus will be set to list control when left-clicking on blank area.
*/
TEST(ListControlTest, Focus_LeftClickOnBlank) {

    ListControlTestFixture fixture;

    fixture.Window().Messager().SendWMLBUTTONDOWN(Point{10, 105});
    ASSERT_EQ(fixture.ListControl().IsFocused(), true);
}



/*
Focus will be set to list control when right-clicking anywhere.
*/
TEST(ListControlTest, Focus_RightClick) {

    ListControlTestFixture fixture;

    //Righ-click on item.
    fixture.Window().Messager().SendWMRBUTTONDOWN(Point{10, 10});
    ASSERT_EQ(fixture.ListControl().IsFocused(), true);

    fixture.ListControl().SetIsFocused(false);

    //Rich-click on blank area.
    fixture.Window().Messager().SendWMRBUTTONDOWN(Point{ 10, 105 });
    ASSERT_EQ(fixture.ListControl().IsFocused(), true);
}


/**
Focus will be set to items when pressing specific keys.
*/
TEST(ListControlTest, Focus_KeyDown) {

    ListControlTestFixture fixture;
    fixture.ListControl().SetIsFocused(true);

    fixture.Window().Messager().SendWMKEYDOWN(Key::Down);
    ASSERT_TRUE(fixture.ListControl().GetVisibleItemAtIndex(0)->IsFocused());

    fixture.Window().Messager().SendWMKEYDOWN(Key::End);
    ASSERT_TRUE(fixture.ListControl().GetVisibleItemAtIndex(9)->IsFocused());

    fixture.Window().Messager().SendWMKEYDOWN(Key::Up);
    ASSERT_TRUE(fixture.ListControl().GetVisibleItemAtIndex(8)->IsFocused());

    fixture.Window().Messager().SendWMKEYDOWN(Key::Home);
    ASSERT_TRUE(fixture.ListControl().GetVisibleItemAtIndex(0)->IsFocused());
}


/*
Focus will be adjusted to items after data being added.
*/
TEST(ListControlTest, Focus_DataAdded) {

    ListControlTestFixture fixture;

    //Set focus to one of the items.
    fixture.Window().Messager().SendWMLBUTTONDOWN(Point{ 10, 5 });
    ASSERT_TRUE(fixture.ListControl().GetVisibleItemAtIndex(0)->IsFocused());

    //Add item before the focused index, the focused index should be moved to next index.
    fixture.DataSource().AddAt(100, 0);
    ASSERT_TRUE(fixture.ListControl().GetVisibleItemAtIndex(1)->IsFocused());

    //Add item after the focused index, the focused index shouldn't be changed.
    fixture.DataSource().AddAt(101, 2);
    ASSERT_TRUE(fixture.ListControl().GetVisibleItemAtIndex(1)->IsFocused());
}


/*
Focus will be adjusted to items after data being removing.
*/
TEST(ListControlTest, Focus_DataRemoved) {

    ListControlTestFixture fixture;

    //Set focus to one of the items.
    fixture.Window().Messager().SendWMLBUTTONDOWN(Point{ 10, 25 });
    ASSERT_TRUE(fixture.ListControl().GetVisibleItemAtIndex(2)->IsFocused());

    //Remove item before the focused index, the focused index should be moved to previous index.
    fixture.DataSource().RemoveAt(0);
    ASSERT_TRUE(fixture.ListControl().GetVisibleItemAtIndex(1)->IsFocused());

    //Remove item after the focused index, the focused index shouldn't be changed.
    fixture.DataSource().RemoveAt(5);
    ASSERT_TRUE(fixture.ListControl().GetVisibleItemAtIndex(1)->IsFocused());

    //Remove the focused index, the focus should be changed to the list control.
    fixture.DataSource().RemoveAt(1);
    ASSERT_TRUE(fixture.ListControl().IsFocused());
}


/*
Focus will be adjusted to items after data being moved.
*/
TEST(ListControlTest, Focus_DataMoved) {

    ListControlTestFixture fixture;

    //Set focus to one of the items.
    fixture.Window().Messager().SendWMLBUTTONDOWN(Point{ 10, 45 });
    ASSERT_TRUE(fixture.ListControl().GetVisibleItemAtIndex(4)->IsFocused());

    //Move the focused index backward.
    fixture.DataSource().Move(4, 1);
    ASSERT_TRUE(fixture.ListControl().GetVisibleItemAtIndex(1)->IsFocused());

    //Move the focused index forward.
    fixture.DataSource().Move(1, 3);
    ASSERT_TRUE(fixture.ListControl().GetVisibleItemAtIndex(3)->IsFocused());
}


TEST(ListControlTest, Focus_FocusOnListWhenDataChanged) {

    //The focus hasn't been set to any item.
    {
        ListControlTestFixture fixture;
        fixture.ListControl().SetIsFocused(true);
        fixture.DataSource().AddAt(100, 0);
        ASSERT_TRUE(fixture.ListControl().IsFocused());
    }

    //The focus has been set to one of the items.
    {
        ListControlTestFixture fixture;

        //Set focus to one of the items.
        fixture.Window().Messager().SendWMLBUTTONDOWN(Point{ 10, 5 });
        ASSERT_TRUE(fixture.ListControl().GetVisibleItemAtIndex(0)->IsFocused());

        fixture.ListControl().SetIsFocused(true);
        fixture.DataSource().AddAt(100, 0);
        ASSERT_TRUE(fixture.ListControl().IsFocused());
    }
}