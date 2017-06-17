#pragma once

#include <memory>
#include <zaf/base/error.h>

namespace zaf {

class DataNode;
class Stream;

class DataReader {
public:
    DataReader() { }
    virtual ~DataReader() { }

    virtual std::shared_ptr<DataNode> Read(const void* data, std::size_t data_length, std::error_code& error_code) = 0;

    std::shared_ptr<DataNode> Read(const void* data, std::size_t data_length) {
        std::error_code error_code;
        auto result = Read(data, data_length, error_code);
        ZAF_CHECK_ERROR(error_code);
        return result;
    }

    std::shared_ptr<DataNode> Read(const std::string& data, std::error_code& error_code) {
        return Read(data.data(), data.length(), error_code);
    }

    std::shared_ptr<DataNode> Read(const std::string& data) {
        return Read(data.data(), data.length());
    }

    DataReader(const DataReader&) = delete;
    DataReader& operator=(const DataReader&) = delete;
};

}