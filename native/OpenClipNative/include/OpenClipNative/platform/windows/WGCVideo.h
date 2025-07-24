#pragma once

#if defined(_WIN32) || defined(_WIN64)

#include <OpenClipNative/platform/VideoBackend.h>
#include <OpenClipNative/common/CircularBuffer.h>

#include <Windows.h>
#include <d3d11_4.h>
#include <wrl/client.h>
#include <winrt/base.h>

namespace winrt::Windows::Graphics::Capture {
    struct GraphicsCaptureItem;
    struct Direct3D11CaptureFramePool;
    struct GraphicsCaptureSession;
}
namespace winrt::Windows::Graphics::DirectX::Direct3D11 {
    struct IDirect3DDevice;
}


class WGCVideo : public VideoBackend {
public:
    WGCVideo();
    ~WGCVideo() override;

    int StartVideoCapture(HWND targetHandle) override;
    void StopVideoCapture() override;
    int ReadVideoBuffer(unsigned char* pBuffer, int bufferSize) override;
    int GetVideoWidth() override;
    int GetVideoHeight() override;

private:
    void CaptureThreadFunc();
    void OnFrameArrived(
        winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool const& sender,
        winrt::Windows::Foundation::IInspectable const& args);
    void Cleanup();

    Microsoft::WRL::ComPtr<ID3D11Device> m_d3d11Device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_d3d11Context;
    winrt::com_ptr<winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice> m_winrtDevice;
    winrt::com_ptr<winrt::Windows::Graphics::Capture::GraphicsCaptureItem> m_captureItem;
    winrt::com_ptr<winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool> m_framePool{ nullptr };
    winrt::com_ptr<winrt::Windows::Graphics::Capture::GraphicsCaptureSession> m_session{ nullptr };
    winrt::event_token m_frameArrivedToken;

    std::thread m_captureThread;
    std::atomic<bool> m_isCapturing;

    CircularBuffer<BYTE> m_videoCircularBuffer;

    std::atomic<int> m_width;
    std::atomic<int> m_height;
};

#endif // _WIN32