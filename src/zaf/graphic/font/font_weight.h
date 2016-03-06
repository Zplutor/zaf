#pragma once

namespace zaf {

/**
 Provides predefined values of font weight.
 */
class FontWeight {
public:
	static const int Thin = 100;
	static const int ExtraLight = 200;
	static const int Light = 300;
	static const int SemiLight = 350;
	static const int Regular = 400;
	static const int Medium = 500;
	static const int SemiBold = 600;
	static const int Bold = 700;
	static const int ExtraBold = 800;
	static const int Black = 900;
	static const int ExtraBlack = 950;

private:
	FontWeight();
};

}