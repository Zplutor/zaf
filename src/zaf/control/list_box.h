#pragma once

#include <zaf/control/list_control.h>

namespace zaf {

class ListBoxDataSource;
class ListBoxDelegate;

/**
 Represents a list control to display texts.   
 */
class ListBox : public ListControl {
public:
    ZAF_OBJECT;

public:
    ListBox();
    ~ListBox();

    /**
     Get item height.

     The default height is 18.
     */
    float DefaultItemHeight() const {
        return default_item_height_;
    }

    /**
     Set item height. 
     */
    void SetDefaultItemHeight(float item_height);

    void AddItem(const std::shared_ptr<dynamic::Object>& data);
    void AddItemAtIndex(std::size_t index, const std::shared_ptr<dynamic::Object>& data);
    void RemoveItemAtIndex(std::size_t index);
    void RemoveAllItems();

protected:
    void Initialize() override;
    void OnDataSourceChanged(const std::shared_ptr<ListDataSource>& previous_data_source) override;
    void OnDelegateChanged(const std::shared_ptr<ListControlDelegate>& previous_delegate) override;

private:
    std::shared_ptr<ListBoxDataSource> data_source_;
    std::shared_ptr<ListBoxDelegate> delegate_{};

    float default_item_height_{ 18 };
};

ZAF_OBJECT_BEGIN(ListBox);
ZAF_OBJECT_END;

}