#pragma once

#include <memory>
#include <system_error>

namespace zaf {

class DataNode;
class Stream;

class DataWriter {
public:
    DataWriter() { }
    virtual ~DataWriter() { }

    virtual void Execute(
        const std::shared_ptr<DataNode>& data_node,
        Stream& output_stream, 
        std::error_code& error_code) = 0;

    DataWriter(const DataWriter&) = delete;
    DataWriter& operator=(const DataWriter&) = delete;
};

}