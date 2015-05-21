//
// Copyright (c) 2008-2015 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "../../Core/Context.h"
#include "../../IO/FileSystem.h"
#include "../../Graphics/Graphics.h"
#include "../../Graphics/GraphicsEvents.h"
#include "../../Graphics/GraphicsImpl.h"
#include "../../IO/Log.h"
#include "../../Graphics/Renderer.h"
#include "../../Core/Profiler.h"
#include "../../Resource/ResourceCache.h"
#include "../../Graphics/Texture3D.h"
#include "../../Resource/XMLFile.h"

#include "../../DebugNew.h"

namespace Urho3D
{

Texture3D::Texture3D(Context* context) :
    Texture(context)
{
}

Texture3D::~Texture3D()
{
    Release();
}

void Texture3D::RegisterObject(Context* context)
{
    context->RegisterFactory<Texture3D>();
}

bool Texture3D::BeginLoad(Deserializer& source)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    // In headless mode, do not actually load the texture, just return success
    if (!graphics_)
        return true;
    
    String texPath, texName, texExt;
    SplitPath(GetName(), texPath, texName, texExt);
    
    cache->ResetDependencies(this);

    loadParameters_ = new XMLFile(context_);
    if (!loadParameters_->Load(source))
    {
        loadParameters_.Reset();
        return false;
    }
    
    XMLElement textureElem = loadParameters_->GetRoot();
    XMLElement volumeElem = textureElem.GetChild("volume");
    XMLElement colorlutElem = textureElem.GetChild("colorlut");

    if (volumeElem)
    {
        String name = volumeElem.GetAttribute("name");
        
        String volumeTexPath, volumeTexName, volumeTexExt;
        SplitPath(name, volumeTexPath, volumeTexName, volumeTexExt);
        // If path is empty, add the XML file path
        if (volumeTexPath.Empty())
            name = texPath + name;

        loadImage_ = cache->GetTempResource<Image>(name);
        // Precalculate mip levels if async loading
        if (loadImage_ && GetAsyncLoadState() == ASYNC_LOADING)
            loadImage_->PrecalculateLevels();
        cache->StoreResourceDependency(this, name);
        return true;
    }
    else if (colorlutElem)
    {
        String name = colorlutElem.GetAttribute("name");
        
        String colorlutTexPath, colorlutTexName, colorlutTexExt;
        SplitPath(name, colorlutTexPath, colorlutTexName, colorlutTexExt);
        // If path is empty, add the XML file path
        if (colorlutTexPath.Empty())
            name = texPath + name;

        SharedPtr<File> file = GetSubsystem<ResourceCache>()->GetFile(name);
        loadImage_ = new Image(context_);
        if (!loadImage_->LoadColorLUT(*(file.Get())))
        {
            loadParameters_.Reset();
            loadImage_.Reset();
            return false;
        }
        // Precalculate mip levels if async loading
        if (loadImage_ && GetAsyncLoadState() == ASYNC_LOADING)
            loadImage_->PrecalculateLevels();
        cache->StoreResourceDependency(this, name);
        return true;
    }

    LOGERROR("Texture3D XML data for " + GetName() + " did not contain either volume or colorlut element");
    return false;
}

bool Texture3D::EndLoad()
{
    // In headless mode, do not actually load the texture, just return success
    if (!graphics_)
        return true;
    
    // If over the texture budget, see if materials can be freed to allow textures to be freed
    CheckTextureBudget(GetTypeStatic());

    SetParameters(loadParameters_);
    bool success = SetData(loadImage_);
    
    loadImage_.Reset();
    loadParameters_.Reset();
    
    return success;
}

void Texture3D::Release()
{
    if (object_)
    {
        if (!graphics_)
            return;
        
        for (unsigned i = 0; i < MAX_TEXTURE_UNITS; ++i)
        {
            if (graphics_->GetTexture(i) == this)
                graphics_->SetTexture(i, 0);
        }
        
        if (renderSurface_)
            renderSurface_->Release();
        
        ((ID3D11Resource*)object_)->Release();
        object_ = 0;

        if (shaderResourceView_)
        {
            ((ID3D11ShaderResourceView*)shaderResourceView_)->Release();
            shaderResourceView_ = 0;
        }

        if (sampler_)
        {
            ((ID3D11SamplerState*)sampler_)->Release();
            sampler_ = 0;
        }
    }
    else
    {
        if (renderSurface_)
            renderSurface_->Release();
    }
}

