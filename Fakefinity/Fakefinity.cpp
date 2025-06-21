// VirtualCapture.cpp — Bezel Compensation, INI auto-generation, and restored smooth DXGI sync
// Mirrors display output across 3 monitors with bezel gaps skipped (fully INI-configurable)

#include <windows.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <wrl/client.h>
#include <fstream>
#include <sstream>
#include <map>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;

#define TARGET_OUTPUT_INDEX_DEFAULT 3
#define HOTKEY_QUIT_ID    1
#define HOTKEY_MINIMIZE_ID 2
#define HOTKEY_RESTORE_ID 3

HWND g_hWnd = nullptr;
ComPtr<ID3D11Device> g_d3dDevice;
ComPtr<ID3D11DeviceContext> g_d3dContext;
ComPtr<IDXGIOutputDuplication> g_duplication;
ComPtr<ID3D11Texture2D> g_stagingTex;
ComPtr<IDXGISwapChain> g_swapChain;
ComPtr<ID3D11RenderTargetView> g_rtv;
UINT g_outputWidth = 0;
UINT g_outputHeight = 0;
int g_windowX = -997;
int g_windowY = 0;
UINT g_targetOutputIndex = TARGET_OUTPUT_INDEX_DEFAULT;
UINT g_leftWidth = 997;
UINT g_centerWidth = 1920;
UINT g_rightWidth = 997;
UINT g_bezelSize = 45;
UINT g_virtualHeightOverride = 0;
UINT g_refreshRate = 60;

void LoadSettings()
{
    std::map<std::string, std::string> defaults = {
        {"WindowX", "-997"},
        {"WindowY", "0"},
        {"MonitorIndex", "3"},
        {"LeftWidth", "997"},
        {"CenterWidth", "1920"},
        {"RightWidth", "997"},
        {"BezelSize", "45"},
        {"VirtualHeight", "0"},
        {"RefreshRate", "60"}
    };

    std::ifstream iniIn("Fakefinity.ini");
    if (iniIn.is_open()) {
        std::string line;
        while (std::getline(iniIn, line)) {
            std::istringstream iss(line);
            std::string key;
            if (std::getline(iss, key, '=')) {
                std::string value;
                if (std::getline(iss, value)) {
                    defaults[key] = value;
                }
            }
        }
        iniIn.close();
    }

    g_windowX = std::stoi(defaults["WindowX"]);
    g_windowY = std::stoi(defaults["WindowY"]);
    g_targetOutputIndex = std::stoi(defaults["MonitorIndex"]);
    g_leftWidth = std::stoi(defaults["LeftWidth"]);
    g_centerWidth = std::stoi(defaults["CenterWidth"]);
    g_rightWidth = std::stoi(defaults["RightWidth"]);
    g_bezelSize = std::stoi(defaults["BezelSize"]);
    g_virtualHeightOverride = std::stoi(defaults["VirtualHeight"]);
    g_refreshRate = std::stoi(defaults["RefreshRate"]);

    std::ofstream iniOut("Fakefinity.ini", std::ios::trunc);
    for (const auto& kv : defaults) {
        iniOut << kv.first << "=" << kv.second << "\n";
    }
    iniOut.close();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_HOTKEY:
        if (wParam == HOTKEY_QUIT_ID) PostQuitMessage(0);
        else if (wParam == HOTKEY_MINIMIZE_ID) ShowWindow(hWnd, SW_MINIMIZE);
        else if (wParam == HOTKEY_RESTORE_ID) {
            ShowWindow(hWnd, SW_RESTORE);
            SetForegroundWindow(hWnd);
            SetFocus(hWnd);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

bool InitDuplication()
{
    HRESULT hr;
    D3D_FEATURE_LEVEL featureLevel;
    hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0, D3D11_SDK_VERSION,
        &g_d3dDevice, &featureLevel, &g_d3dContext);
    if (FAILED(hr)) return false;

    ComPtr<IDXGIDevice> dxgiDevice;
    g_d3dDevice.As(&dxgiDevice);
    ComPtr<IDXGIAdapter> dxgiAdapter;
    dxgiDevice->GetAdapter(&dxgiAdapter);

    ComPtr<IDXGIOutput> dxgiOutput;
    hr = dxgiAdapter->EnumOutputs(g_targetOutputIndex, &dxgiOutput);
    if (FAILED(hr)) return false;

    DXGI_OUTPUT_DESC outputDesc;
    dxgiOutput->GetDesc(&outputDesc);

    ComPtr<IDXGIOutput1> dxgiOutput1;
    dxgiOutput.As(&dxgiOutput1);
    hr = dxgiOutput1->DuplicateOutput(g_d3dDevice.Get(), &g_duplication);
    if (FAILED(hr)) return false;

    g_outputWidth = g_leftWidth + g_bezelSize + g_centerWidth + g_bezelSize + g_rightWidth;
    g_outputHeight = (g_virtualHeightOverride > 0) ? g_virtualHeightOverride :
        outputDesc.DesktopCoordinates.bottom - outputDesc.DesktopCoordinates.top;

    return true;
}

bool InitSwapChain()
{
    HRESULT hr;
    ComPtr<IDXGIDevice> dxgiDevice;
    g_d3dDevice.As(&dxgiDevice);
    ComPtr<IDXGIAdapter> dxgiAdapter;
    dxgiDevice->GetAdapter(&dxgiAdapter);
    ComPtr<IDXGIFactory1> dxgiFactory;
    dxgiAdapter->GetParent(__uuidof(IDXGIFactory1), &dxgiFactory);

    DXGI_SWAP_CHAIN_DESC scDesc = {};
    scDesc.BufferCount = 2;
    scDesc.BufferDesc.Width = g_outputWidth;
    scDesc.BufferDesc.Height = g_outputHeight;
    scDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    scDesc.BufferDesc.RefreshRate.Numerator = g_refreshRate;
    scDesc.BufferDesc.RefreshRate.Denominator = 1;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.OutputWindow = g_hWnd;
    scDesc.SampleDesc.Count = 1;
    scDesc.Windowed = TRUE;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    hr = dxgiFactory->CreateSwapChain(g_d3dDevice.Get(), &scDesc, &g_swapChain);
    if (FAILED(hr)) return false;

    ComPtr<ID3D11Texture2D> backBuffer;
    hr = g_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer);
    if (FAILED(hr)) return false;

    hr = g_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, &g_rtv);
    return SUCCEEDED(hr);
}

