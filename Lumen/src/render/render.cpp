#define IMGUI_DEFINE_MATH_OPERATORS
#include <render/render.h>

#include <dwmapi.h>
#include <cstdio>
#include <chrono>
#include <thread>

#include "assets/verdana.h"

#include <globals.h>
#include <features/esp/esp.h>
#include <features/settings.h>
#include "sdk/math/math.h"
#include <game/game.h>
#include <logger/logger.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
    {
        return true;
    }

    switch (msg)
    {
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
        {
            return 0;
        }
        break;

    case WM_SYSKEYDOWN:
        if (wParam == VK_F4) {
            DestroyWindow(hwnd);
            return 0;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CLOSE:
        return 0;
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

render_t::render_t()
{
    detail = std::make_unique<detail_t>();
}

render_t::~render_t()
{
    destroy_imgui();
    destroy_window();
    destroy_device();
}

bool render_t::create_window()
{
    detail->window_class.cbSize = sizeof(detail->window_class);
    detail->window_class.style = CS_CLASSDC;
    detail->window_class.lpszClassName = "T4";
    detail->window_class.hInstance = GetModuleHandleA(0);
    detail->window_class.lpfnWndProc = wnd_proc;

    RegisterClassExA(&detail->window_class);

    detail->window = CreateWindowExA(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW,
        detail->window_class.lpszClassName,
        "T4",
        WS_POPUP,
        0,
        0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        0,
        0,
        detail->window_class.hInstance,
        0
    );

    if (!detail->window)
    {
        return false;
    }

    SetLayeredWindowAttributes(detail->window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

    RECT client_area{};
    RECT window_area{};

    GetClientRect(detail->window, &client_area);
    GetWindowRect(detail->window, &window_area);

    POINT diff{};
    ClientToScreen(detail->window, &diff);

    MARGINS margins
    {
        window_area.left + (diff.x - window_area.left),
        window_area.top + (diff.y - window_area.top),
        window_area.right,
        window_area.bottom,
    };

    DwmExtendFrameIntoClientArea(detail->window, &margins);

    ShowWindow(detail->window, SW_SHOW);
    UpdateWindow(detail->window);

    return true;
}

bool render_t::create_device()
{
    DXGI_SWAP_CHAIN_DESC swap_chain_desc{};

    swap_chain_desc.BufferCount = 1;

    swap_chain_desc.BufferDesc.Width = 0;
    swap_chain_desc.BufferDesc.Height = 0;
    swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    swap_chain_desc.OutputWindow = detail->window;

    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swap_chain_desc.Windowed = 1;

    swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    swap_chain_desc.SampleDesc.Count = 2;
    swap_chain_desc.SampleDesc.Quality = 0;

    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    D3D_FEATURE_LEVEL feature_level;
    D3D_FEATURE_LEVEL feature_level_list[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

    HRESULT result = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        feature_level_list,
        2,
        D3D11_SDK_VERSION,
        &swap_chain_desc,
        &detail->swap_chain,
        &detail->device,
        &feature_level,
        &detail->device_context
    );

    if (result == DXGI_ERROR_UNSUPPORTED)
    {
        result = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_WARP,
            nullptr,
            0,
            feature_level_list,
            2,
            D3D11_SDK_VERSION,
            &swap_chain_desc,
            &detail->swap_chain,
            &detail->device,
            &feature_level,
            &detail->device_context
        );
    }

    if (result != S_OK)
    {
        MessageBoxA(nullptr, "This software can not run on your computer.", "Critical Problem", MB_ICONERROR | MB_OK);
    }

    ID3D11Texture2D* back_buffer{ nullptr };
    detail->swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));

    if (back_buffer)
    {
        detail->device->CreateRenderTargetView(back_buffer, nullptr, &detail->render_target_view);
        back_buffer->Release();

        return true;
    }

    return false;
}

