#include "FREditPopup.hpp"

#include <Geode/modify/LevelInfoLayer.hpp>
class $modify(FRLevelInfoLayer, LevelInfoLayer) {
    struct Fields {
        FakeRateSaveData m_fakeRateData;
    };

    static void onModify(auto& self) {
        (void)self.setHookPriority("LevelInfoLayer::init", -100);
        (void)self.setHookPriority("LevelInfoLayer::levelDownloadFinished", -100);
        (void)self.setHookPriority("LevelInfoLayer::levelUpdateFinished", -100);
        (void)self.setHookPriority("LevelInfoLayer::likedItem", -100);
    }

    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        auto buttonSprite = CircleButtonSprite::createWithSprite("FR_fakeRateBtn_001.png"_spr, 1.0f, CircleBaseColor::Green, CircleBaseSize::Medium);
        buttonSprite->getTopNode()->setScale(1.0f);
        auto fakeRateButton = CCMenuItemExt::createSpriteExtra(buttonSprite, [this](auto) {
            FREditPopup::create(m_level, m_fields->m_fakeRateData.stars, m_fields->m_fakeRateData.feature,
                m_fields->m_fakeRateData.difficulty, m_fields->m_fakeRateData.moreDifficultiesOverride,
                [this](int stars, int feature, int difficulty, int mdo, bool update, bool coins) {
                    updateFakeRate(stars, feature, difficulty, mdo, update, coins);
                })->show();
        });
        fakeRateButton->setID("fake-rate-button"_spr);
        auto leftSideMenu = getChildByID("left-side-menu");
        leftSideMenu->addChild(fakeRateButton);
        leftSideMenu->updateLayout();

        checkFakeRate();