bool Texture3D::SetSize(int width, int height, int depth, unsigned format, TextureUsage usage)
{
    // Delete the old rendersurface if any
    renderSurface_.Reset();
    usage_ = usage;

    if (usage_ == TEXTURE_RENDERTARGET)
    {
        renderSurface_ = new RenderSurface(this);

        // Clamp mode addressing by default, nearest filtering, and mipmaps disabled
        addressMode_[COORD_U] = ADDRESS_CLAMP;
        addressMode_[COORD_V] = ADDRESS_CLAMP;
        filterMode_ = FILTER_NEAREST;
        requestedLevels_ = 1;
    }
    
    if (usage_ == TEXTURE_RENDERTARGET)
        SubscribeToEvent(E_RENDERSURFACEUPDATE, HANDLER(Texture3D, HandleRenderSurfaceUpdate));
    else
        UnsubscribeFromEvent(E_RENDERSURFACEUPDATE);

    width_ = width;
    height_ = height;
    depth_ = depth;
    format_ = format;
    
    return Create();
}

bool Texture3D::SetData(unsigned level, int x, int y, int z, int width, int height, int depth, const void* data)
{
    PROFILE(SetTextureData);
    
    if (!object_)
    {
        LOGERROR("No texture created, can not set data");
        return false;
    }
    
    if (!data)
    {
        LOGERROR("Null source for setting data");
        return false;
    }
    
    if (level >= levels_)
    {
        LOGERROR("Illegal mip level for setting data");
        return false;
    }
    
    int levelWidth = GetLevelWidth(level);
    int levelHeight = GetLevelHeight(level);
    int levelDepth = GetLevelDepth(level);
    if (x < 0 || x + width > levelWidth || y < 0 || y + height > levelHeight || z < 0 || z + depth > levelDepth || width <= 0 || height <= 0 || depth <= 0)
    {
        LOGERROR("Illegal dimensions for setting data");
        return false;
    }
    
    // If compressed, align the update region on a block
    if (IsCompressed())
    {
        x &= ~3;
        y &= ~3;
        width += 3;
        width &= 0xfffffffc;
        height += 3;
        height &= 0xfffffffc;
    }

    unsigned char* src = (unsigned char*)data;
    unsigned rowSize = GetRowDataSize(width);
    unsigned rowStart = GetRowDataSize(x);
    unsigned subResource = D3D11CalcSubresource(level, 0, levels_);

    if (usage_ == TEXTURE_DYNAMIC)
    {
        if (IsCompressed())
        {
            height = (height + 3) >> 2;
            y >>= 2;
        }

        D3D11_MAPPED_SUBRESOURCE mappedData;
        mappedData.pData = 0;

        graphics_->GetImpl()->GetDeviceContext()->Map((ID3D11Resource*)object_, subResource, D3D11_MAP_WRITE_DISCARD, 0,
            &mappedData);
        if (mappedData.pData)
        {
            for (int page = 0; page < depth; ++page)
            {
                for (int row = 0; row < height; ++row)
                {
                    memcpy((unsigned char*)mappedData.pData + (page + z) * mappedData.DepthPitch + (row + y) * mappedData.RowPitch
                        + rowStart, src + row * rowSize, rowSize);
                }
            }

            graphics_->GetImpl()->GetDeviceContext()->Unmap((ID3D11Resource*)object_, subResource);
        }
        else
        {
            LOGERROR("Failed to map texture for update");
            return false;
        }
    }
    else
    {
        if (IsCompressed())
            levelHeight = (levelHeight + 3) >> 2;

        D3D11_BOX destBox;
        destBox.left = x;
        destBox.right = x + width;
        destBox.top = y;
        destBox.bottom = y + height;
        destBox.front = z;
        destBox.back = z + depth;

        graphics_->GetImpl()->GetDeviceContext()->UpdateSubresource((ID3D11Resource*)object_, subResource, &destBox, data,
            rowSize, levelHeight * rowSize);
    }

    return true;
}

