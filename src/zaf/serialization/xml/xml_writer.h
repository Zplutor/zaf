#pragma once

#include <memory>
#include <zaf/base/stream.h>
#include <zaf/serialization/data_writer.h>

namespace zaf {

class XmlWriter : public DataWriter {
public:
    void Execute(
        const std::shared_ptr<DataNode>& data_node,
        Stream& output_stream, 
        std::error_code& error_code) override;
};

}