#include "FRUtilities.hpp"

int FRUtilities::getBaseCurrency(int stars) {
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

CCNode* FRUtilities::getChildBySpriteName(CCNode* parent, const char* name) {
    for (auto child : CCArrayExt<CCNode*>(parent->getChildren())) {
        if (isSpriteName(static_cast<CCNode*>(child), name)) return child;
    }
    return nullptr;
}

int FRUtilities::getDifficultyForStars(int stars) {
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

int FRUtilities::getDifficultyFromLevel(GJGameLevel* level) {
    auto difficulty = level->getAverageDifficulty();
    if (level->m_demon > 0) switch (level->m_demonDifficulty) {
        case 3: difficulty = 7; break;
        case 4: difficulty = 8; break;
        case 5: difficulty = 9; break;
        case 6: difficulty = 10; break;
        default: difficulty = 6; break;
    }
    return difficulty;
}

bool FRUtilities::isSpriteName(CCNode* node, const char* name) {
    if (!node) return false;

    auto texture = CCTextureCache::sharedTextureCache()->textureForKey(name);
    if (!texture) return false;

    if (auto* spr = typeinfo_cast<CCSprite*>(node)) {
        if (spr->getTexture() == texture) return true;
    }
    else if (auto* btn = typeinfo_cast<CCMenuItemSprite*>(node)) {
        auto* img = btn->getNormalImage();
        if (auto* spr = typeinfo_cast<CCSprite*>(img)) {
            if (spr->getTexture() == texture) return true;
        }
    }
    return false;
}