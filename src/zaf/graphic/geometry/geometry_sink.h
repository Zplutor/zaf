#pragma once

#include <vector>
#include <zaf/base/direct2d.h>
#include <zaf/base/error.h>
#include <zaf/base/flag_enum.h>
#include <zaf/graphic/point.h>

namespace zaf {

class GeometrySink {
public:
	enum class FillMode {
		Alternate = 0,
		Winding = 1,
	};

	enum class SegmentFlag {
		None = 0,
		ForceUnstroked = 1 << 0, 
		ForceRoundLineJoin = 1 << 1,
	};

	enum class BeginFigureOption {
		Fill = 0,
		Hollow = 1,
	};

	enum class EndFigureOption {
		Open = 0,
		Close = 1,
	};

public:
	GeometrySink(ID2D1GeometrySink* handle) : handle_(handle) { }

	~GeometrySink() {
		if (handle_ != nullptr) {
			handle_->Release();
		}
	}

	void SetFillMode(FillMode fill_mode) {
		handle_->SetFillMode(static_cast<D2D1_FILL_MODE>(fill_mode));
	}

	void SetSegmentFlag(SegmentFlag flag) {
		handle_->SetSegmentFlags(static_cast<D2D1_PATH_SEGMENT>(flag));
	}

	void BeginFigure(const Point& start_position, BeginFigureOption option) {
		handle_->BeginFigure(start_position.ToD2D1POINT2F(), static_cast<D2D1_FIGURE_BEGIN>(option));
	}

	void EndFigure(EndFigureOption option) {
		handle_->EndFigure(static_cast<D2D1_FIGURE_END>(option));
	}

	void AddLine(const Point& end_point) {
		handle_->AddLine(end_point.ToD2D1POINT2F());
	}

	void AddLines(const std::vector<Point>& points);

	void Close() {
        std::error_code error_code;
        Close(error_code);
        ZAF_CHECK_ERROR(error_code);
	}

    void Close(std::error_code& error_code) {
        HRESULT result = handle_->Close();
        error_code = MakeComErrorCode(result);
    }

	ID2D1GeometrySink* GetHandle() const {
		return handle_;
	}

private:
	ID2D1GeometrySink* handle_;
};

ZAF_ENABLE_FLAG_ENUM(GeometrySink::SegmentFlag);

}