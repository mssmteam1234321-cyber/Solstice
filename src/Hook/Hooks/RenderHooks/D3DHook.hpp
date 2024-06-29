#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <Hook/Hook.hpp>
#include <Hook/HookManager.hpp>
#include <SDK/SigManager.hpp>
#include <SDK/Minecraft/ClientInstance.hpp>
//
// Created by vastrakai on 6/29/2024.
//



class D3DHook : public Hook
{
public:
    D3DHook() : Hook() {
        mName = "D3DHook";
    }


    static HRESULT present(IDXGISwapChain3* swapChain, UINT syncInterval, UINT flags);
    static HRESULT resizeBuffers(IDXGISwapChain3* swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags);
    static void initImGui(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
    static void shutdownImGui();
    static void igNewFrame();
    static void igEndFrame();
    void init() override;
    void shutdown() override;
};

REGISTER_HOOK(D3DHook);
