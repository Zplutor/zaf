#pragma once

#include <memory>
#include <zaf/base/error.h>

namespace zaf {

class DataNode;

class DataWriter {
public:
    DataWriter() { }
    virtual ~DataWriter() { }

    virtual std::string Write(const std::shared_ptr<DataNode>& data_node, std::error_code& error_code) = 0;

    std::string Write(const std::shared_ptr<DataNode>& data_node) {
        std::error_code error_code;
        auto result = Write(data_node, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    DataWriter(const DataWriter&) = delete;
    DataWriter& operator=(const DataWriter&) = delete;
};

}