bool CaptureAndRender()
{
    DXGI_OUTDUPL_FRAME_INFO frameInfo = {};
    ComPtr<IDXGIResource> desktopResource;
    HRESULT hr = g_duplication->AcquireNextFrame(100, &frameInfo, &desktopResource);

    if (hr == DXGI_ERROR_WAIT_TIMEOUT)
        return true;
    if (FAILED(hr))
        return false;

    ComPtr<ID3D11Texture2D> acquiredTex;
    desktopResource.As(&acquiredTex);

    ComPtr<ID3D11Texture2D> backBuffer;
    hr = g_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer);
    if (FAILED(hr)) return false;

    UINT bezel = g_bezelSize;
    UINT leftWidth = g_leftWidth;
    UINT centerWidth = g_centerWidth;
    UINT rightWidth = g_rightWidth;

    D3D11_BOX srcLeft = { 0, 0, 0, leftWidth, g_outputHeight, 1 };
    g_d3dContext->CopySubresourceRegion(backBuffer.Get(), 0, 0, 0, 0, acquiredTex.Get(), 0, &srcLeft);

    D3D11_BOX srcCenter = { leftWidth + bezel, 0, 0, leftWidth + bezel + centerWidth, g_outputHeight, 1 };
    g_d3dContext->CopySubresourceRegion(backBuffer.Get(), 0, leftWidth, 0, 0, acquiredTex.Get(), 0, &srcCenter);

    D3D11_BOX srcRight = { leftWidth + bezel + centerWidth + bezel, 0, 0,
        leftWidth + bezel + centerWidth + bezel + rightWidth, g_outputHeight, 1 };
    g_d3dContext->CopySubresourceRegion(backBuffer.Get(), 0, leftWidth + centerWidth, 0, 0, acquiredTex.Get(), 0, &srcRight);

    g_swapChain->Present(1, 0);
    g_duplication->ReleaseFrame();
    return true;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    LoadSettings();

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"VirtualCaptureWindowClass";
    RegisterClass(&wc);

    if (!InitDuplication()) {
        MessageBox(nullptr, L"Failed to initialize duplication!", L"Error", MB_ICONERROR);
        return -1;
    }

    g_hWnd = CreateWindowEx(0, wc.lpszClassName, L"Virtual Display Capture", WS_POPUP | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, g_outputWidth, g_outputHeight,
        nullptr, nullptr, hInstance, nullptr);
    if (!g_hWnd) return -1;

    ShowWindow(g_hWnd, SW_SHOWNORMAL);
    SetForegroundWindow(g_hWnd);
    SetFocus(g_hWnd);

    SetWindowPos(g_hWnd, HWND_TOP, g_windowX, g_windowY, g_outputWidth, g_outputHeight, SWP_SHOWWINDOW);

    RegisterHotKey(g_hWnd, HOTKEY_QUIT_ID, MOD_CONTROL | MOD_ALT, 'Q');
    RegisterHotKey(g_hWnd, HOTKEY_MINIMIZE_ID, MOD_CONTROL | MOD_ALT, 'M');
    RegisterHotKey(g_hWnd, HOTKEY_RESTORE_ID, MOD_CONTROL | MOD_ALT, 'R');

    if (!InitSwapChain()) {
        MessageBox(nullptr, L"Failed to initialize SwapChain!", L"Error", MB_ICONERROR);
        return -1;
    }

    MSG msg = {};
    while (true)
    {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (msg.message == WM_QUIT) break;

        CaptureAndRender();
    }

    UnregisterHotKey(g_hWnd, HOTKEY_QUIT_ID);
    UnregisterHotKey(g_hWnd, HOTKEY_MINIMIZE_ID);
    UnregisterHotKey(g_hWnd, HOTKEY_RESTORE_ID);

    return 0;
}