        return true;
    }

    void levelDownloadFinished(GJGameLevel* level) override {
        LevelInfoLayer::levelDownloadFinished(level);
        checkFakeRate();
    }

    void levelUpdateFinished(GJGameLevel* level, UpdateResponse response) override {
        LevelInfoLayer::levelUpdateFinished(level, response);
        checkFakeRate();
    }

    void likedItem(LikeItemType type, int id, bool liked) override {
        LevelInfoLayer::likedItem(type, id, liked);
        checkFakeRate();
    }

    void checkFakeRate() {
        auto vec = Mod::get()->getSavedValue<std::vector<FakeRateSaveData>>("fake-rate", {});
        auto it = std::find_if(vec.begin(), vec.end(), [this](auto const& item) { return item.id == m_level->m_levelID; });
        auto stars = m_level->m_stars;
        if (it != vec.end()) updateFakeRate(it->stars, it->feature, it->difficulty, it->moreDifficultiesOverride, false, true);
        else m_fields->m_fakeRateData = {
            .id = m_level->m_levelID,
            .stars = stars,
            .feature = m_level->m_featured > 1 ? m_level->m_isEpic + 1 : 0,
            .difficulty = FakeRate::getDifficultyFromLevel(m_level),
            .moreDifficultiesOverride = stars == 4 || stars == 7 || stars == 9 ? stars : 0
        };
    }

    void updateFakeRate(int stars, int feature, int difficulty, int mdo, bool update, bool coins) {
        m_fields->m_fakeRateData = {
            .id = m_level->m_levelID,
            .stars = stars,
            .feature = feature,
            .difficulty = difficulty,
            .moreDifficultiesOverride = mdo
        };

        if (auto betweenDifficultySprite = static_cast<CCSprite*>(getChildByID("hiimjustin000.demons_in_between/between-difficulty-sprite"))) {
            betweenDifficultySprite->setVisible(false);
            m_difficultySprite->setOpacity(255);
        }
        auto gddpOverride = false;
        if (auto gddpDifficultySprite = static_cast<CCSprite*>(getChildByID("gddp-difficulty"))) {
            gddpOverride = gddpDifficultySprite->isVisible();
            gddpDifficultySprite->setVisible(false);
            m_difficultySprite->setOpacity(255);
        }
        if (Loader::get()->isModLoaded("itzkiba.grandpa_demon") && !gddpOverride) {
            removeChildByTag(69420);
            auto children = reinterpret_cast<CCNode**>(getChildren()->data->arr);
            for (int i = 0; i < getChildrenCount(); i++) {
                if (children[i]->getID() == "grd-difficulty") children[i]->setVisible(false);
            }
            if (auto grdInfinity = getChildByID("grd-infinity")) grdInfinity->setVisible(false);
            m_difficultySprite->setVisible(true);
            if (auto featureGlow = m_difficultySprite->getChildByTag(69420))
                featureGlow->setPosition(m_difficultySprite->getContentSize() * 0.5f);
        }
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto gsm = GameStatsManager::sharedState();
        auto showStars = stars > 0 || m_level->m_dailyID > 0 || m_level->m_gauntletLevel;
        m_difficultySprite->updateDifficultyFrame(difficulty, GJDifficultyName::Long);
        m_difficultySprite->updateFeatureState((GJFeatureState)feature);
        CCNode* nodeToSetPosition = m_difficultySprite;
        CCNode* difficultySpriteParent = m_difficultySprite->getParent();
        if (Loader::get()->isModLoaded("acaruso.horn")) {
            auto children = getChildren();
            for (int i = 0; i < children->count(); i++) {
                if (auto child = typeinfo_cast<CCMenu*>(children->objectAtIndex(i))) {
                    auto grandchildren = child->getChildren();
                    if (!grandchildren || grandchildren->count() < 1) continue;
                    if (auto button = typeinfo_cast<CCMenuItemSpriteExtra*>(grandchildren->objectAtIndex(0))) {
                        if (button->getNormalImage() == m_difficultySprite) {
                            nodeToSetPosition = child;
                            difficultySpriteParent = button;
                            break;
                        }
                    }
                }
            }
        }
        nodeToSetPosition->setPositionY(winSize.height / 2 + 56.0f + (difficulty > 5 ? 5.0f : 0.0f) + (showStars ? 10.0f : 0.0f));
        auto& position = nodeToSetPosition->getPosition();
        m_starsIcon->setPosition({ position.x + 8.0f, position.y - 30.0f - (difficulty > 5 ? 9.0f : 0.0f) });
        m_starsIcon->setVisible(showStars);
        m_starsLabel->setPosition(m_starsIcon->getPositionX() - 8.0f, m_starsIcon->getPositionY());
        m_starsLabel->setString(std::to_string(stars).c_str());
        m_starsLabel->setVisible(showStars);
        if (gsm->hasCompletedLevel(m_level)) m_starsLabel->setColor({ 255, 255, 50 });
        for (int i = 0; i < m_coins->count(); i++) {
            auto coin = static_cast<CCSprite*>(m_coins->objectAtIndex(i));
            coin->setPositionY(position.y - 31.5f - (showStars ? 14.0f : 0.0f)
                - (m_level->m_gauntletLevel || m_level->m_dailyID > 0 ? 14.5f : 0.0f) - (difficulty > 5 ? 9.0f : 0.0f));
            auto coinStr = fmt::format("{}_{}", m_level->m_levelID.value(), i + 1);
            if (m_level->m_dailyID > 0) coinStr += "_" + std::to_string(m_level->m_dailyID);
            else if (m_level->m_gauntletLevel) coinStr += "_g";
            if (coins || m_level->m_coinsVerified == 1)
                coin->setColor(gsm->hasUserCoin(coinStr.c_str()) || gsm->hasPendingUserCoin(coinStr.c_str()) ? ccColor3B { 255, 255, 255 } : ccColor3B { 165, 165, 165 });
            else coin->setColor(gsm->hasUserCoin(coinStr.c_str()) || gsm->hasPendingUserCoin(coinStr.c_str()) ? ccColor3B { 255, 175, 75 } : ccColor3B { 165, 113, 48 });
        }
        if (m_level->m_dailyID > 0 || m_level->m_gauntletLevel) {
            auto diamondLabel = static_cast<CCLabelBMFont*>(getChildByID("diamond-label"));
            auto diamondIcon = getChildByID("diamond-icon");
            auto diamonds = stars > 1 ? stars + 2 : 0;
            diamondLabel->setString(fmt::format("{}/{}", (int)floorf(diamonds * m_level->m_normalPercent / 100.0f), diamonds).c_str());
            diamondIcon->setPosition({
                position.x + diamondLabel->getScaledContentSize().width * 0.5f + 2.0f,
                position.y - (difficulty > 5 ? 9.0f : 0.0f) - 44.5f
            });
            diamondLabel->setPosition({ diamondIcon->getPositionX() - 8.0f, diamondIcon->getPositionY() });
        }
        auto yPos = winSize.height / 2 + 51.0f + (showStars ? 10.0f : 0.0f);
        auto yOffset = showStars ? 28.0f : 30.0f;
        auto downloadsIcon = static_cast<CCSprite*>(m_icons->objectAtIndex(1));
        downloadsIcon->setPositionY(yPos + yOffset);
        m_downloadsLabel->setPositionY(downloadsIcon->getPositionY());
        m_likesIcon->setPositionY(yPos + 1.0f);
        m_likesLabel->setPositionY(m_likesIcon->getPositionY());
        auto lengthIcon = static_cast<CCSprite*>(m_icons->objectAtIndex(0));
        lengthIcon->setPositionY(yPos - yOffset);
        m_lengthLabel->setPositionY(lengthIcon->getPositionY() + (m_exactLengthLabel->isVisible() ? 6.0f : 0.0f));
        m_orbsIcon->setVisible(showStars);
        m_orbsLabel->setVisible(showStars);
        if (showStars) {
            m_orbsIcon->setPositionY(yPos - yOffset * 2.0f);
            m_orbsLabel->setPositionY(m_orbsIcon->getPositionY());
            auto orbs = FakeRate::getBaseCurrency(stars);
            auto totalOrbs = (int)floorf(orbs * 1.25f);
            m_orbsLabel->setString(fmt::format("{}/{}", (int)floorf(m_level->m_normalPercent != 100 ? orbs * m_level->m_normalPercent / 100.0f : totalOrbs), totalOrbs).c_str());
            m_orbsLabel->limitLabelWidth(60.0f, 0.5f, 0.0f);
        }
        if (m_exactLengthLabel->isVisible()) m_exactLengthLabel->setPositionY(m_lengthLabel->getPositionY() - 14.0f);

        if (Loader::get()->isModLoaded("uproxide.more_difficulties")) fixMoreDifficultiesIncompatibility(difficultySpriteParent, mdo, !coins);
    }

    void fixMoreDifficultiesIncompatibility(CCNode* difficultySpriteParent, int mdo, bool remove) {
        auto spriteName = std::string();
        auto moreDifficultiesSprite = static_cast<CCSprite*>(getChildByID("uproxide.more_difficulties/more-difficulties-spr"));
        if (moreDifficultiesSprite) {
            moreDifficultiesSprite->setVisible(false);
            spriteName = FakeRate::getSpriteName(moreDifficultiesSprite);
        }
        m_difficultySprite->setOpacity(255);

        auto legacy = Loader::get()->getLoadedMod("uproxide.more_difficulties")->getSettingValue<bool>("legacy-difficulties");
        auto pos = difficultySpriteParent->convertToWorldSpace({
            m_difficultySprite->getPositionX() + (legacy ? 0.0f : 0.25f),
            m_difficultySprite->getPositionY() - (legacy ? 0.0f : 0.1f)
        });
        auto frameName = "";
        auto stars = m_fields->m_fakeRateData.stars;
        if (spriteName == "uproxide.more_difficulties/MD_DifficultyCP.png") {
            moreDifficultiesSprite->setVisible(true);
            m_difficultySprite->setOpacity(0);
        }
        else {
            auto moreDifficultiesOverride = mdo;
            auto starsRequested = m_level->m_starsRequested;
            if (remove && m_level->m_stars == 0 && (starsRequested == 4 || starsRequested == 7 || starsRequested == 9)) moreDifficultiesOverride = starsRequested;
            if (moreDifficultiesOverride == 0) return;
            switch (moreDifficultiesOverride) {
                case 4: frameName = legacy ? "uproxide.more_difficulties/MD_Difficulty04_Legacy.png" : "uproxide.more_difficulties/MD_Difficulty04.png"; break;
                case 7: frameName = legacy ? "uproxide.more_difficulties/MD_Difficulty07_Legacy.png" : "uproxide.more_difficulties/MD_Difficulty07.png"; break;
                case 9: frameName = legacy ? "uproxide.more_difficulties/MD_Difficulty09_Legacy.png" : "uproxide.more_difficulties/MD_Difficulty09.png"; break;
            }
            if (!moreDifficultiesSprite) {
                moreDifficultiesSprite = CCSprite::createWithSpriteFrameName(frameName);
                moreDifficultiesSprite->setID("uproxide.more_difficulties/more-difficulties-spr");
                addChild(moreDifficultiesSprite, 3);
            }
            else {
                moreDifficultiesSprite->initWithSpriteFrameName(frameName);
                moreDifficultiesSprite->setVisible(true);
            }
            moreDifficultiesSprite->setPosition(pos);
            m_difficultySprite->setOpacity(0);
        }
    }
};

