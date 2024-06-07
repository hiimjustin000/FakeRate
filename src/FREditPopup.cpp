#include "FREditPopup.hpp"

void FRLevelInfoLayer::onModify(auto& self) {
    (void)self.setHookPriority("LevelInfoLayer::init", -100);
    (void)self.setHookPriority("LevelInfoLayer::levelDownloadFinished", -100);
    (void)self.setHookPriority("LevelInfoLayer::levelUpdateFinished", -100);
    (void)self.setHookPriority("LevelInfoLayer::likedItem", -100);
}

bool FRLevelInfoLayer::init(GJGameLevel* level, bool challenge) {
    if (!LevelInfoLayer::init(level, challenge)) return false;

    auto buttonSprite = CircleButtonSprite::createWithSprite("FR_fakeRateBtn_001.png"_spr, 1.0f, CircleBaseColor::Green, CircleBaseSize::Medium);
    buttonSprite->getTopNode()->setScale(1.0f);
    auto fakeRateButton = CCMenuItemExt::createSpriteExtra(buttonSprite, [this](auto) {
        FREditPopup::create(this, m_level, m_fields->m_fakeRateData.stars, m_fields->m_fakeRateData.feature, m_fields->m_fakeRateData.difficulty)->show();
    });
    fakeRateButton->setID("fake-rate-button"_spr);
    auto leftSideMenu = getChildByID("left-side-menu");
    leftSideMenu->addChild(fakeRateButton);
    leftSideMenu->updateLayout();

    checkFakeRate();

    return true;
}

void FRLevelInfoLayer::levelDownloadFinished(GJGameLevel* level) {
    LevelInfoLayer::levelDownloadFinished(level);
    checkFakeRate();
}

void FRLevelInfoLayer::levelUpdateFinished(GJGameLevel* level, UpdateResponse response) {
    LevelInfoLayer::levelUpdateFinished(level, response);
    checkFakeRate();
}

void FRLevelInfoLayer::likedItem(LikeItemType type, int id, bool liked) {
    LevelInfoLayer::likedItem(type, id, liked);
    checkFakeRate();
}

void FRLevelInfoLayer::checkFakeRate() {
    auto vec = Mod::get()->getSavedValue<std::vector<FakeRateSaveData>>("fake-rate", {});
    auto it = std::find_if(vec.begin(), vec.end(), [this](auto const& item) { return item.id == m_level->m_levelID; });
    if (it != vec.end()) updateFakeRate(it->stars, it->feature, it->difficulty, false, true);
    else m_fields->m_fakeRateData = {
        .id = m_level->m_levelID,
        .stars = m_level->m_stars,
        .feature = m_level->m_featured > 1 ? m_level->m_isEpic + 1 : 0,
        .difficulty = FakeRate::getDifficultyFromLevel(m_level)
    };
}