bool render_t::create_imgui()
{
    using namespace ImGui;
    CreateContext();
    StyleColorsDark();

	float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

    ImGuiStyle& style = ImGui::GetStyle();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = nullptr;

    ImGui::StyleColorsDark();
	style.ScaleAllSizes(main_scale);
	style.FontScaleDpi = main_scale;

    float verdana_regular_size = 13.f;
    verdana_regular_size *= main_scale;

	const unsigned int freetype_flags = ImGuiFreeTypeLoaderFlags_MonoHinting | ImGuiFreeTypeLoaderFlags_Monochrome;
	io.Fonts->SetFontLoader(ImGuiFreeType::GetFontLoader());
	io.Fonts->FontLoaderFlags = freetype_flags;

	ImFontConfig font_cfg;
	font_cfg.PixelSnapH = true;
	font_cfg.OversampleH = 2;
	font_cfg.OversampleV = 1;
	font_cfg.RasterizerMultiply = 1.05f;
	font_cfg.FontLoaderFlags = freetype_flags;

	ImFontConfig verdana_regular_cfg = font_cfg;
	verdana_regular_cfg.FontDataOwnedByAtlas = false;
	io.Fonts->AddFontFromMemoryTTF((void*)font_verdana_regular, sizeof(font_verdana_regular), verdana_regular_size, &verdana_regular_cfg);

    if (!ImGui_ImplWin32_Init(detail->window))
    {
        return false;
    }

    if (!detail->device || !detail->device_context)
    {
        return false;
    }

    if (!ImGui_ImplDX11_Init(detail->device, detail->device_context))
    {
        return false;
    }

    return true;
}

void render_t::destroy_device()
{
	if (detail->render_target_view) detail->render_target_view->Release();
	if (detail->swap_chain) detail->swap_chain->Release();
	if (detail->device_context) detail->device_context->Release();
	if (detail->device) detail->device->Release();
}

void render_t::destroy_window()
{
    DestroyWindow(detail->window);
    UnregisterClassA(detail->window_class.lpszClassName, detail->window_class.hInstance);
}

void render_t::destroy_imgui()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void render_t::start_render()
{
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

	if (settings::visuals::streamproof)
	{
		SetWindowDisplayAffinity(detail->window, WDA_EXCLUDEFROMCAPTURE);
	}
	else
	{
		SetWindowDisplayAffinity(detail->window, WDA_NONE);
	}

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (GetAsyncKeyState(VK_HOME) & 1)
    {
        running = !running;

        if (running)
        {
            SetWindowLong(detail->window, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT);
        }
        else
        {
            SetWindowLong(detail->window, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_LAYERED);
        }
    }
}

void render_t::end_render()
{
    ImGui::Render();

    float clear_color[4]{ 0, 0, 0, 0 };
    detail->device_context->OMSetRenderTargets(1, &detail->render_target_view, nullptr);
    detail->device_context->ClearRenderTargetView(detail->render_target_view, clear_color);

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    detail->swap_chain->Present(0, 0);
}

