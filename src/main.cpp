#include <Geode/Geode.hpp>
#include <Geode/modify/ShaderLayer.hpp>

using namespace geode::prelude;

class $modify(ShaderLayer) {
    bool init() {
        if (!ShaderLayer::init()) return false;
        float multiplier = static_cast<float>(Mod::get()->getSettingValue<double>("resolution-multiplier")
        );
        if (multiplier >= 0.99f) return true;
        if (!m_renderTexture) {
            log::warn("ShaderLayer: no render texture after init");
            return true;
        }
        CCSize screen = m_screenSize; // set by setupShader()
        if (screen.width < 1 || screen.height < 1) {
            screen = CCDirector::sharedDirector()->getWinSize();
        }
        int newW = std::max(1, static_cast<int>(screen.width  * multiplier));
        int newH = std::max(1, static_cast<int>(screen.height * multiplier));
        log::info("Shader RT: {}x{} -> {}x{} ({:.2f}x)",
            (int)screen.width, (int)screen.height, newW, newH, multiplier);
        auto fmt = m_renderTexture->getSprite()->getTexture()->getPixelFormat();
        auto newRT = CCRenderTexture::create(newW, newH, fmt);
        if (!newRT) {
            log::error("Failed to create downscaled RT😭");
            return true;
        }
        auto spr = newRT->getSprite();
        float scaleX = screen.width  / (float)newW;
        float scaleY = screen.height / (float)newH;
        spr->setScaleX(scaleX);
        spr->setScaleY(scaleY);
        spr->getTexture()->setAliasTexParameters();
        int z = m_renderTexture->getZOrder();
        this->removeChild(m_renderTexture, true);
        m_renderTexture = newRT;
        m_sprite = spr;
        this->addChild(newRT, z);
        CCSize newSize = CCSizeMake(newW, newH);
        m_textureContentSize  = newSize;
        m_targetTextureSize   = newSize;
        m_targetTextureSizeExtra = CCSizeMake(1.f, 1.f); // no extra scaling
        m_aspectRatio = (float)newW / (float)newH;
        this->setupCommonUniforms();
        return true;
    }
};