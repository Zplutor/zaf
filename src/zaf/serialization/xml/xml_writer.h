#pragma once

#include <zaf/serialization/data_writer.h>

namespace zaf {

class XmlWriter : public DataWriter {
public:
    using DataWriter::Write;

    std::string Write(const std::shared_ptr<DataNode>& data_node, std::error_code& error_code) override;
};

}