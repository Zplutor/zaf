#pragma once

#include <zaf/serialization/data_reader.h>

namespace zaf {

class XmlReader : public DataReader {
public:
    std::shared_ptr<DataNode> Execute(const Stream& stream, std::error_code& error_code) override;
};

}