#include "../FakeRate.hpp"
#include "../classes/FREffects.hpp"

using namespace geode::prelude;

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
            auto it = std::find_if(vec.begin(), vec.end(), [level](FakeRateSaveData const& item) { return item.id == level->m_levelID; });
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
                    if (FakeRate::getSpriteName(gddpDifficultySprite).find("Plus") != std::string::npos) {
                        if (auto epicSprite = getChildBySpriteFrameName(difficultySprite, "GJ_epicCoin_001.png"))
                            epicSprite->setVisible(!Loader::get()->isModLoaded("uproxide.animated_fire"));
                    }
                    difficultySprite->setOpacity(255);
                }
                if (difficultyContainer->getID() == "grd-demon-icon-layer" && !gddpOverride) {
                    difficultyContainer->removeChildByTag(69420);
                    if (Loader::get()->isModLoaded("uproxide.animated_fire")) difficultyContainer->removeChildByTag(69420);
                    difficultySprite->setVisible(true);
                    if (auto grdInfinity = static_cast<CCSprite*>(difficultyContainer->getChildByID("grd-infinity"))) {
                        grdInfinity->setVisible(false);
                        static_cast<CCSprite*>(difficultyContainer->getChildren()->objectAtIndex(difficultyContainer->getChildrenCount() - 2))->setVisible(false);
                    }
                    else static_cast<CCSprite*>(difficultyContainer->getChildren()->lastObject())->setVisible(false);
                    if (auto featureGlow = difficultySprite->getChildByTag(69420))
                        featureGlow->setPosition(difficultySprite->getContentSize() / 2);
                }

                auto& fakeRateData = *it;
                difficultySprite->updateFeatureState((GJFeatureState)fakeRateData.feature);
                difficultySprite->updateDifficultyFrame(fakeRateData.difficulty, GJDifficultyName::Short);
                auto addCoins = level->m_coins > 0 && !m_compactView;
                auto showStars = fakeRateData.stars != 0 || level->m_dailyID > 0;
                difficultySprite->setPositionY((showStars || addCoins ? 5.0f : 0.0f) + (showStars && addCoins ? 9.0f : 0.0f) + (level->m_dailyID > 0 ? 10.0f : 0.0f));
                auto gsm = GameStatsManager::get();
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
                        coin->setColor(gsm->hasUserCoin(coinStr.c_str()) || gsm->hasPendingUserCoin(coinStr.c_str()) ?
                            ccColor3B { 255, 255, 255 } : ccColor3B { 165, 165, 165 });
                    }
                }

                if (level->m_dailyID > 0) {
                    auto diamondLabel = static_cast<CCLabelBMFont*>(difficultyContainer->getChildByID("diamond-label"));
                    auto diamondIcon = static_cast<CCSprite*>(difficultyContainer->getChildByID("diamond-icon"));
                    auto diamonds = fakeRateData.stars > 1 ? fakeRateData.stars + 2 : 0;
                    diamondLabel->setString(fmt::format("{}/{}", (int)floorf(diamonds * level->m_normalPercent / 100.0f), diamonds).c_str());
                    diamondIcon->setPositionX(difficultySprite->getPositionX() + diamondLabel->getScaledContentWidth() * 0.5f + 2.0f);
                    diamondLabel->setPositionX(diamondIcon->getPositionX() - 8.0f);
                }

                auto padding = (showStars ? 18.0f : 25.0f) * (m_compactView ? 0.8f : 1.0f);
                auto lengthLabel = static_cast<CCLabelBMFont*>(m_mainLayer->getChildByID("length-label"));
                auto downloadsIcon = static_cast<CCSprite*>(m_mainLayer->getChildByID("downloads-icon"));
                auto downloadsLabel = static_cast<CCLabelBMFont*>(m_mainLayer->getChildByID("downloads-label"));
                downloadsIcon->setPositionX(lengthLabel->getPositionX() + lengthLabel->getScaledContentWidth() + padding);
                downloadsLabel->setPositionX(downloadsIcon->getPositionX() + 9.0f);
                auto likesLabel = static_cast<CCLabelBMFont*>(m_mainLayer->getChildByID("likes-label"));
                auto likesIcon = static_cast<CCSprite*>(m_mainLayer->getChildByID("likes-icon"));
                likesIcon->setPositionX(downloadsLabel->getPositionX() + downloadsLabel->getScaledContentWidth() + padding);
                likesLabel->setPositionX(likesIcon->getPositionX() + 10.0f);
                if (showStars) {
                    auto orbsIcon = static_cast<CCSprite*>(m_mainLayer->getChildByID("orbs-icon"));
                    if (!orbsIcon) {
                        orbsIcon = CCSprite::createWithSpriteFrameName("currencyOrbIcon_001.png");
                        orbsIcon->setScale(m_compactView ? 0.4f : 0.6f);
                        orbsIcon->setPosition({ likesLabel->getPositionX() + likesLabel->getScaledContentWidth() + padding, likesLabel->getPositionY() });
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
                    int totalOrbs = floorf(orbs * 1.25f);
                    auto percent = level->m_normalPercent.value();
                    auto savedLevel = GameLevelManager::get()->getSavedLevel(level);
                    if (savedLevel) percent = savedLevel->m_normalPercent.value();
                    orbsLabel->setString((percent == 100 ? fmt::format("{}", totalOrbs) : fmt::format("{}/{}", (int)floorf(orbs * percent / 100.0f), totalOrbs)).c_str());
                    orbsLabel->limitLabelWidth(45.0f, m_compactView ? 0.3f : 0.4f, 0.0f);
                    if (percent == 100) orbsLabel->setColor({ 100, 255, 255 });
                }
                else {
                    if (auto orbsIcon = static_cast<CCSprite*>(m_mainLayer->getChildByID("orbs-icon"))) orbsIcon->removeFromParent();
                    if (auto orbsLabel = static_cast<CCLabelBMFont*>(m_mainLayer->getChildByID("orbs-label"))) orbsLabel->removeFromParent();
                }

                auto& position = difficultySprite->getPosition();
                auto gdo = fakeRateData.grandpaDemonOverride;
                auto dbo = fakeRateData.demonsInBetweenOverride;
                auto gio = fakeRateData.gddpIntegrationOverride;

                if (Loader::get()->isModLoaded("uproxide.more_difficulties")) fixMoreDifficultiesIncompatibility(fakeRateData.moreDifficultiesOverride, gdo, dbo, gio);

                if (Loader::get()->isModLoaded("itzkiba.grandpa_demon") && gdo > 0 && gdo < 7) {
                    auto grdSprite = CCSprite::createWithSpriteFrameName(fmt::format("itzkiba.grandpa_demon/GrD_demon{}.png", gdo - 1).c_str());
                    grdSprite->setID("grandpa-demon-sprite"_spr);
                    grdSprite->setPosition(position);
                    difficultyContainer->addChild(grdSprite, 3);
                    if (gdo == 5) {
                        auto grdInfinity = FREffects::grdInfinity();
                        grdInfinity->setID("grandpa-demon-infinity"_spr);
                        grdInfinity->setPosition(position + CCPoint { -0.4f, 14.0f });
                        difficultyContainer->addChild(grdInfinity);
                    }
                    difficultySprite->setOpacity(0);
                }

                if (Loader::get()->isModLoaded("hiimjustin000.demons_in_between") && dbo > 0 && dbo < 21) {
                    auto demonsInBetween = Loader::get()->getLoadedMod("hiimjustin000.demons_in_between");
                    auto dibFeature = "";
                    if (fakeRateData.feature == 3 && demonsInBetween->getSettingValue<bool>("enable-legendary")) dibFeature = "_4";
                    else if (fakeRateData.feature == 4 && demonsInBetween->getSettingValue<bool>("enable-mythic")) dibFeature = "_5";
                    auto dibSprite = CCSprite::createWithSpriteFrameName(fmt::format("hiimjustin000.demons_in_between/DIB_{:02d}{}_btn_001.png", dbo, dibFeature).c_str());
                    dibSprite->setID("between-difficulty-sprite"_spr);
                    dibSprite->setPosition(position + FakeRate::getDIBOffset(dbo, GJDifficultyName::Short));
                    difficultyContainer->addChild(dibSprite, 3);
                    difficultySprite->setOpacity(0);
                }

                if (Loader::get()->isModLoaded("minemaker0430.gddp_integration") && gio > 0 && gio < 16) {
                    auto gddpSprite = CCSprite::createWithSpriteFrameName(FakeRate::getGDDPFrame(gio, GJDifficultyName::Short).c_str());
                    gddpSprite->setID("gddp-difficulty"_spr);
                    gddpSprite->setPosition(position + CCPoint { 0.5f, 0.0f });
                    difficultyContainer->addChild(gddpSprite, 3);
                    difficultySprite->setOpacity(0);
                }
            }
        }
    }

    void fixMoreDifficultiesIncompatibility(int mdo, int gdo, int dbo, int gio) {
        auto difficultyContainer = m_mainLayer->getChildByID("difficulty-container");
        if (!difficultyContainer) difficultyContainer = m_mainLayer->getChildByID("grd-demon-icon-layer");
        auto moreDifficultiesSprite = static_cast<CCSprite*>(difficultyContainer->getChildByID("uproxide.more_difficulties/more-difficulties-spr"));
        if (moreDifficultiesSprite) moreDifficultiesSprite->setVisible(false);

        auto difficultySprite = static_cast<GJDifficultySprite*>(difficultyContainer->getChildByID("difficulty-sprite"));
        difficultySprite->setOpacity(255);
        if ((mdo != 4 && mdo != 7 && mdo != 9) || gdo > 0 || dbo > 0 || gio > 0) return;
        difficultySprite->setOpacity(0);

        auto legacy = Loader::get()->getLoadedMod("uproxide.more_difficulties")->getSettingValue<bool>("legacy-difficulties");
        if (!moreDifficultiesSprite) {
            moreDifficultiesSprite = CCSprite::createWithSpriteFrameName(fmt::format("uproxide.more_difficulties/MD_Difficulty{:02d}{}.png",
                mdo, legacy ? "_Legacy" : "").c_str());
            moreDifficultiesSprite->setID("uproxide.more_difficulties/more-difficulties-spr");
            difficultyContainer->addChild(moreDifficultiesSprite, 3);
        }
        else {
            moreDifficultiesSprite->setDisplayFrame(CCSpriteFrameCache::get()->spriteFrameByName(
                fmt::format("uproxide.more_difficulties/MD_Difficulty{:02d}{}.png", mdo, legacy ? "_Legacy" : "").c_str()));
            moreDifficultiesSprite->setVisible(true);
        }

        moreDifficultiesSprite->setPosition({
            difficultySprite->getPositionX() + (legacy ? 0.0f : 0.25f),
            difficultySprite->getPositionY() - (legacy ? 0.0f : 0.1f)
        });
    }
};
