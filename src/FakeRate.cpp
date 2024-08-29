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
    if (name == GJDifficultyName::Long) switch (difficulty) {
        case 1: return { 0.0f, -5.0f };
        case 2: return { 0.125f, -5.0f };
        case 3: return { 0.0f, -5.0f };
        case 4: return { 0.0f, -5.125f };
        case 5: return { 0.25f, -5.0f };
        case 6: return { 0.125f, -4.75f };
        case 7: return { 0.0f, -5.0f };
        case 8: return { 0.0f, -4.125f };
        case 9: return { -0.125f, -4.125f };
        case 10: return { 0.0f, -4.0f };
        case 11: return { -0.125f, -4.125f };
        case 12: return { 0.0f, -4.125f };
        case 13: return { 0.125f, -4.125f };
        case 14: return { 0.0f, -4.125f };
        case 15: return { 0.0f, -4.125f };
        case 16: return { 0.0f, -3.625f };
        case 17: return { 0.0f, -3.625f };
        case 18: return { 0.0f, -3.5f };
        case 19: return { 0.0f, -3.5f };
        case 20: return { 0.0f, -3.5f };
    }
    else if (name == GJDifficultyName::Short) switch (difficulty) {
        case 1: return { -0.125f, -0.25f };
        case 2: return { -0.125f, -0.25f };
        case 3: return { -0.125f, -0.25f };
        case 4: return { -0.125f, -0.375f };
        case 5: return { -0.125f, -0.25f };
        case 6: return { -0.125f, -0.25f };
        case 7: return { -0.125f, -0.375f };
        case 8: return { -0.125f, 0.5f };
        case 9: return { -0.125f, 0.5f };
        case 10: return { -0.125f, 0.25f };
        case 11: return { -0.125f, 0.5f };
        case 12: return { 0.125f, 0.5f };
        case 13: return { 0.125f, 0.5f };
        case 14: return { 0.125f, 0.5f };
        case 15: return { 0.0f, 0.5f };
        case 16: return { 0.0f, 1.25f };
        case 17: return { 0.0f, 1.25f };
        case 18: return { 0.0f, 1.125f };
        case 19: return { 0.0f, 1.125f };
        case 20: return { 0.0f, 1.125f };
    }

    return { 0.0f, 0.0f };
}

int FakeRate::getGRDOverride(CCSprite* sprite) {
    auto sprName = getSpriteName(sprite);

    auto pos = sprName.find("GrD_demon");
    if (pos != std::string::npos) {
        auto num = sprName.substr(pos + 9);
        auto str = numFromString<int>(num);
        if (str.has_value()) return str.value() + 1;
        else return 0;
    }
    else return 0;
}

int FakeRate::getDIBOverride(CCSprite* sprite) {
    auto sprName = getSpriteName(sprite);

    auto pos = sprName.find("DIB_");
    if (pos != std::string::npos) {
        auto num = sprName.substr(pos + 4);
        auto str = numFromString<int>(num);
        if (str.has_value()) return str.value();
        else return 0;
    }
    else return 0;
}

int FakeRate::getGDDPOverride(CCSprite* sprite) {
    auto sprName = getSpriteName(sprite);
    if (sprName.substr(sprName.size() - 8) == "Text.png") sprName = sprName.substr(0, sprName.size() - 8);
    if (sprName.substr(sprName.size() - 5) == "Small") sprName = sprName.substr(0, sprName.size() - 5);
    if (sprName.substr(sprName.size() - 4) == "Plus") sprName = sprName.substr(0, sprName.size() - 4);

    auto pos = sprName.find("DP_");
    if (pos != std::string::npos) {
        auto num = sprName.substr(pos + 3);
        auto str = GDDP_INDICES.find(num);
        if (str != GDDP_INDICES.end()) return str->second;
        else return 0;
    }
    else return 0;
}

std::string FakeRate::getGDDPFrame(int difficulty, GJDifficultyName name) {
    auto diff = "";
    switch (difficulty) {
        case 1: diff = "Beginner"; break;
        case 2: diff = "Bronze"; break;
        case 3: diff = "Silver"; break;
        case 4: diff = "Gold"; break;
        case 5: diff = "Amber"; break;
        case 6: diff = "Platinum"; break;
        case 7: diff = "Sapphire"; break;
        case 8: diff = "Jade"; break;
        case 9: diff = "Emerald"; break;
        case 10: diff = "Ruby"; break;
        case 11: diff = "Diamond"; break;
        case 12: diff = "Onyx"; break;
        case 13: diff = "Amethyst"; break;
        case 14: diff = "Azurite"; break;
        case 15: diff = "Obsidian"; break;
        default: diff = "Beginner"; break;
    }

    return fmt::format("minemaker0430.gddp_integration/DP_{}{}Text.png", diff, name == GJDifficultyName::Short ? "Small" : "");
}
