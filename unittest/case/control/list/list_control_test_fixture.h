#pragma once

#include <zaf/control/list_control.h>
#include <zaf/control/list_data_source.h>
#include <zaf/window/window.h>

namespace zaf::test {

class TestListDataSource;

class ListControlTestFixture {
public:
    ListControlTestFixture();
    ~ListControlTestFixture();

    zaf::Window& Window() const {
        return *window_;
    }

    zaf::ListControl& ListControl() const {
        return *list_control_;
    }

    TestListDataSource& DataSource() const {
        return *data_source_;
    }

private:
    void InitializeWindow();
    void InitializeListControl();

private:
    std::shared_ptr<zaf::Window> window_;
    std::shared_ptr<zaf::ListControl> list_control_;
    std::shared_ptr<TestListDataSource> data_source_;
    std::shared_ptr<zaf::ListControlDelegate> delegate_;
};


class TestListDataSource : public zaf::ListDataSource {
public:
    TestListDataSource();

    std::size_t GetDataCount() const;
    std::shared_ptr<zaf::Object> GetDataAtIndex(std::size_t index) const;

    void AddAt(int value, std::size_t index);
    void RemoveAt(std::size_t index);
    void Move(std::size_t from, std::size_t to);

private:
    std::vector<std::shared_ptr<Int32>> data_list_;
};

}