#include "../classes/FREditPopup.hpp"
#include "../classes/FREffects.hpp"

using namespace geode::prelude;

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
        auto fakeRateButton = CCMenuItemSpriteExtra::create(buttonSprite, this, menu_selector(FRLevelInfoLayer::onFakeRate));
        fakeRateButton->setID("fake-rate-button"_spr);
        auto leftSideMenu = getChildByID("left-side-menu");
        leftSideMenu->addChild(fakeRateButton);
        leftSideMenu->updateLayout();

        checkFakeRate();

        return true;
    }

    void onFakeRate(CCObject* sender) {
        FREditPopup::create(m_level, m_fields->m_fakeRateData, [this](FakeRateSaveData data, bool remove) { updateFakeRate(data, remove); })->show();
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
        auto f = m_fields.self();
        f->m_fakeRateData = {
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

        if (!f->m_fakeRateApplied) {
            f->m_fakeRateApplied = true;
            f->m_backgroundColor = static_cast<CCSprite*>(getChildByID("background"))->getColor();
            auto gdutils = Loader::get()->getLoadedMod("gdutilsdevs.gdutils");
            if (gdutils && gdutils->getSettingValue<bool>("activate-background"))
                f->m_backgroundColor = gdutils->getSettingValue<ccColor3B>("background");
        }

        f->m_grandpaBackground1 = static_cast<CCSprite*>(getChildByID("grandpa-background-1"_spr));
        f->m_grandpaBackground2 = static_cast<CCSprite*>(getChildByID("grandpa-background-2"_spr));
        f->m_grandpaParticles1 = static_cast<CCParticleSystemQuad*>(getChildByID("grandpa-particles-1"_spr));
        f->m_grandpaParticles2 = static_cast<CCParticleSystemQuad*>(getChildByID("grandpa-particles-2"_spr));

        auto hide = false;
        if (auto betweenDifficultySprite = static_cast<CCSprite*>(getChildByID("hiimjustin000.demons_in_between/between-difficulty-sprite"))) {
            betweenDifficultySprite->setVisible(remove);
            m_difficultySprite->setOpacity(remove ? 0 : 255);
            hide = remove || hide;
            f->m_fakeRateData.demonsInBetweenOverride = remove ? FakeRate::getDIBOverride(betweenDifficultySprite) : dbo;
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
            f->m_fakeRateData.gddpIntegrationOverride = remove ? FakeRate::getGDDPOverride(gddpDifficultySprite) : gio;
        }
        auto hasEffects = false;
        auto winSize = CCDirector::get()->getWinSize();
        if (Loader::get()->isModLoaded("itzkiba.grandpa_demon") && !gddpOverride) {
            removeChildByTag(69420);
            if (Loader::get()->isModLoaded("uproxide.animated_fire")) removeChildByTag(69420);
            auto hasDemon = false;
            for (auto child : CCArrayExt<CCNode*>(getChildren())) {
                if (child->getID() == "grd-difficulty") {
                    child->setVisible(remove);
                    hasDemon = true;
                }

                auto bgNum = 0;
                if (isSpriteName(child, "itzkiba.grandpa_demon/GrD_demon4_bg.png") &&
                    child != f->m_grandpaBackground1 && child != f->m_grandpaBackground2) {
                    if (child->getID() == "") child->setID(fmt::format("grd-bg-{}", ++bgNum));
                    if (child->getID() == "grd-bg-1") child->setVisible(remove);
                    else if (child->getID() == "grd-bg-2") child->setVisible(remove);
                    hasDemon = true;
                    hasEffects = true;
                }

                auto particleNum = 0;
                if (typeinfo_cast<CCParticleSystemQuad*>(child) && child->getPositionY() == winSize.height / 2 + 76.0f &&
                    child != f->m_grandpaParticles1 && child != f->m_grandpaParticles2) {
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
            f->m_fakeRateData.grandpaDemonOverride = hasDemon && remove ? FakeRate::getGRDOverride(static_cast<CCSprite*>(getChildByID("grd-difficulty"))) : gdo;
        }
        auto gsm = GameStatsManager::get();
        auto showStars = stars != 0 || m_level->m_dailyID > 0 || m_level->m_gauntletLevel;
        m_difficultySprite->updateFeatureState((GJFeatureState)feature);
        m_difficultySprite->updateDifficultyFrame(difficulty, GJDifficultyName::Long);
        auto isDemon = difficulty > 5 || gdo > 0 || dbo > 0 || gio > 0;
        m_difficultySprite->setPositionY(winSize.height / 2 + 56.0f + (isDemon ? 5.0f : 0.0f) + (showStars ? 10.0f : 0.0f));
        auto& position = m_difficultySprite->getPosition();
        m_starsIcon->setPosition({ position.x + 8.0f, position.y - 30.0f - (isDemon ? 9.0f : 0.0f) });
        m_starsIcon->setVisible(showStars);
        m_starsLabel->setPosition({ m_starsIcon->getPositionX() - 8.0f, m_starsIcon->getPositionY() });
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
            ccColor3B { 18, 18, 86 } : f->m_backgroundColor);
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

            if (f->m_grandpaBackground1) f->m_grandpaBackground1->removeFromParent();
            if (f->m_grandpaBackground2) f->m_grandpaBackground2->removeFromParent();
            if (f->m_grandpaParticles1) f->m_grandpaParticles1->removeFromParent();
            if (f->m_grandpaParticles2) f->m_grandpaParticles2->removeFromParent();

            if (showBackground) {
                switch (gdo) {
                    case 3: {
                        f->m_grandpaBackground1 = FREffects::legendaryBackground();
                        f->m_grandpaBackground1->setID("grandpa-background-1"_spr);
                        addChild(f->m_grandpaBackground1);
                        break;
                    }
                    case 4: {
                        f->m_grandpaBackground1 = FREffects::mythicalBackground();
                        f->m_grandpaBackground1->setID("grandpa-background-1"_spr);
                        addChild(f->m_grandpaBackground1);
                        break;
                    }
                    case 5: case 6: {
                        auto backgrounds = FREffects::infinityBackgrounds(gdo == 6);
                        f->m_grandpaBackground1 = backgrounds.first;
                        f->m_grandpaBackground1->setID("grandpa-background-1"_spr);
                        addChild(f->m_grandpaBackground1);
                        f->m_grandpaBackground2 = backgrounds.second;
                        f->m_grandpaBackground2->setID("grandpa-background-2"_spr);
                        addChild(f->m_grandpaBackground2);
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
                    f->m_grandpaParticles1 = CCParticleSystemQuad::create();
                    f->m_grandpaParticles1->initWithDictionary(CCDictionary::createWithContentsOfFileThreadSafe(particles1), false);
                    f->m_grandpaParticles1->setPosition(particlePos);
                    f->m_grandpaParticles1->setID("grandpa-particles-1"_spr);
                    addChild(f->m_grandpaParticles1);
                }
                if (strlen(particles2) > 0) {
                    f->m_grandpaParticles2 = CCParticleSystemQuad::create();
                    f->m_grandpaParticles2->initWithDictionary(CCDictionary::createWithContentsOfFileThreadSafe(particles2), false);
                    f->m_grandpaParticles2->setPosition(particlePos);
                    f->m_grandpaParticles2->setID("grandpa-particles-2"_spr);
                    addChild(f->m_grandpaParticles2);
                }
            }

            m_difficultySprite->setOpacity(0);
        }
        else if (auto grdSprite = getChildByID("grandpa-demon-sprite"_spr)) {
            grdSprite->removeFromParent();
            if (auto grdInfinity = getChildByID("grandpa-demon-infinity"_spr)) grdInfinity->removeFromParent();
            if (f->m_grandpaBackground1) f->m_grandpaBackground1->removeFromParent();
            if (f->m_grandpaBackground2) f->m_grandpaBackground2->removeFromParent();
            if (f->m_grandpaParticles1) f->m_grandpaParticles1->removeFromParent();
            if (f->m_grandpaParticles2) f->m_grandpaParticles2->removeFromParent();
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
            moreDifficultiesSprite->setDisplayFrame(CCSpriteFrameCache::get()->spriteFrameByName(
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
