#pragma once
#include <memory>
#include <vector>
#include <string>

#include <d3d11.h>

#include <imgui/imgui.h>
#include <imgui/imgui_freetype.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>

inline ImFont* esp_font = nullptr;

struct detail_t {
	HWND window = nullptr;
	WNDCLASSEX window_class = {};
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* device_context = nullptr;
	ID3D11RenderTargetView* render_target_view = nullptr;
	IDXGISwapChain* swap_chain = nullptr;
};

class render_t {
public:
	render_t();
	~render_t();

	bool running = false;

	void start_render();
	void render_menu();
	void render_visuals();
	void end_render();

	bool create_device();
	bool create_window();
	bool create_imgui();

	std::unique_ptr<detail_t> detail = std::make_unique<detail_t>();
private:
	void destroy_device();
	void destroy_window();
	void destroy_imgui();
};

inline std::unique_ptr<render_t> render = std::make_unique<render_t>();