bool Texture3D::SetData(SharedPtr<Image> image, bool useAlpha)
{
    if (!image)
    {
        LOGERROR("Null image, can not load texture");
        return false;
    }
    
    unsigned memoryUse = sizeof(Texture3D);
    
    int quality = QUALITY_HIGH;
    Renderer* renderer = GetSubsystem<Renderer>();
    if (renderer)
        quality = renderer->GetTextureQuality();
    
    if (!image->IsCompressed())
    {
        // Convert unsuitable formats to RGBA
        unsigned components = image->GetComponents();
        if ((components == 1 && !useAlpha) || components == 2 || components == 3)
        {
            image = image->ConvertToRGBA();
            if (!image)
                return false;
            components = image->GetComponents();
        }

        unsigned char* levelData = image->GetData();
        int levelWidth = image->GetWidth();
        int levelHeight = image->GetHeight();
        int levelDepth = image->GetDepth();
        unsigned format = 0;
        
        // Discard unnecessary mip levels
        for (unsigned i = 0; i < mipsToSkip_[quality]; ++i)
        {
            image = image->GetNextLevel();
            levelData = image->GetData();
            levelWidth = image->GetWidth();
            levelHeight = image->GetHeight();
            levelDepth = image->GetDepth();
        }
        
        switch (components)
        {
        case 1:
            format = Graphics::GetAlphaFormat();
            break;

        case 4:
            format = Graphics::GetRGBAFormat();
            break;
        }
        
        // If image was previously compressed, reset number of requested levels to avoid error if level count is too high for new size
        if (IsCompressed() && requestedLevels_ > 1)
            requestedLevels_ = 0;
        SetSize(levelWidth, levelHeight, levelDepth, format);
        
        for (unsigned i = 0; i < levels_; ++i)
        {
            SetData(i, 0, 0, 0, levelWidth, levelHeight, levelDepth, levelData);
            memoryUse += levelWidth * levelHeight * levelDepth * components;
            
            if (i < levels_ - 1)
            {
                image = image->GetNextLevel();
                levelData = image->GetData();
                levelWidth = image->GetWidth();
                levelHeight = image->GetHeight();
                levelDepth = image->GetDepth();
            }
        }
    }
    else
    {
        int width = image->GetWidth();
        int height = image->GetHeight();
        int depth = image->GetDepth();
        unsigned levels = image->GetNumCompressedLevels();
        unsigned format = graphics_->GetFormat(image->GetCompressedFormat());
        bool needDecompress = false;
        
        if (!format)
        {
            format = Graphics::GetRGBAFormat();
            needDecompress = true;
        }
        
        unsigned mipsToSkip = mipsToSkip_[quality];
        if (mipsToSkip >= levels)
            mipsToSkip = levels - 1;
        while (mipsToSkip && (width / (1 << mipsToSkip) < 4 || height / (1 << mipsToSkip) < 4 || depth / (1 << mipsToSkip) < 4))
            --mipsToSkip;
        width /= (1 << mipsToSkip);
        height /= (1 << mipsToSkip);
        depth /= (1 << mipsToSkip);
        
        SetNumLevels(Max((int)(levels - mipsToSkip), 1));
        SetSize(width, height, depth, format);
        
        for (unsigned i = 0; i < levels_ && i < levels - mipsToSkip; ++i)
        {
            CompressedLevel level = image->GetCompressedLevel(i + mipsToSkip);
            if (!needDecompress)
            {
                SetData(i, 0, 0, 0, level.width_, level.height_, level.depth_, level.data_);
                memoryUse += level.depth_ * level.rows_ * level.rowSize_;
            }
            else
            {
                unsigned char* rgbaData = new unsigned char[level.width_ * level.height_ * level.depth_ * 4];
                level.Decompress(rgbaData);
                SetData(i, 0, 0, 0, level.width_, level.height_, level.depth_, rgbaData);
                memoryUse += level.width_ * level.height_ * level.depth_ * 4;
                delete[] rgbaData;
            }
        }
    }
    
    SetMemoryUse(memoryUse);
    return true;
}

