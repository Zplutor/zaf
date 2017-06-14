#pragma once

#include <memory>
#include <system_error>

namespace zaf {

class DataNode;
class Stream;

class DataReader {
public:
    DataReader() { }
    virtual ~DataReader() { }

    virtual std::shared_ptr<DataNode> Execute(const Stream& stream, std::error_code& error_code) = 0;

    DataReader(const DataReader&) = delete;
    DataReader& operator=(const DataReader&) = delete;
};

}