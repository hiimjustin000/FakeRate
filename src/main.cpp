#include "FREditPopup.hpp"

#include <Geode/modify/LevelInfoLayer.hpp>
class $modify(FRLevelInfoLayer, LevelInfoLayer) {
    struct Fields {
        FakeRateSaveData m_fakeRateData;
        CCSprite* m_grandpaBackground1;
        CCSprite* m_grandpaBackground2;
        CCParticleSystemQuad* m_grandpaParticles1;
        CCParticleSystemQuad* m_grandpaParticles2;
        ccColor3B m_backgroundColor;
        bool m_fakeRateApplied;
    };

    static void onModify(auto& self) {
        (void)self.setHookPriority("LevelInfoLayer::init", -100);
        (void)self.setHookPriority("LevelInfoLayer::levelDownloadFinished", -100);
        (void)self.setHookPriority("LevelInfoLayer::levelUpdateFinished", -100);
        (void)self.setHookPriority("LevelInfoLayer::likedItem", -100);
    }

    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        m_fields->m_backgroundColor = static_cast<CCSprite*>(getChildByID("background"))->getColor();

        auto buttonSprite = CircleButtonSprite::createWithSprite("FR_fakeRateBtn_001.png"_spr, 1.0f, CircleBaseColor::Green, CircleBaseSize::Medium);
        buttonSprite->getTopNode()->setScale(1.0f);
        auto fakeRateButton = CCMenuItemExt::createSpriteExtra(buttonSprite, [this](auto) {
            FREditPopup::create(m_level, m_fields->m_fakeRateData, [this](FakeRateSaveData data, bool remove) { updateFakeRate(data, remove); })->show();
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
        auto it = std::find_if(vec.begin(), vec.end(), [this](FakeRateSaveData const& item) { return item.id == m_level->m_levelID; });
        auto stars = m_level->m_stars.value();
        auto starsRequested = m_level->m_starsRequested;
        auto grandpaDemon = static_cast<CCSprite*>(getChildByID("grd-difficulty"));
        auto demonInBetween = static_cast<CCSprite*>(getChildByID("hiimjustin000.demons_in_between/between-difficulty-sprite"));
        auto gddpDifficulty = static_cast<CCSprite*>(getChildByID("gddp-difficulty"));
        auto gddpOverride = Loader::get()->isModLoaded("minemaker0430.gddp_integration") ?
            Loader::get()->getLoadedMod("minemaker0430.gddp_integration")->getSettingValue<bool>("override-grandpa-demon") : false;
        if (it != vec.end()) updateFakeRate(*it, false);
        else m_fields->m_fakeRateData = {
            .id = m_level->m_levelID,
            .stars = stars,
            .feature = m_level->m_featured > 1 ? m_level->m_isEpic + 1 : 0,
            .difficulty = FakeRate::getDifficultyFromLevel(m_level),
            .moreDifficultiesOverride = Loader::get()->isModLoaded("uproxide.more_difficulties") ? stars == 4 || stars == 7 || stars == 9 ? stars :
                stars == 0 && (starsRequested == 4 || starsRequested == 7 || starsRequested == 9) ? starsRequested : 0 : 0,
            .grandpaDemonOverride = grandpaDemon && (!gddpOverride || !gddpDifficulty) ? FakeRate::getGRDOverride(grandpaDemon) : 0,
            .demonsInBetweenOverride = demonInBetween ? FakeRate::getDIBOverride(demonInBetween) : 0,
            .gddpIntegrationOverride = gddpDifficulty && (!grandpaDemon || gddpOverride) && !demonInBetween ? FakeRate::getGDDPOverride(gddpDifficulty) : 0,
            .coins = m_level->m_coinsVerified > 0
        };
    }

    void updateFakeRate(FakeRateSaveData data, bool remove) {
        auto stars = data.stars;
        auto feature = data.feature;
        auto difficulty = data.difficulty;
        auto mdo = data.moreDifficultiesOverride;
        auto gdo = data.grandpaDemonOverride;
        auto dbo = data.demonsInBetweenOverride;
        auto gio = data.gddpIntegrationOverride;
        auto coins = data.coins;
        auto& fields = m_fields;
        fields->m_fakeRateData = {
            .id = m_level->m_levelID,
            .stars = stars,
            .feature = feature,
            .difficulty = difficulty,
            .moreDifficultiesOverride = mdo,
            .grandpaDemonOverride = gdo,
            .demonsInBetweenOverride = dbo,
            .gddpIntegrationOverride = gio,
            .coins = coins
        };

        if (!fields->m_fakeRateApplied) {
            fields->m_fakeRateApplied = true;
            fields->m_backgroundColor = static_cast<CCSprite*>(getChildByID("background"))->getColor();
            auto gdutils = Loader::get()->getLoadedMod("gdutilsdevs.gdutils");
            if (gdutils && gdutils->getSettingValue<bool>("activate-background"))
                fields->m_backgroundColor = gdutils->getSettingValue<ccColor3B>("background");
        }

        fields->m_grandpaBackground1 = static_cast<CCSprite*>(getChildByID("grandpa-background-1"_spr));
        fields->m_grandpaBackground2 = static_cast<CCSprite*>(getChildByID("grandpa-background-2"_spr));
        fields->m_grandpaParticles1 = static_cast<CCParticleSystemQuad*>(getChildByID("grandpa-particles-1"_spr));
        fields->m_grandpaParticles2 = static_cast<CCParticleSystemQuad*>(getChildByID("grandpa-particles-2"_spr));

        auto hide = false;
        if (auto betweenDifficultySprite = static_cast<CCSprite*>(getChildByID("hiimjustin000.demons_in_between/between-difficulty-sprite"))) {
            betweenDifficultySprite->setVisible(remove);
            m_difficultySprite->setOpacity(remove ? 0 : 255);
            hide = remove || hide;
            fields->m_fakeRateData.demonsInBetweenOverride = remove ? FakeRate::getDIBOverride(betweenDifficultySprite) : dbo;
        }
        auto gddpOverride = false;
        auto gddpIntegration = Loader::get()->getLoadedMod("minemaker0430.gddp_integration");
        if (auto gddpDifficultySprite = static_cast<CCSprite*>(getChildByID("gddp-difficulty"))) {
            gddpOverride = gddpIntegration ? gddpIntegration->getSettingValue<bool>("override-grandpa-demon") : false;
            gddpDifficultySprite->setVisible(remove);
            m_difficultySprite->setOpacity(remove ? 0 : 255);
            if (FakeRate::getSpriteName(gddpDifficultySprite).find("Plus") != std::string::npos) {
                if (auto epicSprite = getChildBySpriteFrameName(m_difficultySprite, "GJ_epicCoin_001.png"))
                    epicSprite->setVisible(!Loader::get()->isModLoaded("uproxide.animated_fire") && !remove);
            }
            hide = remove || hide;
            fields->m_fakeRateData.gddpIntegrationOverride = remove ? FakeRate::getGDDPOverride(gddpDifficultySprite) : gio;
        }
        auto hasEffects = false;
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        if (Loader::get()->isModLoaded("itzkiba.grandpa_demon") && !gddpOverride) {
            removeChildByTag(69420);
            auto hasDemon = false;
            for (auto child : CCArrayExt<CCNode*>(getChildren())) {
                if (child->getID() == "grd-difficulty") {
                    child->setVisible(remove);
                    hasDemon = true;
                }

                auto bgNum = 0;
                if (isSpriteName(child, "itzkiba.grandpa_demon/GrD_demon4_bg.png") &&
                    child != fields->m_grandpaBackground1 && child != fields->m_grandpaBackground2) {
                    if (child->getID() == "") child->setID(fmt::format("grd-bg-{}", ++bgNum));
                    if (child->getID() == "grd-bg-1") child->setVisible(remove);
                    else if (child->getID() == "grd-bg-2") child->setVisible(remove);
                    hasDemon = true;
                    hasEffects = true;
                }

                auto particleNum = 0;
                if (typeinfo_cast<CCParticleSystemQuad*>(child) && child->getPositionY() == winSize.height / 2 + 76.0f &&
                    child != fields->m_grandpaParticles1 && child != fields->m_grandpaParticles2) {
                    if (child->getID() == "") child->setID(fmt::format("grd-particles-{}", ++particleNum));
                    if (child->getID() == "grd-particles-1") child->setVisible(remove);
                    else if (child->getID() == "grd-particles-2") child->setVisible(remove);
                    hasDemon = true;
                    hasEffects = true;
                }
            }
            if (auto grdInfinity = getChildByID("grd-infinity")) grdInfinity->setVisible(remove);
            m_difficultySprite->setVisible(true);
            m_difficultySprite->setOpacity(hasDemon && remove ? 0 : 255);
            hide = (hasDemon && remove) || hide;
            if (auto featureGlow = m_difficultySprite->getChildByTag(69420))
                featureGlow->setPosition(m_difficultySprite->getContentSize() / 2);
            fields->m_fakeRateData.grandpaDemonOverride = hasDemon && remove ? FakeRate::getGRDOverride(static_cast<CCSprite*>(getChildByID("grd-difficulty"))) : gdo;
        }
        auto gsm = GameStatsManager::sharedState();
        auto showStars = stars != 0 || m_level->m_dailyID > 0 || m_level->m_gauntletLevel;
        m_difficultySprite->updateFeatureState((GJFeatureState)feature);
        m_difficultySprite->updateDifficultyFrame(difficulty, GJDifficultyName::Long);
        auto isDemon = difficulty > 5 || gdo > 0 || dbo > 0 || gio > 0;
        m_difficultySprite->setPositionY(winSize.height / 2 + 56.0f + (isDemon ? 5.0f : 0.0f) + (showStars ? 10.0f : 0.0f));
        auto& position = m_difficultySprite->getPosition();
        m_starsIcon->setPosition({ position.x + 8.0f, position.y - 30.0f - (isDemon ? 9.0f : 0.0f) });
        m_starsIcon->setVisible(showStars);
        m_starsLabel->setPosition(m_starsIcon->getPositionX() - 8.0f, m_starsIcon->getPositionY());
        m_starsLabel->setString(std::to_string(stars).c_str());
        m_starsLabel->setVisible(showStars);
        if (gsm->hasCompletedLevel(m_level)) m_starsLabel->setColor({ 255, 255, 50 });
        for (int i = 0; i < m_coins->count(); i++) {
            auto coin = static_cast<CCSprite*>(m_coins->objectAtIndex(i));
            coin->setPositionY(position.y - 31.5f - (showStars ? 14.0f : 0.0f)
                - (m_level->m_gauntletLevel || m_level->m_dailyID > 0 ? 14.5f : 0.0f) - (isDemon ? 9.0f : 0.0f));
            auto coinStr = fmt::format("{}_{}", m_level->m_levelID.value(), i + 1);
            if (m_level->m_dailyID > 0) coinStr += "_" + std::to_string(m_level->m_dailyID);
            else if (m_level->m_gauntletLevel) coinStr += "_g";
            auto hasCoin = gsm->hasUserCoin(coinStr.c_str()) || gsm->hasPendingUserCoin(coinStr.c_str());
            if (coins) coin->setColor(hasCoin ? ccColor3B { 255, 255, 255 } : ccColor3B { 165, 165, 165 });
            else coin->setColor(hasCoin ? ccColor3B { 255, 175, 75 } : ccColor3B { 165, 113, 48 });
        }
        if (m_level->m_dailyID > 0 || m_level->m_gauntletLevel) {
            auto diamondLabel = static_cast<CCLabelBMFont*>(getChildByID("diamond-label"));
            auto diamondIcon = getChildByID("diamond-icon");
            auto diamonds = stars > 1 ? stars + 2 : 0;
            diamondLabel->setString(fmt::format("{}/{}", (int)floorf(diamonds * m_level->m_normalPercent / 100.0f), diamonds).c_str());
            diamondIcon->setPosition({
                position.x + diamondLabel->getScaledContentWidth() * 0.5f + 2.0f,
                position.y - (isDemon ? 9.0f : 0.0f) - 44.5f
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
            int totalOrbs = floorf(orbs * 1.25f);
            m_orbsLabel->setString(fmt::format("{}/{}",
                (int)floorf(m_level->m_normalPercent != 100 ? orbs * m_level->m_normalPercent / 100.0f : totalOrbs), totalOrbs).c_str());
            m_orbsLabel->limitLabelWidth(60.0f, 0.5f, 0.0f);
        }
        if (m_exactLengthLabel->isVisible()) m_exactLengthLabel->setPositionY(m_lengthLabel->getPositionY() - 14.0f);

        m_difficultySprite->setOpacity(hide ? 0 : 255);

        if (Loader::get()->isModLoaded("uproxide.more_difficulties")) fixMoreDifficultiesIncompatibility(mdo, remove, hide, gdo, dbo, gio);

        auto grandpaDemon = Loader::get()->getLoadedMod("itzkiba.grandpa_demon");
        auto showBackground = (hasEffects && remove) || (gdo > 2 && (grandpaDemon ? !grandpaDemon->getSettingValue<bool>("infinite-demon-disable") : false));
        auto background = static_cast<CCSprite*>(getChildByID("background"));
        background->setZOrder(showBackground ? -10 : -2);
        background->setOpacity(showBackground ? 50 : 255);
        background->setColor((gddpOverride && remove) || (gio > 0 && (gddpIntegration ? !gddpIntegration->getSettingValue<bool>("restore-bg-color") : false)) ?
            ccColor3B { 18, 18, 86 } : fields->m_backgroundColor);
        static_cast<CCSprite*>(getChildByID("bottom-left-art"))->setOpacity(showBackground ? 50 : 255);
        static_cast<CCSprite*>(getChildByID("bottom-right-art"))->setOpacity(showBackground ? 50 : 255);

        if (Loader::get()->isModLoaded("itzkiba.grandpa_demon") && gdo > 0 && gdo < 7) {
            if (auto fakeGrandpaDemon = static_cast<CCSprite*>(getChildByID("grandpa-demon-sprite"_spr))) {
                fakeGrandpaDemon->removeFromParent();
                if (auto fakeGrandpaInfinity = static_cast<CCSprite*>(getChildByID("grandpa-demon-infinity"_spr))) fakeGrandpaInfinity->removeFromParent();
            }

            auto grdSprite = CCSprite::createWithSpriteFrameName(fmt::format("itzkiba.grandpa_demon/GrD_demon{}_text.png", gdo - 1).c_str());
            grdSprite->setID("grandpa-demon-sprite"_spr);
            grdSprite->setPosition(position);
            addChild(grdSprite, 13);
            if (gdo == 5) {
                auto grdInfinity = FREffects::grdInfinity();
                grdInfinity->setID("grandpa-demon-infinity"_spr);
                grdInfinity->setPosition(position + CCPoint { -0.4f, 14.0f });
                addChild(grdInfinity);
            } else if (auto grdInfinity = getChildByID("grandpa-demon-infinity"_spr)) grdInfinity->removeFromParent();

            if (fields->m_grandpaBackground1) fields->m_grandpaBackground1->removeFromParent();
            if (fields->m_grandpaBackground2) fields->m_grandpaBackground2->removeFromParent();
            if (fields->m_grandpaParticles1) fields->m_grandpaParticles1->removeFromParent();
            if (fields->m_grandpaParticles2) fields->m_grandpaParticles2->removeFromParent();

            if (showBackground) {
                switch (gdo) {
                    case 3: {
                        fields->m_grandpaBackground1 = FREffects::legendaryBackground();
                        fields->m_grandpaBackground1->setID("grandpa-background-1"_spr);
                        addChild(fields->m_grandpaBackground1);
                        break;
                    }
                    case 4: {
                        fields->m_grandpaBackground1 = FREffects::mythicalBackground();
                        fields->m_grandpaBackground1->setID("grandpa-background-1"_spr);
                        addChild(fields->m_grandpaBackground1);
                        break;
                    }
                    case 5: case 6: {
                        auto backgrounds = FREffects::infinityBackgrounds(gdo == 6);
                        fields->m_grandpaBackground1 = backgrounds.first;
                        fields->m_grandpaBackground1->setID("grandpa-background-1"_spr);
                        addChild(fields->m_grandpaBackground1);
                        fields->m_grandpaBackground2 = backgrounds.second;
                        fields->m_grandpaBackground2->setID("grandpa-background-2"_spr);
                        addChild(fields->m_grandpaBackground2);
                        break;
                    }
                }
            }

            if (!grandpaDemon->getSettingValue<bool>("particles-disable") && gdo > 2) {
                auto particles1 = "";
                auto particles2 = "";
                switch (gdo) {
                    case 3: particles1 = "legendaryEffect.plist"_spr; break;
                    case 4: particles1 = "mythicalEffect.plist"_spr; break;
                    case 5: particles1 = "infiniteEffect.plist"_spr; particles2 = "infiniteSunEffect.plist"_spr; break;
                    case 6: particles1 = "grandpaEffect.plist"_spr; particles2 = "infiniteSunEffect.plist"_spr; break;
                }

                auto particlePos = position + CCPoint { 0.0f, 5.0f };
                if (strlen(particles1) > 0) {
                    fields->m_grandpaParticles1 = CCParticleSystemQuad::create();
                    fields->m_grandpaParticles1->initWithDictionary(CCDictionary::createWithContentsOfFileThreadSafe(particles1), false);
                    fields->m_grandpaParticles1->setPosition(particlePos);
                    fields->m_grandpaParticles1->setID("grandpa-particles-1"_spr);
                    addChild(fields->m_grandpaParticles1);
                }
                if (strlen(particles2) > 0) {
                    fields->m_grandpaParticles2 = CCParticleSystemQuad::create();
                    fields->m_grandpaParticles2->initWithDictionary(CCDictionary::createWithContentsOfFileThreadSafe(particles2), false);
                    fields->m_grandpaParticles2->setPosition(particlePos);
                    fields->m_grandpaParticles2->setID("grandpa-particles-2"_spr);
                    addChild(fields->m_grandpaParticles2);
                }
            }

            m_difficultySprite->setOpacity(0);
        }
        else if (auto grdSprite = getChildByID("grandpa-demon-sprite"_spr)) {
            grdSprite->removeFromParent();
            if (auto grdInfinity = getChildByID("grandpa-demon-infinity"_spr)) grdInfinity->removeFromParent();
            if (fields->m_grandpaBackground1) fields->m_grandpaBackground1->removeFromParent();
            if (fields->m_grandpaBackground2) fields->m_grandpaBackground2->removeFromParent();
            if (fields->m_grandpaParticles1) fields->m_grandpaParticles1->removeFromParent();
            if (fields->m_grandpaParticles2) fields->m_grandpaParticles2->removeFromParent();
        }

        if (Loader::get()->isModLoaded("hiimjustin000.demons_in_between") && dbo > 0 && dbo < 21) {
            if (auto fakeBetweenDemon = static_cast<CCSprite*>(getChildByID("between-difficulty-sprite"_spr))) fakeBetweenDemon->removeFromParent();

            auto demonsInBetween = Loader::get()->getLoadedMod("hiimjustin000.demons_in_between");
            auto dibFeature = "";
            if (feature == 3 && demonsInBetween->getSettingValue<bool>("enable-legendary")) dibFeature = "_4";
            else if (feature == 4 && demonsInBetween->getSettingValue<bool>("enable-mythic")) dibFeature = "_5";
            auto dibSprite = CCSprite::createWithSpriteFrameName(fmt::format("hiimjustin000.demons_in_between/DIB_{:02d}{}_btn2_001.png", dbo, dibFeature).c_str());
            dibSprite->setID("between-difficulty-sprite"_spr);
            dibSprite->setPosition(position + FakeRate::getDIBOffset(dbo, GJDifficultyName::Long));
            addChild(dibSprite, 3);
            m_difficultySprite->setOpacity(0);
        }
        else if (auto dibSprite = getChildByID("between-difficulty-sprite"_spr)) dibSprite->removeFromParent();

        if (Loader::get()->isModLoaded("minemaker0430.gddp_integration") && gio > 0 && gio < 16) {
            if (auto fakeGDDP = static_cast<CCSprite*>(getChildByID("gddp-difficulty"_spr))) fakeGDDP->removeFromParent();

            auto gddpSprite = CCSprite::createWithSpriteFrameName(FakeRate::getGDDPFrame(gio, GJDifficultyName::Long).c_str());
            gddpSprite->setID("gddp-difficulty"_spr);
            gddpSprite->setPosition(position + CCPoint { 0.25f, 30.0f });
            gddpSprite->setAnchorPoint({ 0.5f, 1.0f });
            addChild(gddpSprite, 3);
            m_difficultySprite->setOpacity(0);
        }
        else if (auto gddpSprite = getChildByID("gddp-difficulty"_spr)) gddpSprite->removeFromParent();
    }

    void fixMoreDifficultiesIncompatibility(int mdo, bool remove, bool hide, int gdo, int dbo, int gio) {
        auto moreDifficultiesSprite = static_cast<CCSprite*>(getChildByID("uproxide.more_difficulties/more-difficulties-spr"));
        if (moreDifficultiesSprite) moreDifficultiesSprite->setVisible(false);
        m_difficultySprite->setOpacity(hide ? 0 : 255);

        auto moreDifficultiesOverride = mdo;
        auto starsRequested = m_level->m_starsRequested;
        auto stars = m_level->m_stars.value();
        if (remove && stars == 0 && (starsRequested == 4 || starsRequested == 7 || starsRequested == 9)) moreDifficultiesOverride = starsRequested;
        if (remove && (stars == 4 || stars == 7 || stars == 9)) moreDifficultiesOverride = stars;
        if ((moreDifficultiesOverride != 4 && moreDifficultiesOverride != 7 && moreDifficultiesOverride != 9)
            || gdo > 0 || dbo > 0 || gio > 0) return;

        auto legacy = Loader::get()->getLoadedMod("uproxide.more_difficulties")->getSettingValue<bool>("legacy-difficulties");
        if (!moreDifficultiesSprite) {
            moreDifficultiesSprite = CCSprite::createWithSpriteFrameName(fmt::format("uproxide.more_difficulties/MD_Difficulty{:02d}{}.png",
                moreDifficultiesOverride, legacy ? "_Legacy" : "").c_str());
            moreDifficultiesSprite->setID("uproxide.more_difficulties/more-difficulties-spr");
            addChild(moreDifficultiesSprite, 3);
        }
        else {
            moreDifficultiesSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(
                fmt::format("uproxide.more_difficulties/MD_Difficulty{:02d}{}.png", moreDifficultiesOverride, legacy ? "_Legacy" : "").c_str()));
            moreDifficultiesSprite->setVisible(true);
        }

        moreDifficultiesSprite->setPosition({
            m_difficultySprite->getPositionX() + (legacy ? 0.0f : 0.25f),
            m_difficultySprite->getPositionY() - (legacy ? 0.0f : 0.1f)
        });
        m_difficultySprite->setOpacity(0);
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
            moreDifficultiesSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(
                fmt::format("uproxide.more_difficulties/MD_Difficulty{:02d}{}.png", mdo, legacy ? "_Legacy" : "").c_str()));
            moreDifficultiesSprite->setVisible(true);
        }

        moreDifficultiesSprite->setPosition({
            difficultySprite->getPositionX() + (legacy ? 0.0f : 0.25f),
            difficultySprite->getPositionY() - (legacy ? 0.0f : 0.1f)
        });
    }
};