void FRLevelInfoLayer::updateFakeRate(int stars, int feature, int difficulty, bool update, bool coins) {
    m_fields->m_fakeRateData = { .id = m_level->m_levelID, .stars = stars, .feature = feature, .difficulty = difficulty };
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
    m_starsLabel->setColor({ 255, 255, (unsigned char)(gsm->hasCompletedLevel(m_level) ? 50 : 255) });
    for (int i = 0; i < m_coins->count(); i++) {
        auto coin = static_cast<CCSprite*>(m_coins->objectAtIndex(i));
        coin->setPositionY(position.y - 31.5f - (showStars ? 14.0f : 0.0f)
            - (m_level->m_gauntletLevel || m_level->m_dailyID > 0 ? 14.5f : 0.0f) - (difficulty > 5 ? 9.0f : 0.0f));
        auto coinStr = fmt::format("{}_{}", m_level->m_levelID.value(), i + 1);
        if (m_level->m_dailyID > 0) coinStr += "_" + std::to_string(m_level->m_dailyID);
        else if (m_level->m_gauntletLevel) coinStr += "_g";
        if (coins || m_level->m_coinsVerified == 1) {
            if (gsm->hasUserCoin(coinStr.c_str()) || gsm->hasPendingUserCoin(coinStr.c_str())) coin->setColor({ 255, 255, 255 });
            else coin->setColor({ 165, 165, 165 });
        }
        else {
            if (gsm->hasUserCoin(coinStr.c_str()) || gsm->hasPendingUserCoin(coinStr.c_str())) coin->setColor({ 255, 175, 75 });
            else coin->setColor({ 165, 113, 48 });
        }
    }
    if (m_level->m_dailyID > 0 || m_level->m_gauntletLevel) {
        auto diamondLabel = getChildOfType<CCLabelBMFont>(this, update ? 5 : m_level->m_dailyID > 0 ? 7 : 6);
        auto diamondIcon = static_cast<CCSprite*>(getChildBySpriteFrameName(this, "diamond_small01_001.png"));
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
    m_lengthLabel->setPositionY(lengthIcon->getPositionY());
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
    if (auto exactTime = static_cast<CCLabelBMFont*>(getChildByID("cvolton.betterinfo/exact-time"))) {
        exactTime->setPositionY(m_lengthLabel->getPositionY() - 2.0f);
        m_lengthLabel->setPositionY(m_lengthLabel->getPositionY() + 6.0f);
    }

    if (Loader::get()->isModLoaded("uproxide.more_difficulties")) fixMoreDifficultiesIncompatibility(difficultySpriteParent);
}

void FRLevelInfoLayer::fixMoreDifficultiesIncompatibility(CCNode* difficultySpriteParent) {
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
    if (spriteName == "uproxide.more_difficulties/MD_DifficultyCP.png") {
        moreDifficultiesSprite->setVisible(true);
        m_difficultySprite->setOpacity(0);
    }
    else switch (m_fields->m_fakeRateData.stars) {
        case 4: {
            if (!moreDifficultiesSprite) {
                moreDifficultiesSprite = CCSprite::createWithSpriteFrameName(fmt::format("uproxide.more_difficulties/MD_Difficulty04{}.png", legacy ? "_Legacy" : "").c_str());
                moreDifficultiesSprite->setID("uproxide.more_difficulties/more-difficulties-spr");
                addChild(moreDifficultiesSprite, 3);
            }
            else {
                moreDifficultiesSprite->initWithSpriteFrameName(fmt::format("uproxide.more_difficulties/MD_Difficulty04{}.png", legacy ? "_Legacy" : "").c_str());
                moreDifficultiesSprite->setVisible(true);
            }
            moreDifficultiesSprite->setPosition(pos);
            m_difficultySprite->setOpacity(0);
            break;
        }
        case 7: {
            if (!moreDifficultiesSprite) {
                moreDifficultiesSprite = CCSprite::createWithSpriteFrameName(fmt::format("uproxide.more_difficulties/MD_Difficulty07{}.png", legacy ? "_Legacy" : "").c_str());
                moreDifficultiesSprite->setID("uproxide.more_difficulties/more-difficulties-spr");
                addChild(moreDifficultiesSprite, 3);
            }
            else {
                moreDifficultiesSprite->initWithSpriteFrameName(fmt::format("uproxide.more_difficulties/MD_Difficulty07{}.png", legacy ? "_Legacy" : "").c_str());
                moreDifficultiesSprite->setVisible(true);
            }
            moreDifficultiesSprite->setPosition(pos);
            m_difficultySprite->setOpacity(0);
            break;
        }
        case 9: {
            if (!moreDifficultiesSprite) {
                moreDifficultiesSprite = CCSprite::createWithSpriteFrameName(fmt::format("uproxide.more_difficulties/MD_Difficulty09{}.png", legacy ? "_Legacy" : "").c_str());
                moreDifficultiesSprite->setID("uproxide.more_difficulties/more-difficulties-spr");
                addChild(moreDifficultiesSprite, 3);
            }
            else {
                moreDifficultiesSprite->initWithSpriteFrameName(fmt::format("uproxide.more_difficulties/MD_Difficulty09{}.png", legacy ? "_Legacy" : "").c_str());
                moreDifficultiesSprite->setVisible(true);
            }
            moreDifficultiesSprite->setPosition(pos);
            m_difficultySprite->setOpacity(0);
            break;
        }
    }
}

FREditPopup* FREditPopup::create(FRLevelInfoLayer* delegate, GJGameLevel* level, int stars, int feature, int difficulty) {
    auto ret = new FREditPopup();
    if (ret && ret->initAnchored(200.0f, 150.0f, delegate, level, stars, feature, difficulty)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool FREditPopup::setup(FRLevelInfoLayer* delegate, GJGameLevel* level, int stars, int feature, int difficulty) {
    setTitle("Fake Rate");
    m_delegate = delegate;
    m_level = level;
    m_stars = stars;
    m_feature = feature;
    m_difficulty = difficulty;

    m_difficultySprite = GJDifficultySprite::create(difficulty, GJDifficultyName::Long);
    m_difficultySprite->setPositionX(100.0f);
    m_mainLayer->addChild(m_difficultySprite);

    if (Loader::get()->isModLoaded("uproxide.more_difficulties")) {
        auto legacy = Loader::get()->getLoadedMod("uproxide.more_difficulties")->getSettingValue<bool>("legacy-difficulties");
        auto pos = CCPoint { legacy ? 100.0f : 100.25f, legacy ? 85.0f : 84.9f };

        m_casualSprite = CCSprite::createWithSpriteFrameName(fmt::format("uproxide.more_difficulties/MD_Difficulty04{}.png", legacy ? "_Legacy" : "").c_str());
        m_casualSprite->setPosition(pos);
        m_casualSprite->setVisible(false);
        m_mainLayer->addChild(m_casualSprite);

        m_toughSprite = CCSprite::createWithSpriteFrameName(fmt::format("uproxide.more_difficulties/MD_Difficulty07{}.png", legacy ? "_Legacy" : "").c_str());
        m_toughSprite->setPosition(pos);
        m_toughSprite->setVisible(false);
        m_mainLayer->addChild(m_toughSprite);

        m_cruelSprite = CCSprite::createWithSpriteFrameName(fmt::format("uproxide.more_difficulties/MD_Difficulty09{}.png", legacy ? "_Legacy" : "").c_str());
        m_cruelSprite->setPosition(pos);
        m_cruelSprite->setVisible(false);
        m_mainLayer->addChild(m_cruelSprite);
    }

    m_starSprite = CCSprite::createWithSpriteFrameName(m_level->m_levelLength < 5 ? "star_small01_001.png" : "moon_small01_001.png");
    m_mainLayer->addChild(m_starSprite);

    m_starsLabel = CCLabelBMFont::create(std::to_string(stars).c_str(), "bigFont.fnt");
    m_starsLabel->setScale(0.4f);
    m_starsLabel->setAnchorPoint({ 1.0f, 0.5f });
    m_mainLayer->addChild(m_starsLabel);

    auto starLeftSprite = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
    starLeftSprite->setFlipX(true);
    starLeftSprite->setScale(0.2f);
    m_starLeftArrow = CCMenuItemSpriteExtra::create(starLeftSprite, this, menu_selector(FREditPopup::onStarLeft));
    m_buttonMenu->addChild(m_starLeftArrow);

    auto starRightSprite = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
    starRightSprite->setScale(0.2f);
    m_starRightArrow = CCMenuItemSpriteExtra::create(starRightSprite, this, menu_selector(FREditPopup::onStarRight));
    m_buttonMenu->addChild(m_starRightArrow);

    auto difficultyLeftSprite = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
    difficultyLeftSprite->setFlipX(true);
    difficultyLeftSprite->setScale(0.3f);
    m_difficultyLeftArrow = CCMenuItemSpriteExtra::create(difficultyLeftSprite, this, menu_selector(FREditPopup::onDifficultyLeft));
    m_difficultyLeftArrow->setPosition(70.0f, 70.0f);
    m_buttonMenu->addChild(m_difficultyLeftArrow);

    auto difficultyRightSprite = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
    difficultyRightSprite->setScale(0.3f);
    m_difficultyRightArrow = CCMenuItemSpriteExtra::create(difficultyRightSprite, this, menu_selector(FREditPopup::onDifficultyRight));
    m_difficultyRightArrow->setPosition(130.0f, 70.0f);
    m_buttonMenu->addChild(m_difficultyRightArrow);

    auto featureLeftSprite = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
    featureLeftSprite->setFlipX(true);
    featureLeftSprite->setScale(0.4f);
    m_featureLeftArrow = CCMenuItemSpriteExtra::create(featureLeftSprite, this, menu_selector(FREditPopup::onFeatureLeft));
    m_featureLeftArrow->setPositionX(70.0f);
    m_buttonMenu->addChild(m_featureLeftArrow);

    auto featureRightSprite = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
    featureRightSprite->setScale(0.4f);
    m_featureRightArrow = CCMenuItemSpriteExtra::create(featureRightSprite, this, menu_selector(FREditPopup::onFeatureRight));
    m_featureRightArrow->setPositionX(130.0f);
    m_buttonMenu->addChild(m_featureRightArrow);

    auto addButton = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Add", "goldFont.fnt", "GJ_button_01.png", 0.8f),
        this,
        menu_selector(FREditPopup::onAdd)
    );
    addButton->setPosition(50.0f, 25.0f);
    m_buttonMenu->addChild(addButton);

    auto removeButton = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Remove", "goldFont.fnt", "GJ_button_06.png", 0.8f),
        this,
        menu_selector(FREditPopup::onRemove)
    );
    removeButton->setPosition(135.0f, 25.0f);
    m_buttonMenu->addChild(removeButton);

    updateLabels();

    return true;
}

void FREditPopup::onStarLeft(CCObject*) {
    m_stars--;
    if (m_stars < 1) m_stars = 10;
    updateLabels();
}

void FREditPopup::onStarRight(CCObject*) {
    m_stars++;
    if (m_stars > 10) m_stars = 1;
    updateLabels();
}

void FREditPopup::onDifficultyLeft(CCObject*) {
    if (m_difficulty == 6) m_difficulty = 8;
    else if (m_difficulty == 7) m_difficulty = 10;
    else if (m_difficulty == 9) m_difficulty = 6;
    else m_difficulty--;
    updateLabels();
}

void FREditPopup::onDifficultyRight(CCObject*) {
    if (m_difficulty == 6) m_difficulty = 9;
    else if (m_difficulty == 8) m_difficulty = 6;
    else if (m_difficulty == 10) m_difficulty = 7;
    else m_difficulty++;
    updateLabels();
}

void FREditPopup::onFeatureLeft(CCObject*) {
    m_feature--;
    if (m_feature < 0) m_feature = 4;
    updateLabels();
}

void FREditPopup::onFeatureRight(CCObject*) {
    m_feature++;
    if (m_feature > 4) m_feature = 0;
    updateLabels();
}

void FREditPopup::updateLabels() {
    m_difficulty = m_stars >= 10 ? m_difficulty > 5 ? m_difficulty : 7 : FakeRate::getDifficultyForStars(m_stars);
    m_difficultySprite->updateDifficultyFrame(m_difficulty, GJDifficultyName::Long);
    m_difficultySprite->updateFeatureState((GJFeatureState)m_feature);
    m_difficultySprite->setPositionY(85.0f + (m_difficulty > 5 ? 5.0f : 0.0f));
    m_starSprite->setPosition({ m_difficultySprite->getPositionX() + 8.0f, m_difficultySprite->getPositionY() - 30.0f - (m_difficulty > 5 ? 9.0f : 0.0f) });
    m_starsLabel->setPosition(m_starSprite->getPositionX() - 8.0f, m_starSprite->getPositionY());
    m_starsLabel->setString(std::to_string(m_stars).c_str());
    m_starLeftArrow->setPosition(m_starsLabel->getPositionX() - 20.0f, m_starsLabel->getPositionY());
    m_starRightArrow->setPosition(m_starSprite->getPositionX() + 12.0f, m_starSprite->getPositionY());
    m_difficultyLeftArrow->setVisible(m_difficulty > 5);
    m_difficultyLeftArrow->setEnabled(m_difficulty > 5);
    m_difficultyRightArrow->setVisible(m_difficulty > 5);
    m_difficultyRightArrow->setEnabled(m_difficulty > 5);
    m_featureLeftArrow->setPositionY(m_difficultySprite->getPositionY() + 5.0f);
    m_featureRightArrow->setPositionY(m_difficultySprite->getPositionY() + 5.0f);
    if (Loader::get()->isModLoaded("uproxide.more_difficulties")) {
        m_casualSprite->setVisible(m_stars == 4);
        m_toughSprite->setVisible(m_stars == 7);
        m_cruelSprite->setVisible(m_stars == 9);
        m_difficultySprite->setOpacity(m_stars != 4 && m_stars != 7 && m_stars != 9 ? 255 : 0);
    }
}

void FREditPopup::onAdd(CCObject*) {
    auto vec = Mod::get()->getSavedValue<std::vector<FakeRateSaveData>>("fake-rate", {});
    auto it = std::find_if(vec.begin(), vec.end(), [this](auto const& item) {
        return item.id == m_level->m_levelID;
    });
    if (it != vec.end()) {
        it->stars = m_stars;
        it->feature = m_feature;
        it->difficulty = m_difficulty;
    }
    else {
        vec.push_back({
            .id = m_level->m_levelID,
            .stars = m_stars,
            .feature = m_feature,
            .difficulty = m_difficulty
        });
    }
    Mod::get()->setSavedValue("fake-rate", vec);
    m_delegate->updateFakeRate(m_stars, m_feature, m_difficulty, true, true);
    onClose(nullptr);
}

void FREditPopup::onRemove(CCObject*) {
    auto vec = Mod::get()->getSavedValue<std::vector<FakeRateSaveData>>("fake-rate", {});
    if (vec.empty()) return;
    vec.erase(std::remove_if(vec.begin(), vec.end(), [this](auto const& item) {
        return item.id == m_level->m_levelID;
    }), vec.end());
    Mod::get()->setSavedValue("fake-rate", vec);
    auto difficulty = m_level->getAverageDifficulty();
    if (m_level->m_demon > 0) switch (m_level->m_demonDifficulty) {
        case 3: difficulty = 7; break;
        case 4: difficulty = 8; break;
        case 5: difficulty = 9; break;
        case 6: difficulty = 10; break;
        default: difficulty = 6; break;
    }
    m_delegate->updateFakeRate(m_level->m_stars, m_level->m_featured > 1 ? m_level->m_isEpic + 1 : 0, difficulty, true, false);
    onClose(nullptr);
}