#include <Geode/modify/LevelCell.hpp>
class $modify(FRLevelCell, LevelCell) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("LevelCell::loadFromLevel", -100);
    }

    void loadFromLevel(GJGameLevel* level) {
        LevelCell::loadFromLevel(level);
        auto difficultyContainer = m_mainLayer->getChildByID("difficulty-container");
        if (!difficultyContainer) difficultyContainer = m_mainLayer->getChildByID("grd-demon-icon-layer");
        if (difficultyContainer) {
            auto vec = Mod::get()->getSavedValue<std::vector<FakeRateSaveData>>("fake-rate", {});
            auto it = std::find_if(vec.begin(), vec.end(), [level](auto const& item) { return item.id == level->m_levelID; });
            if (it != vec.end()) {
                auto difficultySprite = static_cast<GJDifficultySprite*>(difficultyContainer->getChildByID("difficulty-sprite"));
                if (auto betweenDifficultySprite = static_cast<CCSprite*>(difficultyContainer->getChildByID("hiimjustin000.demons_in_between/between-difficulty-sprite"))) {
                    betweenDifficultySprite->setVisible(false);
                    difficultySprite->setOpacity(255);
                }
                auto gddpOverride = false;
                if (auto gddpDifficultySprite = static_cast<CCSprite*>(difficultyContainer->getChildByID("gddp-difficulty"))) {
                    gddpOverride = true;
                    gddpDifficultySprite->setVisible(false);
                    difficultySprite->setOpacity(255);
                }
                if (difficultyContainer->getID() == "grd-demon-icon-layer" && !gddpOverride) {
                    difficultyContainer->removeChildByTag(69420);
                    difficultySprite->setVisible(true);
                    if (auto grdInfinity = static_cast<CCSprite*>(difficultyContainer->getChildByID("grd-infinity"))) {
                        grdInfinity->setVisible(false);
                        static_cast<CCSprite*>(difficultyContainer->getChildren()->objectAtIndex(difficultyContainer->getChildrenCount() - 2))->setVisible(false);
                    }
                    else static_cast<CCSprite*>(difficultyContainer->getChildren()->lastObject())->setVisible(false);
                    if (auto featureGlow = difficultySprite->getChildByTag(69420))
                        featureGlow->setPosition(difficultySprite->getContentSize() * 0.5f);
                }

                auto& fakeRateData = *it;
                difficultySprite->updateDifficultyFrame(fakeRateData.difficulty, GJDifficultyName::Short);
                difficultySprite->updateFeatureState((GJFeatureState)fakeRateData.feature);
                auto addCoins = level->m_coins > 0 && !m_compactView;
                auto showStars = fakeRateData.stars > 0 || level->m_dailyID > 0;
                difficultySprite->setPositionY((showStars || addCoins ? 5.0f : 0.0f) + (showStars && addCoins ? 9.0f : 0.0f) + (level->m_dailyID > 0 ? 10.0f : 0.0f));
                auto gsm = GameStatsManager::sharedState();
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
                    if (gsm->hasCompletedLevel(level)) starsLabel->setColor({ 255, 255, 50 });
                }
                else {
                    if (auto starsIcon = static_cast<CCSprite*>(difficultyContainer->getChildByID("stars-icon"))) starsIcon->removeFromParent();
                    if (auto starsLabel = static_cast<CCLabelBMFont*>(difficultyContainer->getChildByID("stars-label"))) starsLabel->removeFromParent();
                }
                auto coinParent = m_compactView ? m_mainLayer : difficultyContainer;
                for (int i = 1; i <= 3; i++) {
                    if (auto coin = static_cast<CCSprite*>(coinParent->getChildByID("coin-icon-" + std::to_string(i)))) {
                        if (!m_compactView) coin->setPositionY(difficultySprite->getPositionY() - 31.5f - (showStars ? 14.0f : 0.0f)
                            - (level->m_gauntletLevel || level->m_dailyID > 0 ? 14.5f : 0.0f));
                        auto coinStr = fmt::format("{}_{}", level->m_levelID.value(), i);
                        if (level->m_dailyID > 0) coinStr += "_" + std::to_string(level->m_dailyID);
                        else if (level->m_gauntletLevel) coinStr += "_g";
                        coin->setColor(gsm->hasUserCoin(coinStr.c_str()) || gsm->hasPendingUserCoin(coinStr.c_str()) ? ccColor3B { 255, 255, 255 } : ccColor3B { 165, 165, 165 });
                    }
                }

                if (level->m_dailyID > 0) {
                    auto diamondLabel = static_cast<CCLabelBMFont*>(difficultyContainer->getChildByID("diamond-label"));
                    auto diamondIcon = static_cast<CCSprite*>(difficultyContainer->getChildByID("diamond-icon"));
                    auto diamonds = fakeRateData.stars > 1 ? fakeRateData.stars + 2 : 0;
                    diamondLabel->setString(fmt::format("{}/{}", (int)floorf(diamonds * level->m_normalPercent / 100.0f), diamonds).c_str());
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
                    auto orbs = FakeRate::getBaseCurrency(fakeRateData.stars);
                    auto totalOrbs = (int)floorf(orbs * 1.25f);
                    auto percent = level->m_normalPercent.value();
                    auto savedLevel = GameLevelManager::sharedState()->getSavedLevel(level->m_levelID);
                    if (savedLevel) percent = savedLevel->m_normalPercent.value();
                    orbsLabel->setString((percent == 100 ? fmt::format("{}", totalOrbs) : fmt::format("{}/{}", (int)floorf(orbs * percent / 100.0f), totalOrbs)).c_str());
                    orbsLabel->limitLabelWidth(45.0f, m_compactView ? 0.3f : 0.4f, 0.0f);
                    if (percent == 100) orbsLabel->setColor({ 100, 255, 255 });
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
        if (!difficultyContainer) difficultyContainer = m_mainLayer->getChildByID("grd-demon-icon-layer");
        auto spriteName = std::string();
        auto moreDifficultiesSprite = static_cast<CCSprite*>(difficultyContainer->getChildByID("uproxide.more_difficulties/more-difficulties-spr"));
        if (moreDifficultiesSprite) {
            moreDifficultiesSprite->setVisible(false);
            spriteName = FakeRate::getSpriteName(moreDifficultiesSprite);
        }
        auto difficultySprite = static_cast<GJDifficultySprite*>(difficultyContainer->getChildByID("difficulty-sprite"));
        difficultySprite->setOpacity(255);

        auto legacy = Loader::get()->getLoadedMod("uproxide.more_difficulties")->getSettingValue<bool>("legacy-difficulties");
        auto pos = CCPoint { difficultySprite->getPositionX() + (legacy ? 0.0f : 0.25f), difficultySprite->getPositionY() - (legacy ? 0.0f : 0.1f) };
        auto frameName = "";
        auto stars = fakeRateData.stars;
        if (spriteName == "uproxide.more_difficulties/MD_DifficultyCP.png") {
            moreDifficultiesSprite->setVisible(true);
            difficultySprite->setOpacity(0);
        }
        else if (fakeRateData.moreDifficultiesOverride > 0) {
            switch (fakeRateData.moreDifficultiesOverride) {
                case 4: frameName = legacy ? "uproxide.more_difficulties/MD_Difficulty04_Legacy.png" : "uproxide.more_difficulties/MD_Difficulty04.png"; break;
                case 7: frameName = legacy ? "uproxide.more_difficulties/MD_Difficulty07_Legacy.png" : "uproxide.more_difficulties/MD_Difficulty07.png"; break;
                case 9: frameName = legacy ? "uproxide.more_difficulties/MD_Difficulty09_Legacy.png" : "uproxide.more_difficulties/MD_Difficulty09.png"; break;
            }
            if (!moreDifficultiesSprite) {
                moreDifficultiesSprite = CCSprite::createWithSpriteFrameName(frameName);
                moreDifficultiesSprite->setID("uproxide.more_difficulties/more-difficulties-spr");
                addChild(moreDifficultiesSprite, 3);
            }
            else {
                moreDifficultiesSprite->initWithSpriteFrameName(frameName);
                moreDifficultiesSprite->setVisible(true);
            }
            moreDifficultiesSprite->setPosition(pos);
            difficultySprite->setOpacity(0);
        }
    }
};
