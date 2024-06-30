//
// Created by vastrakai on 6/29/2024.
//

#include "D2D.hpp"

#include <winrt/base.h>
#include <spdlog/spdlog.h>

struct BlurCallbackData;
static winrt::com_ptr<ID2D1Factory3> d2dFactory = nullptr;
static winrt::com_ptr<ID2D1Device> d2dDevice = nullptr;

static winrt::com_ptr<ID2D1Effect> blurEffect = nullptr;
static winrt::com_ptr<ID2D1Bitmap1> sourceBitmap = nullptr;

static winrt::com_ptr<ID2D1DeviceContext> d2dDeviceContext = nullptr;
static winrt::com_ptr<ID2D1SolidColorBrush> brush = nullptr;

bool initD2D = false;

float dpi = 0.0f;

void D2D::init(IDXGISwapChain* pSwapChain, ID3D11Device* pDevice)
{
    if (initD2D) return;

    if (initD2D == false && SUCCEEDED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2dFactory.put()))) {
        winrt::com_ptr<IDXGIDevice> dxgiDevice;
        pDevice->QueryInterface<IDXGIDevice>(dxgiDevice.put());
        if (dxgiDevice == nullptr) {
            return;
        }

        d2dFactory->CreateDevice(dxgiDevice.get(), d2dDevice.put());
        if (d2dDevice == nullptr) {
            return;
        }

        d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, d2dDeviceContext.put());
        if (d2dDeviceContext == nullptr) {
            return;
        }

        //Create blur
        d2dDeviceContext->CreateEffect(CLSID_D2D1GaussianBlur, blurEffect.put());
        if (blurEffect == nullptr) {
            return;
        }

    }
    spdlog::info("Initialized D2D.");
    initD2D = true;
}

void D2D::shutdown()
{
    if (!initD2D) {
        return;
    }

    d2dFactory = nullptr;
    d2dDevice = nullptr;
    d2dDeviceContext = nullptr;
    brush = nullptr;
    blurEffect = nullptr;
    sourceBitmap = nullptr;
    initD2D = false;
    spdlog::info("Shutdown D2D.");
}

void D2D::beginRender(IDXGISurface* surface, float fxdpi)
{
    D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
                                 D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
    d2dDeviceContext->CreateBitmapFromDxgiSurface(surface, &bitmapProperties, sourceBitmap.put());
}

void D2D::endRender()
{
    d2dDeviceContext->SetTarget(nullptr); // Unbind the render target
    sourceBitmap = nullptr;
}




void D2D::blurCallback(const ImDrawList* parent_list, const ImDrawCmd* cmd) {
    auto data = (BlurCallbackData*)cmd->UserCallbackData;
    if (data == nullptr) {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();
    BlurCallbackData* blurData = (BlurCallbackData*)cmd->UserCallbackData;
    ImVec4 clipRect = data->clipRect.has_value() ? *data->clipRect : cmd->ClipRect;

    d2dDeviceContext->SetTarget(sourceBitmap.get());
    d2dDeviceContext->BeginDraw();

    // Copy the current render target to a bitmap
    ID2D1Bitmap* targetBitmap = nullptr;
    D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(sourceBitmap->GetPixelFormat());
    d2dDeviceContext->CreateBitmap(sourceBitmap->GetPixelSize(), props, &targetBitmap);
    auto destPoint = D2D1::Point2U(0, 0);
    auto size = sourceBitmap->GetPixelSize();
    auto rect = D2D1::RectU(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    targetBitmap->CopyFromBitmap(&destPoint, sourceBitmap.get(), &rect);

    //Create rects and rounded rects
    D2D1_RECT_F screenRectF = D2D1::RectF(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    D2D1_RECT_F clipRectD2D = D2D1::RectF(
            clipRect.x,
            clipRect.y,
            clipRect.z,
            clipRect.w
    );
    D2D1_ROUNDED_RECT clipRectRounded = D2D1::RoundedRect(clipRectD2D, data->rounding, data->rounding);

    //Apply blur effect
    blurEffect->SetInput(0, targetBitmap);
    blurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, data->strength);
    blurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
    blurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION, D2D1_GAUSSIANBLUR_OPTIMIZATION_QUALITY);

    //Get the output image
    ID2D1Image* outImage = nullptr;
    blurEffect->GetOutput(&outImage);

    //Create bitmap brush for the clipping
    ID2D1ImageBrush* outImageBrush = nullptr;
    D2D1_IMAGE_BRUSH_PROPERTIES outImage_props = D2D1::ImageBrushProperties(screenRectF);
    d2dDeviceContext->CreateImageBrush(
            outImage,
            outImage_props,
            &outImageBrush
    );

    //Draw the blur
    ID2D1RoundedRectangleGeometry* clipRectGeo = nullptr;
    d2dFactory->CreateRoundedRectangleGeometry(clipRectRounded, &clipRectGeo);
    d2dDeviceContext->FillGeometry(clipRectGeo, outImageBrush);
    clipRectGeo->Release();

    //Finish Direct2d draw
    d2dDeviceContext->Flush();
    d2dDeviceContext->EndDraw();
    //Release interfaces
    outImageBrush->Release();
    outImage->Release();
    targetBitmap->Release();
    //Free the callback data
    for (auto it = blurCallbacks.begin(); it != blurCallbacks.end(); it++) {
        if (it->get() == data) {
            blurCallbacks.erase(it);
            break;
        }
    }

}

bool D2D::addBlur(ImDrawList* drawList, float strength, std::optional<ImVec4> clipRect, float rounding)
{
    if (!initD2D) {
        return false;
    }

    if (strength == 0)
        return false;

    auto uniqueData = std::make_shared<BlurCallbackData>(strength, rounding, clipRect);
    auto data = uniqueData.get();
    blurCallbacks.push_back(uniqueData);
    drawList->AddCallback(blurCallback, data);
    return true;
}
