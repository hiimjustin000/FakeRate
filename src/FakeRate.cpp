#include "FakeRate.hpp"

int FakeRate::getBaseCurrency(int stars) {
    switch (stars) {
        case 2: return 40;
        case 3: return 60;
        case 4: return 100;
        case 5: return 140;
        case 6: return 180;
        case 7: return 220;
        case 8: return 280;
        case 9: return 340;
        case 10: return 400;
        default: return 0;
    }
}

int FakeRate::getDifficultyForStars(int stars) {
    switch (stars) {
        case 0: return 0;
        case 1: return -1;
        case 2: return 1;
        case 3: return 2;
        case 4: case 5: return 3;
        case 6: case 7: return 4;
        case 8: case 9: return 5;
        case 10: return 6;
        default: return 0;
    }
}

int FakeRate::getDifficultyFromLevel(GJGameLevel* level) {
    if (level->m_demon > 0) return level->m_demonDifficulty > 0 ? level->m_demonDifficulty + 4 : 6;
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