void render_t::render_menu()
{
    static std::int32_t tab = 0;

    static std::int32_t aimbot_subtab = 0;

    ImGui::SetNextWindowSize({ 400, 500 }, ImGuiCond_Once);
    ImGui::Begin("External");

    if (ImGui::Button("Aim"))
    {
        tab = 0;
    }
    ImGui::SameLine();
	if (ImGui::Button("Visual"))
	{
		tab = 1;
	}

    ImGui::Separator();

    switch (tab)
    {
    case 0:
    {
        ImGui::Text("Aimbot");
        ImGui::Checkbox("Enable aimbot", &settings::aimbot::enabled);

        ImGui::NewLine();

        ImGui::Text("Camera Aimbot");
        ImGui::Checkbox("Enable camera aimbot", &settings::aimbot::camera::enabled);
        ImGui::Checkbox("Smoothing", &settings::aimbot::camera::smoothing_enabled);
        if (settings::aimbot::camera::smoothing_enabled)
        {
            ImGui::SliderFloat("Smoothing value", &settings::aimbot::camera::smoothing_value, 0.1f, 50.f, "%.1f");
        }

        ImGui::NewLine();

        static constexpr const char* const parts
        {
            "Closest\0Head\0Torso\0HumanoidRootPart"
        };
        ImGui::Combo("Target Part", &settings::aimbot::target_part, parts);

        ImGui::NewLine();

		ImGui::Text("Checks (shared)");
		ImGui::Checkbox("Team Check", &settings::visuals::teamcheck);
		ImGui::Checkbox("Dead Check", &settings::visuals::deadcheck);
		ImGui::Checkbox("Wall Check", &settings::visuals::wallcheck);

        ImGui::NewLine();

        ImGui::Text("Distance settings");
        
        if (ImGui::Button("0-20"))
        {
            aimbot_subtab = 0;
        }
		ImGui::SameLine();
		if (ImGui::Button("20-100"))
		{
            aimbot_subtab = 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("100-200"))
		{
            aimbot_subtab = 2;
		}
		ImGui::SameLine();
		if (ImGui::Button("200-300"))
		{
            aimbot_subtab = 3;
		}
		ImGui::SameLine();
		if (ImGui::Button("300-400"))
		{
            aimbot_subtab = 4;
		}
		ImGui::SameLine();
		if (ImGui::Button("400+"))
		{
            aimbot_subtab = 5;
		}

        switch(aimbot_subtab)
        {
        case 0:
		{
            ImGui::Text("Selected: 0-20");
			ImGui::SliderFloat("FOV size", &settings::aimbot::a1::fov, 0.f, 1000.f, "%.0f");
			ImGui::SliderFloat("Smooth x", &settings::aimbot::a1::smooth_x, 0.f, 500.f, "%.0f");
			ImGui::SliderFloat("Smooth y", &settings::aimbot::a1::smooth_y, 0.f, 500.f, "%.0f");
            break;
        }
        case 1:
		{
            ImGui::Text("Selected: 20-100");
			ImGui::SliderFloat("FOV size", &settings::aimbot::a2::fov, 0.f, 1000.f, "%.0f");
			ImGui::SliderFloat("Smooth x", &settings::aimbot::a2::smooth_x, 0.f, 500.f, "%.0f");
			ImGui::SliderFloat("Smooth y", &settings::aimbot::a2::smooth_y, 0.f, 500.f, "%.0f");
            break;
        }
		case 2:
		{
            ImGui::Text("Selected: 100-200");
			ImGui::SliderFloat("FOV size", &settings::aimbot::a3::fov, 0.f, 1000.f, "%.0f");
			ImGui::SliderFloat("Smooth x", &settings::aimbot::a3::smooth_x, 0.f, 500.f, "%.0f");
			ImGui::SliderFloat("Smooth y", &settings::aimbot::a3::smooth_y, 0.f, 500.f, "%.0f");
			break;
		}
		case 3:
		{
            ImGui::Text("Selected: 200-300");
			ImGui::SliderFloat("FOV size", &settings::aimbot::a4::fov, 0.f, 1000.f, "%.0f");
			ImGui::SliderFloat("Smooth x", &settings::aimbot::a4::smooth_x, 0.f, 500.f, "%.0f");
			ImGui::SliderFloat("Smooth y", &settings::aimbot::a4::smooth_y, 0.f, 500.f, "%.0f");
			break;
		}
		case 4:
		{
            ImGui::Text("Selected: 300-400");
			ImGui::SliderFloat("FOV size", &settings::aimbot::a5::fov, 0.f, 1000.f, "%.0f");
			ImGui::SliderFloat("Smooth x", &settings::aimbot::a5::smooth_x, 0.f, 500.f, "%.0f");
			ImGui::SliderFloat("Smooth y", &settings::aimbot::a5::smooth_y, 0.f, 500.f, "%.0f");
			break;
		}
		case 5:
		{
            ImGui::Text("Selected: 400+");
			ImGui::SliderFloat("FOV size", &settings::aimbot::a6::fov, 0.f, 1000.f, "%.0f");
			ImGui::SliderFloat("Smooth x", &settings::aimbot::a6::smooth_x, 0.f, 500.f, "%.0f");
			ImGui::SliderFloat("Smooth y", &settings::aimbot::a6::smooth_y, 0.f, 500.f, "%.0f");
			break;
		}
        }
        break;
    }
    case 1:
    {
		ImGui::Text("ESP");
		ImGui::Checkbox("Draw box", &settings::visuals::box);
		ImGui::ColorEdit4("Box colour", (float*)settings::visuals::colour, ImGuiColorEditFlags_NoInputs);

        ImGui::NewLine();

        ImGui::Text("Checks (shared)");
        ImGui::Checkbox("Team Check", &settings::visuals::teamcheck);
        ImGui::Checkbox("Dead Check", &settings::visuals::deadcheck);
        ImGui::Checkbox("Wall Check", &settings::visuals::wallcheck);

		ImGui::NewLine();

        ImGui::Text("Misc");
		ImGui::Checkbox("Enable streamproof", &settings::visuals::streamproof);
		ImGui::Checkbox("Debug wallcheck", &settings::visuals::debug_wallcheck);
		ImGui::SliderFloat("Debug wallcheck max length", &settings::visuals::debug_wallcheck_max_length, 25.f, 1000.f, "%.0f");

        break;
    }
    default:
        break;
    }

    ImGui::End();
}

void render_t::render_visuals()
{
    esp::run();
}