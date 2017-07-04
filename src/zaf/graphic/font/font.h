#pragma once

#include <Windows.h>
#include <string>
#include <zaf/graphic/font/font_weight.h>
#include <zaf/serialization/serializable_object.h>

namespace zaf {

/**
 Describes various properties of a font.
 */
class Font : public SerializableObject {
public:
    ZAF_DECLARE_TYPE_NAME();

public:
    /**
     Get the default font from system setting.
     */
    static const Font GetDefault();

    /**
     Convert a specified LOGFONT structure to Font.
     */
    static const Font FromLOGFONT(const LOGFONT& logfont);

public:
	/**
	 Initialize the instance.
	 */
	Font() :
		size(0),
		weight(FontWeight::Regular) {

	}

public:
	/**
	 Font family name.

	 The default family name is empty, means the default font family name
	 of system.
	 */
	std::wstring family_name;

	/**
	 Font size.

	 The default size is 0.
	 */
	float size;

	/**
	 Font weight.

	 A valid value is between 1 and 999. There are predefined values
	 contained in class FontWeight. The default font weight value is
	 FontWeight::Regular.
	 */
	int weight;

protected:
    void SerializeToDataNode(DataNode& data_node) const override;
    bool DeserializeFromDataNode(const DataNode& data_node) override;
};

}