bool Texture3D::GetData(unsigned level, void* dest) const
{
    if (!object_)
    {
        LOGERROR("No texture created, can not get data");
        return false;
    }
    
    if (!dest)
    {
        LOGERROR("Null destination for getting data");
        return false;
    }
    
    if (level >= levels_)
    {
        LOGERROR("Illegal mip level for getting data");
        return false;
    }
    
    int levelWidth = GetLevelWidth(level);
    int levelHeight = GetLevelHeight(level);
    int levelDepth = GetLevelDepth(level);
    
    D3D11_TEXTURE3D_DESC textureDesc;
    memset(&textureDesc, 0, sizeof textureDesc);
    textureDesc.Width = levelWidth;
    textureDesc.Height = levelHeight;
    textureDesc.Depth = levelDepth;
    textureDesc.MipLevels = 1;
    textureDesc.Format = (DXGI_FORMAT)format_;
    textureDesc.Usage = D3D11_USAGE_STAGING;
    textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

    ID3D11Texture3D* stagingTexture = 0;
    graphics_->GetImpl()->GetDevice()->CreateTexture3D(&textureDesc, 0, &stagingTexture);
    if (!stagingTexture)
    {
        LOGERROR("Failed to create staging texture for GetData");
        return false;
    }

    unsigned srcSubResource = D3D11CalcSubresource(level, 0, levels_);
    D3D11_BOX srcBox;
    srcBox.left = 0;
    srcBox.right = levelWidth;
    srcBox.top = 0;
    srcBox.bottom = levelHeight;
    srcBox.front = 0;
    srcBox.back = levelDepth;
    graphics_->GetImpl()->GetDeviceContext()->CopySubresourceRegion(stagingTexture, 0, 0, 0, 0, (ID3D11Resource*)object_,
        srcSubResource, &srcBox);

    D3D11_MAPPED_SUBRESOURCE mappedData;
    mappedData.pData = 0;
    unsigned rowSize = GetRowDataSize(levelWidth);
    unsigned numRows = IsCompressed() ? (levelHeight + 3) >> 2 : levelHeight;

    graphics_->GetImpl()->GetDeviceContext()->Map((ID3D11Resource*)stagingTexture, 0, D3D11_MAP_READ, 0, &mappedData);
    if (mappedData.pData)
    {
        for (int page = 0; page < levelDepth; ++page)
        {
            for (unsigned row = 0; row < numRows; ++row)
            {
                memcpy((unsigned char*)dest + (page * numRows + row) * rowSize, (unsigned char*)mappedData.pData + page *
                    mappedData.DepthPitch + row * mappedData.RowPitch, rowSize);
            }
        }
        graphics_->GetImpl()->GetDeviceContext()->Unmap((ID3D11Resource*)stagingTexture, 0);
        stagingTexture->Release();
        return true;
    }
    else
    {
        LOGERROR("Failed to map staging texture for GetData");
        stagingTexture->Release();
        return false;
    }
}

bool Texture3D::Create()
{
    Release();
    
    if (!graphics_ || !width_ || !height_ || !depth_)
        return false;
    
    levels_ = CheckMaxLevels(width_, height_, depth_, requestedLevels_);

    D3D11_TEXTURE3D_DESC textureDesc;
    memset(&textureDesc, 0, sizeof textureDesc);
    textureDesc.Width = width_;
    textureDesc.Height = height_;
    textureDesc.Depth = depth_;
    textureDesc.MipLevels = levels_;
    textureDesc.Format = (DXGI_FORMAT)(sRGB_ ? GetSRGBFormat(format_) : format_);
    textureDesc.Usage = usage_ == TEXTURE_DYNAMIC ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = usage_ == TEXTURE_DYNAMIC ? D3D11_CPU_ACCESS_WRITE : 0;

    graphics_->GetImpl()->GetDevice()->CreateTexture3D(&textureDesc, 0, (ID3D11Texture3D**)&object_);
    if (!object_)
    {
        LOGERROR("Failed to create texture");
        return false;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
    memset(&resourceViewDesc, 0, sizeof resourceViewDesc);
    resourceViewDesc.Format = (DXGI_FORMAT)GetSRVFormat(textureDesc.Format);
    resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
    resourceViewDesc.Texture3D.MipLevels = (unsigned)levels_;

    graphics_->GetImpl()->GetDevice()->CreateShaderResourceView((ID3D11Resource*)object_, &resourceViewDesc,
        (ID3D11ShaderResourceView**)&shaderResourceView_);
    if (!shaderResourceView_)
    {
        LOGERROR("Failed to create shader resource view for texture");
        return false;
    }

    return true;
}

void Texture3D::HandleRenderSurfaceUpdate(StringHash eventType, VariantMap& eventData)
{
    if (renderSurface_ && renderSurface_->GetUpdateMode() == SURFACE_UPDATEALWAYS)
        renderSurface_->QueueUpdate();
}

}
