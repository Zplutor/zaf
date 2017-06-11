#pragma once

#include <memory>
#include <zaf/base/stream.h>

namespace zaf {

class DataNode;

class DataWriter {
public:
    DataWriter() { }
    virtual ~DataWriter() { }

    virtual void Execute(
        const std::shared_ptr<DataNode>& data_node,
        const Stream& output_stream, 
        std::error_code& error_code) = 0;

    DataWriter(const DataWriter&) = delete;
    DataWriter& operator=(const DataWriter&) = delete;
};

}