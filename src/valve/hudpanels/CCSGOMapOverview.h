#pragma once

class CTextureDx11
{
public:
    MEM_PAD(0x10); // 0x0
    ID3D11ShaderResourceView* m_pTextureSRV0; // 0x10
    ID3D11ShaderResourceView* m_pTextureSRV1; // 0x18
};

class CSource2UITexture
{
public:
    class Data
    {
    public:
        CTextureDx11* m_pDx11Texture; // 0x0
    };

    MEM_PAD(0x28); // 0x0
    Data* m_pData; // 0x28
};

class CImageData
{
public:
    MEM_PAD(0x30); // 0x0
    CSource2UITexture* m_pUITexture; // 0x30
};

class CImageProxySource
{
public:
    MEM_PAD(0x10); // 0x0
    CImageData* m_pImageData; // 0x10
};

class CImagePanel
{
public:
    MEM_PAD(0xD0); // 0x0
    CImageProxySource* m_pImageProxy; // 0xD0

    inline ID3D11ShaderResourceView* GetNativeTexture()
    {
        CImageProxySource* pImageProxy = m_pImageProxy;
        if (!pImageProxy)
            return nullptr;

        CImageData* pImageData = pImageProxy->m_pImageData;
        if (!pImageData)
            return nullptr;

        CSource2UITexture* pUITexture = pImageData->m_pUITexture;
        if (!pUITexture)
            return nullptr;

        CSource2UITexture::Data* pUITextureData = pUITexture->m_pData;
        if (!pUITextureData)
            return nullptr;

        CTextureDx11* pDX11Texture = pUITextureData->m_pDx11Texture;
        if (!pDX11Texture)
            return nullptr;

        return pDX11Texture->m_pTextureSRV0;
    }
};

class CCSGO_MapOverview
{
public:
    // @ida: #STR "file://{images}/overheadmaps/%s_radar.%s" ref to smallest function
    // [Ptr - 0x28]
    CImagePanel* m_pMap2DPanel() {
        return *(CImagePanel**)(std::uintptr_t(this) + 0x2b0);
    }

    Vector2D m_pMapPos()
    {
        return *(Vector2D*)(std::uintptr_t(this) + 0x190);
    }

    float m_flMapScale() {
        return *(float*)(std::uintptr_t(this) + 0x1a4);
    }

    float m_flMapHeight() {
        return *(float*)(std::uintptr_t(this) + 0x1a8);
    }

    float m_flMapWidth() {
        return *(float*)(std::uintptr_t(this) + 0x1a8 + sizeof(float));
    }
};