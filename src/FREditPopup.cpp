#include "FREditPopup.hpp"

int FRLevelInfoLayer::getBaseCurrency(int stars) {
    switch (stars) {
        case 2:
            return 40;
        case 3:
            return 60;
        case 4:
            return 100;
        case 5:
            return 140;
        case 6:
            return 180;
        case 7:
            return 220;
        case 8:
            return 280;
        case 9:
            return 340;
        case 10:
            return 400;
        default:
            return 0;
    }
}

int FRLevelInfoLayer::getDifficultyFromLevel(GJGameLevel* level) {
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

bool FRLevelInfoLayer::init(GJGameLevel* level, bool challenge) {
    if (!LevelInfoLayer::init(level, challenge)) return false;

    auto leftSideMenu = getChildByID("left-side-menu");
    auto fakeRateButton = CCMenuItemSpriteExtra::create(
        CCSprite::create("FR_fakeRateBtn_001.png"_spr),
        this,
        menu_selector(FRLevelInfoLayer::onFakeRate)
    );
    leftSideMenu->addChild(fakeRateButton);
    leftSideMenu->updateLayout();

    return true;
}

void FRLevelInfoLayer::updateLabelValues() {
    LevelInfoLayer::updateLabelValues();
    auto vec = Mod::get()->getSavedValue<std::vector<FakeRateSaveData>>("fake-rate", {});
    auto it = std::find_if(vec.begin(), vec.end(), [this](auto const& item) { return item.id == m_level->m_levelID; });
    if (it != vec.end()) updateFakeRate(it->stars, it->feature, it->difficulty, false, true);
    else m_fields->m_fakeRateData = {
        .id = m_level->m_levelID,
        .stars = m_level->m_stars,
        .feature = m_level->m_featured > 1 ? m_level->m_isEpic + 1 : 0,
        .difficulty = getDifficultyFromLevel(m_level)
    };
}

void FRLevelInfoLayer::updateFakeRate(int stars, int feature, int difficulty, bool update, bool coins) {
    m_fields->m_fakeRateData = { .id = m_level->m_levelID, .stars = stars, .feature = feature, .difficulty = difficulty };
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto gsm = GameStatsManager::sharedState();
    auto showStars = stars > 0 || m_level->m_dailyID > 0 || m_level->m_gauntletLevel;
    m_difficultySprite->updateDifficultyFrame(difficulty, (GJDifficultyName)1);
    m_difficultySprite->updateFeatureState((GJFeatureState)feature);
    m_difficultySprite->setPositionY(winSize.height / 2 + 56.0f + (difficulty > 5 ? 5.0f : 0.0f) + (showStars ? 10.0f : 0.0f));
    m_starsIcon->setPosition({ m_difficultySprite->getPositionX() + 8.0f, m_difficultySprite->getPositionY() - 30.0f - (difficulty > 5 ? 9.0f : 0.0f) });
    m_starsIcon->setVisible(showStars);
    m_starsLabel->setPosition(m_starsIcon->getPositionX() - 8.0f, m_starsIcon->getPositionY());
    m_starsLabel->setString(std::to_string(stars).c_str());
    m_starsLabel->setVisible(showStars);
    m_starsLabel->setColor({ 255, 255, (unsigned char)(gsm->hasCompletedLevel(m_level) ? 50 : 255) });
    for (int i = 0; i < m_coins->count(); i++) {
        auto coin = static_cast<CCSprite*>(m_coins->objectAtIndex(i));
        coin->setPositionY(m_difficultySprite->getPositionY() - 31.5f - (showStars ? 14.0f : 0.0f)
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
        diamondLabel->setString(fmt::format("{}/{}", (int)floorf(diamonds * m_level->m_orbCompletion / 100.0f), diamonds).c_str());
        diamondIcon->setPosition({
            m_difficultySprite->getPositionX() + diamondLabel->getScaledContentSize().width * 0.5f + 2.0f,
            m_difficultySprite->getPositionY() - (difficulty > 5 ? 9.0f : 0.0f) - 44.5f
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
        auto orbs = FRLevelInfoLayer::getBaseCurrency(stars);
        auto totalOrbs = (int)floorf(orbs * 1.25f);
        m_orbsLabel->setString(fmt::format("{}/{}", (int)floorf(orbs * m_level->m_orbCompletion / 100.0f), totalOrbs).c_str());
        m_orbsLabel->limitLabelWidth(60.0f, 0.5f, 0.0f);
    }
    auto exactTime = static_cast<CCLabelBMFont*>(getChildByID("cvolton.betterinfo/exact-time"));
    if (exactTime) {
        exactTime->setPositionY(m_lengthLabel->getPositionY() - 2.0f);
        m_lengthLabel->setPositionY(m_lengthLabel->getPositionY() + 6.0f);
    }
}

void FRLevelInfoLayer::onFakeRate(CCObject*) {
    auto popup = FREditPopup::create(
        m_level,
        m_fields->m_fakeRateData.stars,
        m_fields->m_fakeRateData.feature,
        m_fields->m_fakeRateData.difficulty
    );
    popup->m_delegate = this;
    popup->show();
}

FREditPopup* FREditPopup::create(GJGameLevel* level, int stars, int feature, int difficulty) {
    auto ret = new FREditPopup();
    if (ret && ret->initAnchored(200.0f, 150.0f, level, stars, feature, difficulty)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

int FREditPopup::getDifficultyForStars(int stars) {
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

bool FREditPopup::setup(GJGameLevel* level, int stars, int feature, int difficulty) {
    setTitle("Fake Rate");
    m_level = level;
    m_stars = stars;
    m_feature = feature;
    m_difficulty = difficulty;

    m_difficultySprite = GJDifficultySprite::create(difficulty, (GJDifficultyName)1);
    m_difficultySprite->setPositionX(100.0f);
    m_mainLayer->addChild(m_difficultySprite);

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
    m_difficulty = m_stars >= 10 ? m_difficulty > 5 ? m_difficulty : 7 : getDifficultyForStars(m_stars);
    m_difficultySprite->updateDifficultyFrame(m_difficulty, (GJDifficultyName)1);
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