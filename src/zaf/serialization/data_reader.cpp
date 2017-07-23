#include <zaf/serialization/data_reader.h>
#include <zaf/serialization/json/json_reader.h>
#include <zaf/serialization/xml/xml_reader.h>

namespace zaf {

std::shared_ptr<DataReader> CreateJsonReader() {
    return std::make_shared<JsonReader>();
}


std::shared_ptr<DataReader> CreateXmlReader() {
    return std::make_shared<XmlReader>();
}

}