#include <Geode/modify/LevelCell.hpp>
#include "FREditPopup.hpp"

class $modify(FRLevelCell, LevelCell) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("LevelCell::loadFromLevel", -100);
    }

    void loadFromLevel(GJGameLevel* level) {
        LevelCell::loadFromLevel(level);
        auto difficultyContainer = m_mainLayer->getChildByID("difficulty-container");
        if (difficultyContainer) {
            auto vec = Mod::get()->getSavedValue<std::vector<FakeRateSaveData>>("fake-rate", {});
            auto it = std::find_if(vec.begin(), vec.end(), [this](auto const& item) { return item.id == m_level->m_levelID; });
            if (it != vec.end()) {
                auto fakeRateData = *it;
                auto difficultySprite = static_cast<GJDifficultySprite*>(difficultyContainer->getChildByID("difficulty-sprite"));
                difficultySprite->updateDifficultyFrame(fakeRateData.difficulty, (GJDifficultyName)0);
                difficultySprite->updateFeatureState((GJFeatureState)fakeRateData.feature);
                auto addCoins = level->m_coins > 0 && !m_compactView;
                auto showStars = fakeRateData.stars > 0 || m_level->m_dailyID > 0;
                difficultySprite->setPositionY((showStars || addCoins ? 5.0f : 0.0f) + (showStars && addCoins ? 9.0f : 0.0f) + (level->m_dailyID > 0 ? 10.0f : 0.0f));
                if (showStars) {
                    auto starsIcon = static_cast<CCSprite*>(difficultyContainer->getChildByID("stars-icon"));
                    if (!starsIcon) {
                        starsIcon = CCSprite::createWithSpriteFrameName(level->m_levelLength < 5 ? "star_small01_001.png" : "moon_small01_001.png");
                        starsIcon->setPosition({ difficultySprite->getPositionX() + 8.0f, difficultySprite->getPositionY() - 30.0f });
                        starsIcon->setID("stars-icon");
                        difficultyContainer->addChild(starsIcon);
                    }
                    auto starsLabel = static_cast<CCLabelBMFont*>(difficultyContainer->getChildByID("stars-label"));
                    if (!starsLabel) {
                        starsLabel = CCLabelBMFont::create("0", "bigFont.fnt");
                        starsLabel->setPosition({ starsIcon->getPositionX() - 8.0f, starsIcon->getPositionY() });
                        starsLabel->setScale(0.4f);
                        starsLabel->setAnchorPoint({ 1.0f, 0.5f });
                        starsLabel->setID("stars-label");
                        difficultyContainer->addChild(starsLabel);
                    }
                    starsLabel->setString(std::to_string(fakeRateData.stars).c_str());
                }
                else {
                    if (auto starsIcon = static_cast<CCSprite*>(difficultyContainer->getChildByID("stars-icon"))) starsIcon->removeFromParent();
                    if (auto starsLabel = static_cast<CCLabelBMFont*>(difficultyContainer->getChildByID("stars-label"))) starsLabel->removeFromParent();
                }
                auto gsm = GameStatsManager::sharedState();
                auto coinParent = m_compactView ? m_mainLayer : difficultyContainer;
                for (int i = 1; i <= 3; i++) {
                    if (auto coin = static_cast<CCSprite*>(coinParent->getChildByID("coin-icon-" + std::to_string(i)))) {
                        if (!m_compactView) coin->setPositionY(difficultySprite->getPositionY() - 31.5f - (showStars ? 14.0f : 0.0f)
                            - (m_level->m_gauntletLevel || m_level->m_dailyID > 0 ? 14.5f : 0.0f));
                        auto coinStr = fmt::format("{}_{}", m_level->m_levelID.value(), i);
                        if (m_level->m_dailyID > 0) coinStr += "_" + std::to_string(m_level->m_dailyID);
                        else if (m_level->m_gauntletLevel) coinStr += "_g";
                        if (gsm->hasUserCoin(coinStr.c_str()) || gsm->hasPendingUserCoin(coinStr.c_str())) coin->setColor({ 255, 255, 255 });
                        else coin->setColor({ 165, 165, 165 });
                    }
                }

                if (m_level->m_dailyID > 0) {
                    auto diamondLabel = static_cast<CCLabelBMFont*>(difficultyContainer->getChildByID("diamond-label"));
                    auto diamondIcon = static_cast<CCSprite*>(difficultyContainer->getChildByID("diamond-icon"));
                    auto diamonds = fakeRateData.stars > 1 ? fakeRateData.stars + 2 : 0;
                    diamondLabel->setString(fmt::format("{}/{}", (int)floorf(diamonds * m_level->m_orbCompletion / 100.0f), diamonds).c_str());
                    diamondIcon->setPositionX(difficultySprite->getPositionX() + diamondLabel->getScaledContentSize().width * 0.5f + 2.0f);
                    diamondLabel->setPositionX(diamondIcon->getPositionX() - 8.0f);
                }

                auto padding = (showStars ? 18.0f : 25.0f) * (m_compactView ? 0.8f : 1.0f);
                auto lengthLabel = static_cast<CCLabelBMFont*>(m_mainLayer->getChildByID("length-label"));
                auto downloadsIcon = static_cast<CCSprite*>(m_mainLayer->getChildByID("downloads-icon"));
                auto downloadsLabel = static_cast<CCLabelBMFont*>(m_mainLayer->getChildByID("downloads-label"));
                downloadsIcon->setPositionX(lengthLabel->getPositionX() + lengthLabel->getScaledContentSize().width + padding);
                downloadsLabel->setPositionX(downloadsIcon->getPositionX() + 9.0f);
                auto likesLabel = static_cast<CCLabelBMFont*>(m_mainLayer->getChildByID("likes-label"));
                auto likesIcon = static_cast<CCSprite*>(m_mainLayer->getChildByID("likes-icon"));
                likesIcon->setPositionX(downloadsLabel->getPositionX() + downloadsLabel->getScaledContentSize().width + padding);
                likesLabel->setPositionX(likesIcon->getPositionX() + 10.0f);
                if (showStars) {
                    auto orbsIcon = static_cast<CCSprite*>(m_mainLayer->getChildByID("orbs-icon"));
                    if (!orbsIcon) {
                        orbsIcon = CCSprite::createWithSpriteFrameName("currencyOrbIcon_001.png");
                        orbsIcon->setScale(m_compactView ? 0.4f : 0.6f);
                        orbsIcon->setPosition({ likesLabel->getPositionX() + likesLabel->getScaledContentSize().width + padding, likesLabel->getPositionY() });
                        orbsIcon->setID("orbs-icon");
                        m_mainLayer->addChild(orbsIcon);
                    }
                    auto orbsLabel = static_cast<CCLabelBMFont*>(m_mainLayer->getChildByID("orbs-label"));
                    if (!orbsLabel) {
                        orbsLabel = CCLabelBMFont::create("", "bigFont.fnt");
                        orbsLabel->setScale(m_compactView ? 0.3f : 0.4f);
                        orbsLabel->setAnchorPoint({ 0.0f, 0.5f });
                        orbsLabel->setPosition({ orbsIcon->getPositionX() + 10.0f, orbsIcon->getPositionY() });
                        orbsLabel->setID("orbs-label");
                        m_mainLayer->addChild(orbsLabel);
                    }
                    auto orbs = FRUtilities::getBaseCurrency(fakeRateData.stars);
                    auto totalOrbs = (int)floorf(orbs * 1.25f);
                    orbsLabel->setString((m_level->m_orbCompletion == 100 ?
                        fmt::format("{}", totalOrbs) :
                        fmt::format("{}/{}", (int)floorf(orbs * m_level->m_orbCompletion / 100.0f), totalOrbs)).c_str());
                    orbsLabel->limitLabelWidth(45.0f, m_compactView ? 0.3f : 0.4f, 0.0f);
                    if (m_level->m_orbCompletion == 100) orbsLabel->setColor({ 100, 255, 255 });
                }
                else {
                    if (auto orbsIcon = static_cast<CCSprite*>(m_mainLayer->getChildByID("orbs-icon"))) orbsIcon->removeFromParent();
                    if (auto orbsLabel = static_cast<CCLabelBMFont*>(m_mainLayer->getChildByID("orbs-label"))) orbsLabel->removeFromParent();
                }

                if (Loader::get()->isModLoaded("uproxide.more_difficulties")) fixMoreDifficultiesIncompatibility(fakeRateData);
            }
        }
    }

    void fixMoreDifficultiesIncompatibility(FakeRateSaveData const& fakeRateData) {
        auto difficultyContainer = m_mainLayer->getChildByID("difficulty-container");

        if (auto existingCasualSprite = static_cast<CCSprite*>(FRUtilities::getChildBySpriteName(difficultyContainer, "uproxide.more_difficulties/MD_Difficulty04.png")))
            existingCasualSprite->removeFromParent();
        if (auto existingToughSprite = static_cast<CCSprite*>(FRUtilities::getChildBySpriteName(difficultyContainer, "uproxide.more_difficulties/MD_Difficulty07.png")))
            existingToughSprite->removeFromParent();
        if (auto existingCruelSprite = static_cast<CCSprite*>(FRUtilities::getChildBySpriteName(difficultyContainer, "uproxide.more_difficulties/MD_Difficulty09.png")))
            existingCruelSprite->removeFromParent();

        auto difficultySprite = static_cast<GJDifficultySprite*>(difficultyContainer->getChildByID("difficulty-sprite"));
        difficultySprite->setOpacity(255);
        auto pos = CCPoint { difficultySprite->getPositionX() + 0.25f, difficultySprite->getPositionY() - 0.1f };
        switch (fakeRateData.stars) {
            case 4: {
                auto casualSprite = CCSprite::create("uproxide.more_difficulties/MD_Difficulty04.png");
                casualSprite->setPosition(pos);
                difficultyContainer->addChild(casualSprite, 3);
                difficultySprite->setOpacity(0);
                break;
            }
            case 7: {
                auto toughSprite = CCSprite::create("uproxide.more_difficulties/MD_Difficulty07.png");
                toughSprite->setPosition(pos);
                difficultyContainer->addChild(toughSprite, 3);
                difficultySprite->setOpacity(0);
                break;
            }
            case 9: {
                auto cruelSprite = CCSprite::create("uproxide.more_difficulties/MD_Difficulty09.png");
                cruelSprite->setPosition(pos);
                difficultyContainer->addChild(cruelSprite, 3);
                difficultySprite->setOpacity(0);
                break;
            }
        }
    }
};
