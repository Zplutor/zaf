#include <zaf/graphic/gif_metadata_querier.h>

namespace zaf {

std::optional<std::size_t> GifGlobalMetadataQuerier::GetLoopCount() const {

    std::optional<std::size_t> result;

    auto identifier = GetApplicationExtensionIdentifier();

    auto identifier_size = identifier.size();
    if (identifier_size != 11) {
        return result;
    }

    if (memcmp(identifier.data(), "NETSCAPE2.0", identifier_size) &&
        memcmp(identifier.data(), "ANIMEXTS1.0", identifier_size)) {
        return result;
    }

    /*
     The data is in the following format:
     byte 0: extsize (must be > 1)
     byte 1: loopType (1 == animated gif)
     byte 2: loop count (least significant byte)
     byte 3: loop count (most significant byte)
     byte 4: set to zero
     */
    auto data = GetApplicationExtensionData();
    if (data.size() < 4) {
        return result;
    }

    if ((data[0] < 1) || (data[1] != 1)) {
        return result;
    }

    result = data[2] | (data[3] << 8);
    return result;
}

}