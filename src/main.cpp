#include <Geode/Geode.hpp>
#include <Geode/modify/ShaderLayer.hpp>

using namespace geode::prelude;

class $modify(ShaderLayer) {
    bool init() {
        if (!ShaderLayer::init()) return false;
        float multiplier = Mod::get()->getSettingValue<double>("resolution-multiplier");
        if (multiplier >= 0.99f) {
            return true;
        }
        auto originalTexture = this->m_renderTexture;
        if (!originalTexture) {
            log::warn("ShaderLayer has no render texture, skipping resize");
            return true;
        }
        auto sprite = originalTexture->getSprite();
        if (!sprite) {
            log::warn("Render texture has no sprite, skipping resize");
            return true;
        }
        auto originalSize = sprite->getTexture()->getContentSize();
        int newWidth = static_cast<int>(originalSize.width * multiplier);
        int newHeight = static_cast<int>(originalSize.height * multiplier);
        // Miku sure we don't go below 1x1
        if (newWidth < 1) newWidth = 1;
        if (newHeight < 1) newHeight = 1;
        log::info("Resizing shader texture from {}x{} to {}x{} ({}x multiplier)",
                  static_cast<int>(originalSize.width), 
                  static_cast<int>(originalSize.height),
                  newWidth, 
                  newHeight,
                  multiplier);
        auto format = sprite->getTexture()->getPixelFormat();
        auto newTexture = CCRenderTexture::create(newWidth, newHeight, format);
        if (!newTexture) {
            log::error("Failed to create new render texture, keeping original");
            return true;
        }
        // replace the old render texture with the new one, we'd need to retain it first since we're replacing a reference
        newTexture->retain();
        if (originalTexture) {
            this->removeChild(originalTexture);
            originalTexture->release();
        }
        this->m_renderTexture = newTexture;
        this->addChild(newTexture);
        auto newSprite = newTexture->getSprite();
        if (newSprite) {
            this->m_sprite = newSprite;
        }
        return true;
    }
};
