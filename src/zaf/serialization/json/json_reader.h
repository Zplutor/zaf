#pragma once

#include <zaf/serialization/data_reader.h>

namespace zaf {

class JsonReader : public DataReader {
public:
    using DataReader::Read;

    std::shared_ptr<DataNode> Read(const void* data, std::size_t data_length, std::error_code& error_code) override;
};

}