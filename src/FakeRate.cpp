#include "FakeRate.hpp"

int FakeRate::getBaseCurrency(int stars) {
    switch (stars) {
        case 0: case 1: return 0;
        case 2: return 40;
        case 3: return 60;
        case 4: return 100;
        case 5: return 140;
        case 6: return 180;
        case 7: return 220;
        case 8: return 280;
        case 9: return 340;
        default: return 400;
    }
}

int FakeRate::getDifficultyFromLevel(GJGameLevel* level) {
    if (level->m_demon > 0) return level->m_demonDifficulty > 0 ? level->m_demonDifficulty + 4 : 6;
    else if (level->m_autoLevel) return -1;
    else if (level->m_ratings < 5) return 0;
    else return level->m_ratingsSum / level->m_ratings;
}

std::string FakeRate::getSpriteName(CCSprite* sprite) {
    if (auto texture = sprite->getTexture()) {
        for (auto [key, frame] : CCDictionaryExt<std::string, CCSpriteFrame*>(CCSpriteFrameCache::sharedSpriteFrameCache()->m_pSpriteFrames)) {
            if (frame->getTexture() == texture && frame->getRect() == sprite->getTextureRect()) return key;
        }
        for (auto [key, obj] : CCDictionaryExt<std::string, CCTexture2D*>(CCTextureCache::sharedTextureCache()->m_pTextures)) {
            if (obj == texture) return key;
        }
    }
    return "";
}

void FakeRate::toggle(CCNode* node, bool enabled) {
    if (auto sprite = typeinfo_cast<CCSprite*>(node)) {
        auto color = enabled ? ccColor3B { 255, 255, 255 } : ccColor3B { 125, 125, 125 };
        sprite->setColor(color);

        if (sprite->getChildren()) {
            auto children = sprite->getChildren();
            for (int i = 0; i < children->count(); i++) {
                if (auto child = typeinfo_cast<CCSprite*>(children->objectAtIndex(i))) child->setColor(color);
            }
        }
    }
}

CCPoint FakeRate::getDIBOffset(int difficulty, GJDifficultyName name) {
    if (difficulty < 1 || difficulty > 20) return { 0.0f, 0.0f };
    return name == GJDifficultyName::Long ? LONG_OFFSETS[difficulty - 1] : SHORT_OFFSETS[difficulty - 1];
}

int FakeRate::getGRDOverride(CCSprite* sprite) {
    auto sprName = getSpriteName(sprite);
    if (sprName.substr(sprName.size() - 5) == "_text") sprName = sprName.substr(0, sprName.size() - 5);

    auto pos = sprName.find("GrD_demon");
    if (pos != std::string::npos) {
        auto num = sprName.substr(pos + 9);
        return std::stoi(num) + 1;
    }
    else return 0;
}

int FakeRate::getDIBOverride(CCSprite* sprite) {
    auto sprName = getSpriteName(sprite);
    if (sprName.substr(sprName.size() - 12) == "_btn_001.png") sprName = sprName.substr(0, sprName.size() - 12);
    else if (sprName.substr(sprName.size() - 13) == "_btn2_001.png") sprName = sprName.substr(0, sprName.size() - 13);

    auto pos = sprName.find("DIB_");
    if (pos != std::string::npos) {
        auto num = sprName.substr(pos + 4, 2);
        return std::stoi(num);
    }
    else return 0;
}
