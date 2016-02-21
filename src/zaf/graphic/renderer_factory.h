#pragma once

#include <dwrite.h>
#include <memory>
#include <zaf/base/direct2d.h>
#include <zaf/graphic/stroke.h>

namespace zaf {

class Font;
class PathGeometry;
class Renderer;
class TextFormat;

/**
 Represent a factory that creates renderers and renderer-independent resources.

 There is no need to create RendererFactory instance by yourself, use Application::GetRendererFactory 
 method instead.
 */
class RendererFactory {
public:
	/**
	 Initialize the instance with specified underlying instances.
	 */
	RendererFactory(ID2D1Factory* d2d_factory_handle, IDWriteFactory* dwrite_factory_handle);

	/**
	 Destroy the instance.
	 */
	~RendererFactory();
	
	/**
	 Create a renderer that associated with specified window.

	 @return
	 Return nullptr if failed.
	 */
	const std::shared_ptr<Renderer> CreateRenderer(HWND window_handle);

	/**
	 Create a path geometry.

	 @return 
	 Return nullptr if failed.
	 */
	const std::shared_ptr<PathGeometry> CreatePathGeometry();

	/**
	 Create a stroke with specified properties.

	 @return 
	 Return nullptr if failed.
	 */
    const std::shared_ptr<Stroke> CreateStroke(const Stroke::Properties& properties);

	/**
	 Create a text format with specified font.

	 @return 
	 Return nullptr if failed.
	 */
	const std::shared_ptr<TextFormat> CreateTextFormat(const Font& font);

	/**
	 Get the underlying ID2D1Factory instance.
	 */
	ID2D1Factory* GetDirect2dFactoryHandle() const {
		return d2d_factory_handle_;
	}

	/**
	 Get the underlying IDWriteFactory instance.
	 */
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