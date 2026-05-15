#include <Geode/Geode.hpp>
#include <Geode/modify/ShaderLayer.hpp>

using namespace geode::prelude;

class $modify(ShaderLayer) {
    bool init() {
        if (!ShaderLayer::init()) return false;
        float multiplier = Mod::get()->getSettingValue<double>("resolution-multiplier");
        if (multiplier >= 0.99f) return true;
        CCRenderTexture* rt = nullptr;
        auto& children = *this->getChildren();
        for (int i = 0; i < this->getChildrenCount(); i++) {
            rt = dynamic_cast<CCRenderTexture*>(children.objectAtIndex(i));
            if (rt) break;
        }
        if (!rt) {
            log::warn("ShaderLayer: could not find CCRenderTexture child, skipping");
            return true;
        }
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        int newW = std::max(1, static_cast<int>(winSize.width  * multiplier));
        int newH = std::max(1, static_cast<int>(winSize.height * multiplier));
        log::info("Shader resolution: {}x{} -> {}x{} (multiplier {})",
            static_cast<int>(winSize.width), static_cast<int>(winSize.height),
            newW, newH, multiplier);
        auto format = rt->getSprite()->getTexture()->getPixelFormat();
        auto newRT = CCRenderTexture::create(newW, newH, format);
        if (!newRT) {
            log::error("Failed to create scaled render texture");
            return true;
        }
        // pos new RT sprite to fill the full screen (upscale via sprite scale)
        auto sprite = newRT->getSprite();
        sprite->setScaleX(winSize.width  / newW);
        sprite->setScaleY(winSize.height / newH);
        // disable antialias so the low-res look is crisp (imma remove if fucked)
        sprite->getTexture()->setAliasTexParameters();
        // remove old RT, add new one in same z-order
        int z = rt->getZOrder();
        this->removeChild(rt, true);
        this->addChild(newRT, z);
        return true;
    }
};
