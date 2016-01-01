#pragma once

#include <memory>
#include <zaf/base/direct2d.h>
#include <zaf/graphic/stroke.h>

namespace zaf {

class Renderer;

/**
 Represent a factory that creates renderer and renderer-independent resources.
 */
class RendererFactory {
public:
	RendererFactory();
	~RendererFactory();
	
	const std::shared_ptr<Renderer> CreateRenderer(HWND window_handle);

    const std::shared_ptr<Stroke> CreateStroke(const Stroke::Properties& properties);

	ID2D1Factory* GetDirect2dFactoryHandle() const {
		return d2d_factory_handle_;
	}

	IDWriteFactory* GetDirectWriteFactoryHandle() const {
		return dwrite_factory_handle_;
	}

private:
	RendererFactory(const RendererFactory&) = delete;
	RendererFactory& operator=(const RendererFactory&) = delete;

private:
	ID2D1Factory* d2d_factory_handle_;
	IDWriteFactory* dwrite_factory_handle_;
};

}