#include "FREditPopup.hpp"

FREditPopup* FREditPopup::create(GJGameLevel* level, FakeRateSaveData data, UpdateFakeRateCallback callback) {
    auto ret = new FREditPopup();
    if (ret->initAnchored(300.0f, 200.0f, level, data, callback)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool FREditPopup::setup(GJGameLevel* level, FakeRateSaveData data, UpdateFakeRateCallback callback) {
    setTitle("Fake Rate");
    m_level = level;
    m_stars = data.stars;
    m_feature = data.feature;
    m_difficulty = data.difficulty;
    m_moreDifficultiesOverride = data.moreDifficultiesOverride;
    m_grandpaDemonOverride = data.grandpaDemonOverride;
    m_demonsInBetweenOverride = data.demonsInBetweenOverride;
    m_gddpIntegrationOverride = data.gddpIntegrationOverride;
    m_coins = data.coins;
    m_legacy = false;

    m_difficultySprite = GJDifficultySprite::create(data.difficulty, GJDifficultyName::Long);
    m_difficultySprite->setPositionX(60.0f);
    m_mainLayer->addChild(m_difficultySprite);

    if (Loader::get()->isModLoaded("uproxide.animated_fire")) {
        m_fireSprite = FireSprite::create(GJFeatureState::Epic);
        m_fireSprite->setPosition(m_difficultySprite->getContentSize() / 2 + CCPoint { 0.0f, 16.875f });
        m_difficultySprite->addChild(m_fireSprite, -1);
    }
    if (Loader::get()->isModLoaded("uproxide.more_difficulties")) {
        m_legacy = Loader::get()->getLoadedMod("uproxide.more_difficulties")->getSettingValue<bool>("legacy-difficulties");
        m_mdSprite = CCSprite::createWithSpriteFrameName(m_legacy ?
            "uproxide.more_difficulties/MD_Difficulty04_Legacy.png" : "uproxide.more_difficulties/MD_Difficulty04.png");
        m_mdSprite->setPosition(m_legacy ? CCPoint { 60.0f, 100.0f } : CCPoint { 60.25f, 99.9f });
        m_mdSprite->setVisible(false);
        m_mainLayer->addChild(m_mdSprite);
    }
    if (Loader::get()->isModLoaded("itzkiba.grandpa_demon")) {
        m_grdSprite = CCSprite::createWithSpriteFrameName("itzkiba.grandpa_demon/GrD_demon0_text.png");
        m_grdSprite->setPosition({ 60.0f, 105.0f });
        m_grdSprite->setVisible(false);
        m_mainLayer->addChild(m_grdSprite);

        m_grdInfinity = FREffects::grdInfinity();
        m_grdInfinity->setPosition({ 59.6f, 119.0f });
        m_grdInfinity->setVisible(false);
        m_mainLayer->addChild(m_grdInfinity);
    }
    if (Loader::get()->isModLoaded("hiimjustin000.demons_in_between")) {
        m_dibSprite = CCSprite::createWithSpriteFrameName("hiimjustin000.demons_in_between/DIB_01_btn2_001.png");
        m_dibSprite->setPosition({ 60.0f, 105.0f });
        m_dibSprite->setVisible(false);
        m_mainLayer->addChild(m_dibSprite);
    }
    if (Loader::get()->isModLoaded("minemaker0430.gddp_integration")) {
        m_gddpSprite = CCSprite::createWithSpriteFrameName("minemaker0430.gddp_integration/DP_BeginnerText.png");
        m_gddpSprite->setPosition({ 60.25f, 140.0f });
        m_gddpSprite->setAnchorPoint({ 0.5f, 1.0f });
        m_gddpSprite->setVisible(false);
        m_mainLayer->addChild(m_gddpSprite);
    }

    m_starSprite = CCSprite::createWithSpriteFrameName(m_level->m_levelLength < 5 ? "star_small01_001.png" : "moon_small01_001.png");
    m_mainLayer->addChild(m_starSprite);

    m_starsLabel = CCLabelBMFont::create(std::to_string(data.stars).c_str(), "bigFont.fnt");
    m_starsLabel->setScale(0.4f);
    m_starsLabel->setAnchorPoint({ 1.0f, 0.5f });
    m_mainLayer->addChild(m_starsLabel);

    m_coinSprites = CCArray::create();
    m_coinSprites->retain();

    for (int i = 0; i < m_level->m_coins; i++) {
        auto coin = CCSprite::createWithSpriteFrameName("usercoin_small01_001.png");
        coin->setPositionX(60.0f +
            (i == 2 ? 10.0f : i == 1 && m_level->m_coins == 2 ? 5.0f : i == 0 && m_level->m_coins == 2 ? -5.0f : i == 0 && m_level->m_coins == 3 ? -10.0f : 0.0f));
        m_mainLayer->addChild(coin);
        m_coinSprites->addObject(coin);
    }

    auto difficultyButton = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("Difficulty", "goldFont.fnt", "GJ_button_02.png", 0.8f), [this](auto) {
        FRSetDifficultyPopup::create({
            .difficulty = m_difficulty,
            .moreDifficultiesOverride = m_moreDifficultiesOverride,
            .grandpaDemonOverride = m_grandpaDemonOverride,
            .demonsInBetweenOverride = m_demonsInBetweenOverride,
            .gddpIntegrationOverride = m_gddpIntegrationOverride
        }, m_legacy, [this](int difficulty, int mdo, int gdo, int dbo, int gio) {
            m_difficulty = difficulty;
            m_moreDifficultiesOverride = mdo;
            m_grandpaDemonOverride = gdo;
            m_demonsInBetweenOverride = dbo;
            m_gddpIntegrationOverride = gio;
            updateLabels();
        })->show();
    });
    difficultyButton->setPosition(200.0f, 150.0f);
    m_buttonMenu->addChild(difficultyButton);

    auto starsButton = CCMenuItemExt::createSpriteExtra(ButtonSprite::create(m_level->m_levelLength < 5 ? "Stars" : "Moons",
        "goldFont.fnt", "GJ_button_02.png", 0.8f), [this](auto) {
        FRSetStarsPopup::create(m_stars, m_level->m_levelLength == 5, [this](int stars) {
            m_stars = stars;
            updateLabels();
        })->show();
    });
    starsButton->setPosition(200.0f, 110.0f);
    m_buttonMenu->addChild(starsButton);

    auto featureButton = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("Feature", "goldFont.fnt", "GJ_button_02.png", 0.8f), [this](auto) {
        FRSetFeaturePopup::create({
            .feature = m_feature,
            .difficulty = m_difficulty,
            .moreDifficultiesOverride = m_moreDifficultiesOverride,
            .grandpaDemonOverride = m_grandpaDemonOverride,
            .demonsInBetweenOverride = m_demonsInBetweenOverride,
            .gddpIntegrationOverride = m_gddpIntegrationOverride
        }, m_legacy, [this](int feature) {
            m_feature = feature;
            updateLabels();
        })->show();
    });
    featureButton->setPosition(200.0f, 70.0f);
    m_buttonMenu->addChild(featureButton);

    auto coinsToggle = CCMenuItemExt::createTogglerWithStandardSprites(0.7f, [this](auto) {
        m_coins = !m_coins;
        updateLabels();
    });
    coinsToggle->setPosition(30.0f, 30.0f);
    coinsToggle->toggle(m_coins);
    m_buttonMenu->addChild(coinsToggle);

    auto coinsLabel = CCLabelBMFont::create("Coins", "bigFont.fnt");
    coinsLabel->setScale(0.5f);
    coinsLabel->setPosition(70.0f, 30.0f);
    m_mainLayer->addChild(coinsLabel);

    auto addButton = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("Add", "goldFont.fnt", "GJ_button_01.png", 0.8f), [this, callback](auto) {
        auto vec = Mod::get()->getSavedValue<std::vector<FakeRateSaveData>>("fake-rate", {});
        std::vector<FakeRateSaveData>::iterator it = std::find_if(vec.begin(), vec.end(), [this](FakeRateSaveData const& item) {
            return item.id == m_level->m_levelID;
        });
        if (it != vec.end()) {
            it->stars = m_stars;
            it->feature = m_feature;
            it->difficulty = m_difficulty;
            it->moreDifficultiesOverride = m_moreDifficultiesOverride;
            it->grandpaDemonOverride = m_grandpaDemonOverride;
            it->demonsInBetweenOverride = m_demonsInBetweenOverride;
            it->gddpIntegrationOverride = m_gddpIntegrationOverride;
            it->coins = m_coins;
        }
        else {
            vec.push_back({
                .id = m_level->m_levelID,
                .stars = m_stars,
                .feature = m_feature,
                .difficulty = m_difficulty,
                .moreDifficultiesOverride = m_moreDifficultiesOverride,
                .grandpaDemonOverride = m_grandpaDemonOverride,
                .demonsInBetweenOverride = m_demonsInBetweenOverride,
                .gddpIntegrationOverride = m_gddpIntegrationOverride,
                .coins = m_coins
            });
        }
        Mod::get()->setSavedValue("fake-rate", vec);
        callback({
            .id = m_level->m_levelID,
            .stars = m_stars,
            .feature = m_feature,
            .difficulty = m_difficulty,
            .moreDifficultiesOverride = m_moreDifficultiesOverride,
            .grandpaDemonOverride = m_grandpaDemonOverride,
            .demonsInBetweenOverride = m_demonsInBetweenOverride,
            .gddpIntegrationOverride = m_gddpIntegrationOverride,
            .coins = m_coins
        }, false);
        onClose(nullptr);
    });
    addButton->setPosition(150.0f, 30.0f);
    m_buttonMenu->addChild(addButton);

    auto removeButton = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("Remove", "goldFont.fnt", "GJ_button_06.png", 0.8f), [this, callback](auto) {
        auto vec = Mod::get()->getSavedValue<std::vector<FakeRateSaveData>>("fake-rate", {});
        if (vec.empty()) return;
        vec.erase(std::remove_if(vec.begin(), vec.end(), [this](FakeRateSaveData const& item) {
            return item.id == m_level->m_levelID;
        }), vec.end());
        Mod::get()->setSavedValue("fake-rate", vec);
        auto stars = m_level->m_stars;
        callback({
            .id = m_level->m_levelID,
            .stars = stars,
            .feature = m_level->m_featured > 1 ? m_level->m_isEpic + 1 : 0,
            .difficulty = FakeRate::getDifficultyFromLevel(m_level),
            .moreDifficultiesOverride = stars == 4 || stars == 7 || stars == 9 ? stars : 0,
            .grandpaDemonOverride = 0,
            .demonsInBetweenOverride = 0,
            .gddpIntegrationOverride = 0,
            .coins = m_level->m_coinsVerified > 0
        }, true);
        onClose(nullptr);
    });
    removeButton->setPosition(235.0f, 30.0f);
    m_buttonMenu->addChild(removeButton);

    updateLabels();

    return true;
}

void FREditPopup::updateLabels() {
    if (m_fireSprite && m_difficultySprite->m_featureState != (GJFeatureState)m_feature) m_fireSprite->retain();
    m_difficultySprite->updateFeatureState((GJFeatureState)m_feature);
    m_difficultySprite->updateDifficultyFrame(m_difficulty, GJDifficultyName::Long);
    auto isDemon = m_difficulty > 5 || m_grandpaDemonOverride > 0 || m_demonsInBetweenOverride > 0 || m_gddpIntegrationOverride > 0;
    m_difficultySprite->setPositionY(100.0f + (isDemon ? 5.0f : 0.0f) + (m_stars != 0 ? 10.0f : 0.0f));
    m_starSprite->setPosition({ m_difficultySprite->getPositionX() + 8.0f, m_difficultySprite->getPositionY() - 30.0f - (isDemon ? 9.0f : 0.0f) });
    m_starSprite->setVisible(m_stars != 0);
    m_starsLabel->setPosition(m_starSprite->getPositionX() - 8.0f, m_starSprite->getPositionY());
    m_starsLabel->setString(std::to_string(m_stars).c_str());
    m_starsLabel->setVisible(m_stars != 0);
    for (auto coin : CCArrayExt<CCSprite*>(m_coinSprites)) {
        coin->setPositionY(m_difficultySprite->getPositionY() - 31.5f - (m_stars != 0 ? 14.0f : 0.0f) - (isDemon ? 9.0f : 0.0f));
        coin->setColor(m_coins ? ccColor3B { 255, 255, 255 } : ccColor3B { 255, 175, 75 });
    }
    m_difficultySprite->setOpacity(255);
    if (Loader::get()->isModLoaded("uproxide.animated_fire") && m_feature > 1) {
        m_fireSprite->init((GJFeatureState)m_feature);
        m_fireSprite->setPosition(m_difficultySprite->getContentSize() / 2 + CCPoint { 0.0f, m_feature == 2 ? 16.875f : 15.875f });
        if (auto oldFire = getChildBySpriteFrameName(m_difficultySprite, (m_feature == 2 ? "GJ_epicCoin_001.png" :
                fmt::format("GJ_epicCoin{}_001.png", m_feature - 1)).c_str())) oldFire->setVisible(false);
        m_difficultySprite->addChild(m_fireSprite, -1);
        m_fireSprite->release();
    }
    if (Loader::get()->isModLoaded("uproxide.more_difficulties")) {
        if (m_moreDifficultiesOverride == 4 || m_moreDifficultiesOverride == 7 || m_moreDifficultiesOverride == 9) {
            m_mdSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(
                fmt::format("uproxide.more_difficulties/MD_Difficulty{:02d}{}.png", m_moreDifficultiesOverride, m_legacy ? "_Legacy" : "").c_str()));
            m_mdSprite->setPosition(m_difficultySprite->getPosition() + (m_legacy ? CCPoint { 0.0f, 0.0f } : CCPoint { 0.25f, -0.1f }));
            m_mdSprite->setVisible(true);
            m_difficultySprite->setOpacity(0);
        }
        else m_mdSprite->setVisible(false);
    }
    if (Loader::get()->isModLoaded("itzkiba.grandpa_demon")) {
        if (m_grandpaDemonOverride > 0 && m_grandpaDemonOverride < 7) {
            m_grdSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(
                fmt::format("itzkiba.grandpa_demon/GrD_demon{}_text.png", m_grandpaDemonOverride - 1).c_str()));
            m_grdSprite->setPosition(m_difficultySprite->getPosition());
            m_grdSprite->setVisible(true);
            if (m_mdSprite) m_mdSprite->setVisible(false);
            m_grdInfinity->setPosition(m_difficultySprite->getPosition() + CCPoint { -0.4f, 14.0f });
            m_difficultySprite->setOpacity(0);
        }
        else m_grdSprite->setVisible(false);

        m_grdInfinity->setVisible(m_grandpaDemonOverride == 5);
    }
    if (Loader::get()->isModLoaded("hiimjustin000.demons_in_between")) {
        if (m_demonsInBetweenOverride > 0 && m_demonsInBetweenOverride < 21) {
            auto demonsInBetween = Loader::get()->getLoadedMod("hiimjustin000.demons_in_between");
            auto dibFeature = "";
            if (m_feature == 3 && demonsInBetween->getSettingValue<bool>("enable-legendary")) dibFeature = "_4";
            else if (m_feature == 4 && demonsInBetween->getSettingValue<bool>("enable-mythic")) dibFeature = "_5";
            m_dibSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(
                fmt::format("hiimjustin000.demons_in_between/DIB_{:02d}{}_btn2_001.png", m_demonsInBetweenOverride, dibFeature).c_str()));
            m_dibSprite->setPosition(m_difficultySprite->getPosition() + FakeRate::getDIBOffset(m_demonsInBetweenOverride, GJDifficultyName::Long));
            m_dibSprite->setVisible(true);
            if (m_mdSprite) m_mdSprite->setVisible(false);
            m_difficultySprite->setOpacity(0);
        }
        else m_dibSprite->setVisible(false);
    }
    if (Loader::get()->isModLoaded("minemaker0430.gddp_integration")) {
        if (m_gddpIntegrationOverride > 0 && m_gddpIntegrationOverride < 16) {
            m_gddpSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(
                FakeRate::getGDDPFrame(m_gddpIntegrationOverride, GJDifficultyName::Long).c_str()));
            m_gddpSprite->setPosition(m_difficultySprite->getPosition() + CCPoint { 0.25f, 30.0f });
            m_gddpSprite->setVisible(true);
            if (m_mdSprite) m_mdSprite->setVisible(false);
            m_difficultySprite->setOpacity(0);
        }
        else m_gddpSprite->setVisible(false);
    }
}

FREditPopup::~FREditPopup() {
    CC_SAFE_RELEASE(m_coinSprites);
}

FRSetDifficultyPopup* FRSetDifficultyPopup::create(FakeRateSaveData data, bool legacy, SetDifficultyCallback callback) {
    auto ret = new FRSetDifficultyPopup();
    if (ret->initAnchored(300.0f, 250.0f, data, legacy, callback)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool FRSetDifficultyPopup::setup(FakeRateSaveData data, bool legacy, SetDifficultyCallback callback) {
    setTitle("Select Difficulty");
    m_noElasticity = true;
    m_difficulty = data.difficulty;
    m_moreDifficultiesOverride = data.moreDifficultiesOverride;
    m_grandpaDemonOverride = data.grandpaDemonOverride;
    m_demonsInBetweenOverride = data.demonsInBetweenOverride;
    m_gddpIntegrationOverride = data.gddpIntegrationOverride;
    m_legacy = legacy;

    auto table = TableNode::create(Loader::get()->isModLoaded("uproxide.more_difficulties") ? 5 : 4, 3);
    table->setColumnLayout(ColumnLayout::create()->setAxisReverse(true));
    table->setRowLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::Even));
    table->setRowHeight(63.0f);
    table->setContentSize({ 300.0f, 170.0f });
    table->setPosition(150.0f, 130.0f);
    m_mainLayer->addChild(table);

    for (auto [d, mdo] : DIFFICULTIES) {
        auto num = d == -1 ? "auto" : fmt::format("{:02d}", d);
        auto frameName = d > 5 ? fmt::format("difficulty_{}_btn2_001.png", num) : fmt::format("difficulty_{}_btn_001.png", num);
        if (Loader::get()->isModLoaded("uproxide.more_difficulties") && mdo > 0)
            frameName = fmt::format("uproxide.more_difficulties/MD_Difficulty{:02d}{}.png", mdo, m_legacy ? "_Legacy" : "");
        else if (mdo > 0) continue;
        auto toggle = CCMenuItemExt::createSpriteExtraWithFrameName(frameName.c_str(), 1.0f, [this, d, mdo](CCMenuItemSpriteExtra* sender) {
            if (sender == m_selected) return;
            m_difficulty = d;
            m_moreDifficultiesOverride = mdo;
            m_grandpaDemonOverride = 0;
            m_demonsInBetweenOverride = 0;
            m_gddpIntegrationOverride = 0;
            FakeRate::toggle(m_selected->getNormalImage(), false);
            FakeRate::toggle(sender->getNormalImage(), true);
            m_selected = sender;
        });
        auto isToggled = mdo == m_moreDifficultiesOverride && (m_moreDifficultiesOverride <= 0 ? d == m_difficulty : true);
        FakeRate::toggle(toggle->getNormalImage(), isToggled);
        m_selected = isToggled ? toggle : m_selected;
        table->addButton(toggle);
    }

    table->updateAllLayouts();

    auto confirmButton = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("Confirm", "goldFont.fnt", "GJ_button_01.png", 0.8f), [this, callback](auto) {
        callback(m_difficulty, m_moreDifficultiesOverride, m_grandpaDemonOverride, m_demonsInBetweenOverride, m_gddpIntegrationOverride);
        onClose(nullptr);
    });
    confirmButton->setPosition(150.0f, 25.0f);
    m_buttonMenu->addChild(confirmButton);

    auto overrideMenu = CCMenu::create();
    overrideMenu->setLayout(RowLayout::create()->setGap(4.0f)->setAxisAlignment(AxisAlignment::End));
    overrideMenu->setPosition(245.0f, 25.0f);
    overrideMenu->setContentSize({ 100.0f, 30.0f });
    m_mainLayer->addChild(overrideMenu);

    if (Loader::get()->isModLoaded("itzkiba.grandpa_demon"))
        overrideMenu->addChild(CCMenuItemExt::createSpriteExtraWithFilename("FR_grdBtn_001.png"_spr, 0.65f, [this](auto) {
            FRGRDPopup::create(m_grandpaDemonOverride, [this](int grd) {
                m_grandpaDemonOverride = grd;
                m_demonsInBetweenOverride = 0;
                m_gddpIntegrationOverride = 0;
            })->show();
        }));

    if (Loader::get()->isModLoaded("hiimjustin000.demons_in_between"))
        overrideMenu->addChild(CCMenuItemExt::createSpriteExtraWithFilename("FR_dibBtn_001.png"_spr, 0.75f, [this](auto) {
            FRDIBPopup::create(m_demonsInBetweenOverride, [this](int dib) {
                m_grandpaDemonOverride = 0;
                m_demonsInBetweenOverride = dib;
                m_gddpIntegrationOverride = 0;
            })->show();
        }));

    if (Loader::get()->isModLoaded("minemaker0430.gddp_integration"))
        overrideMenu->addChild(CCMenuItemExt::createSpriteExtraWithFilename("FR_gddpBtn_001.png"_spr, 0.65f, [this](auto) {
            FRGDDPPopup::create(m_gddpIntegrationOverride, [this](int gddp) {
                m_grandpaDemonOverride = 0;
                m_demonsInBetweenOverride = 0;
                m_gddpIntegrationOverride = gddp;
            })->show();
        }));

    overrideMenu->updateLayout();

    return true;
}

FRSetStarsPopup* FRSetStarsPopup::create(int stars, bool platformer, SetIntCallback callback) {
    auto ret = new FRSetStarsPopup();
    if (ret->initAnchored(250.0f, 150.0f, stars, platformer, callback)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool FRSetStarsPopup::setup(int stars, bool platformer, SetIntCallback callback) {
    setTitle(platformer ? "Set Moons" : "Set Stars");
    m_noElasticity = true;
    m_stars = stars;

    m_input = TextInput::create(150.0f, platformer ? "Moons" : "Stars");
    m_input->setCommonFilter(CommonFilter::Int);
    m_input->setPosition(125.0f, 80.0f);
    m_input->getInputNode()->setLabelPlaceholderColor({ 120, 170, 240 });
    m_input->setString(std::to_string(m_stars));
    m_input->setMaxCharCount(11);
    m_input->setCallback([this](std::string const& text) {
        auto stars = numFromString<int64_t>(text).unwrapOr(0);
        if (stars < INT_MIN) stars = INT_MIN;
        if (stars > INT_MAX) stars = INT_MAX;
        m_stars = stars;
        m_label->setString(std::to_string(m_stars).c_str());
        m_starLayout->updateLayout();
    });
    m_mainLayer->addChild(m_input);

    m_starLayout = CCNode::create();
    m_starLayout->setPosition(125.0f, 52.5f);
    m_starLayout->setContentSize({ 250.0f, 15.0f });
    m_starLayout->setAnchorPoint({ 0.5f, 0.5f });
    m_starLayout->setLayout(RowLayout::create()->setGap(1.75f)->setAutoScale(false));
    m_mainLayer->addChild(m_starLayout);

    m_label = CCLabelBMFont::create(std::to_string(m_stars).c_str(), "bigFont.fnt");
    m_label->setScale(0.4f);
    m_starLayout->addChild(m_label);

    m_starLayout->addChild(CCSprite::createWithSpriteFrameName(platformer ? "moon_small01_001.png" : "star_small01_001.png"));

    m_starLayout->updateLayout();

    auto leftButton = CCMenuItemExt::createSpriteExtraWithFrameName("edit_leftBtn_001.png", 1.1f, [this](auto) {
        if (m_stars != INT_MIN) m_stars -= 1;
        auto stars = std::to_string(m_stars);
        m_input->setString(stars);
        m_label->setString(stars.c_str());
        m_starLayout->updateLayout();
    });
    leftButton->setPosition(30.0f, 80.0f);
    m_buttonMenu->addChild(leftButton);

    auto rightButton = CCMenuItemExt::createSpriteExtraWithFrameName("edit_rightBtn_001.png", 1.1f, [this](auto) {
        if (m_stars != INT_MAX) m_stars += 1;
        auto stars = std::to_string(m_stars);
        m_input->setString(stars);
        m_label->setString(stars.c_str());
        m_starLayout->updateLayout();
    });
    rightButton->setPosition(220.0f, 80.0f);
    m_buttonMenu->addChild(rightButton);

    auto confirmButton = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("Confirm", "goldFont.fnt", "GJ_button_01.png", 0.8f), [this, callback](auto) {
        callback(m_stars);
        onClose(nullptr);
    });
    confirmButton->setPosition(125.0f, 25.0f);
    m_buttonMenu->addChild(confirmButton);

    return true;
}

FRSetFeaturePopup* FRSetFeaturePopup::create(FakeRateSaveData data, bool legacy, SetIntCallback callback) {
    auto ret = new FRSetFeaturePopup();
    if (ret->initAnchored(300.0f, 150.0f, data, legacy, callback)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool FRSetFeaturePopup::setup(FakeRateSaveData data, bool legacy, SetIntCallback callback) {
    setTitle("Select Feature");
    m_noElasticity = true;
    m_feature = static_cast<GJFeatureState>(data.feature);
    m_difficulty = data.difficulty;
    m_moreDifficultiesOverride = data.moreDifficultiesOverride;
    m_grandpaDemonOverride = data.grandpaDemonOverride;
    m_demonsInBetweenOverride = data.demonsInBetweenOverride;
    m_gddpIntegrationOverride = data.gddpIntegrationOverride;
    m_legacy = legacy;

    auto menuRow = CCMenu::create();
    menuRow->setLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::Even));
    menuRow->setPosition(150.0f, 80.0f + (data.difficulty > 5 || data.grandpaDemonOverride > 0 ||
        data.demonsInBetweenOverride > 0 || data.gddpIntegrationOverride > 0 ? 5.0f : 0.0f));
    menuRow->setContentSize({ 300.0f, 50.0f });
    m_mainLayer->addChild(menuRow);

    for (int i = 0; i < 5; i++) {
        auto feature = static_cast<GJFeatureState>(i);
        auto difficultySprite = GJDifficultySprite::create(m_difficulty, GJDifficultyName::Long);
        difficultySprite->updateFeatureState(feature);
        if (Loader::get()->isModLoaded("uproxide.animated_fire") && i > 1) {
            auto fire = FireSprite::create((GJFeatureState)i);
            fire->setPosition(difficultySprite->getContentSize() / 2 + CCPoint { 0.0f, i == 2 ? 16.875f : 15.875f });
            if (auto oldFire = getChildBySpriteFrameName(difficultySprite, (i == 2 ? "GJ_epicCoin_001.png" :
                fmt::format("GJ_epicCoin{}_001.png", i - 1)).c_str())) oldFire->setVisible(false);
            difficultySprite->addChild(fire, -1);
        }
        if (Loader::get()->isModLoaded("uproxide.more_difficulties") && m_moreDifficultiesOverride > 0
            && m_grandpaDemonOverride == 0 && m_demonsInBetweenOverride == 0) {
            auto mdSprite = CCSprite::createWithSpriteFrameName((m_legacy ?
                fmt::format("uproxide.more_difficulties/MD_Difficulty{:02d}_Legacy.png", m_moreDifficultiesOverride)
                : fmt::format("uproxide.more_difficulties/MD_Difficulty{:02d}.png", m_moreDifficultiesOverride)).c_str());
            mdSprite->setPosition(difficultySprite->getContentSize() / 2 + (m_legacy ? CCPoint { 0.0f, 0.0f } : CCPoint { 0.25f, -0.1f }));
            difficultySprite->setOpacity(0);
            difficultySprite->addChild(mdSprite);
        }
        if (Loader::get()->isModLoaded("itzkiba.grandpa_demon") && m_grandpaDemonOverride > 0) {
            auto grdSprite = CCSprite::createWithSpriteFrameName(fmt::format("itzkiba.grandpa_demon/GrD_demon{}_text.png", m_grandpaDemonOverride - 1).c_str());
            grdSprite->setPosition(difficultySprite->getContentSize() / 2);
            difficultySprite->setOpacity(0);
            difficultySprite->addChild(grdSprite);
        }
        if (Loader::get()->isModLoaded("hiimjustin000.demons_in_between") && m_demonsInBetweenOverride > 0) {
            auto demonsInBetween = Loader::get()->getLoadedMod("hiimjustin000.demons_in_between");
            auto dibFeature = "";
            if (i == 3 && demonsInBetween->getSettingValue<bool>("enable-legendary")) dibFeature = "_4";
            else if (i == 4 && demonsInBetween->getSettingValue<bool>("enable-mythic")) dibFeature = "_5";
            auto dibSprite = CCSprite::createWithSpriteFrameName(fmt::format("hiimjustin000.demons_in_between/DIB_{:02d}{}_btn2_001.png",
                m_demonsInBetweenOverride, dibFeature).c_str());
            dibSprite->setPosition(difficultySprite->getContentSize() / 2 + FakeRate::getDIBOffset(m_demonsInBetweenOverride, GJDifficultyName::Long));
            difficultySprite->setOpacity(0);
            difficultySprite->addChild(dibSprite);
        }
        if (Loader::get()->isModLoaded("minemaker0430.gddp_integration") && m_gddpIntegrationOverride > 0) {
            auto gddpSprite = CCSprite::createWithSpriteFrameName(FakeRate::getGDDPFrame(m_gddpIntegrationOverride, GJDifficultyName::Long).c_str());
            gddpSprite->setAnchorPoint({ 0.5f, 1.0f });
            gddpSprite->setPosition(difficultySprite->getContentSize() / 2 + CCPoint { 0.25f, 30.0f });
            difficultySprite->setOpacity(0);
            difficultySprite->addChild(gddpSprite);
        }
        auto toggle = CCMenuItemExt::createSpriteExtra(difficultySprite, [this, feature](CCMenuItemSpriteExtra* sender) {
            if (sender == m_selected) return;
            m_feature = feature;
            FakeRate::toggle(m_selected->getNormalImage(), false);
            if (auto particleSystem = getChildOfType<CCParticleSystemQuad>(m_selected->getNormalImage(), 0)) particleSystem->setVisible(false);
            FakeRate::toggle(sender->getNormalImage(), true);
            if (auto particleSystem = getChildOfType<CCParticleSystemQuad>(sender->getNormalImage(), 0)) particleSystem->setVisible(true);
            m_selected = sender;
        });
        FakeRate::toggle(difficultySprite, feature == m_feature);
        if (auto particleSystem = getChildOfType<CCParticleSystemQuad>(difficultySprite, 0)) particleSystem->setVisible(feature == m_feature);
        m_selected = feature == m_feature ? toggle : m_selected;
        menuRow->addChild(toggle);
    }

    menuRow->updateLayout();

    auto confirmButton = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("Confirm", "goldFont.fnt", "GJ_button_01.png", 0.8f), [this, callback](auto) {
        callback(static_cast<int>(m_feature));
        onClose(nullptr);
    });
    confirmButton->setPosition(150.0f, 25.0f);
    m_buttonMenu->addChild(confirmButton);

    return true;
}

FRGRDPopup* FRGRDPopup::create(int grandpaDemonOverride, SetIntCallback callback) {
    auto ret = new FRGRDPopup();
    if (ret->initAnchored(250.0f, 200.0f, grandpaDemonOverride, callback)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool FRGRDPopup::setup(int grandpaDemonOverride, SetIntCallback callback) {
    setTitle("Grandpa Demon");
    m_noElasticity = true;
    m_grandpaDemonOverride = grandpaDemonOverride;

    auto table = TableNode::create(3, 2);
    table->setColumnLayout(ColumnLayout::create()->setAxisReverse(true));
    table->setRowLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::Even));
    table->setRowHeight(65.0f);
    table->setContentSize({ 250.0f, 130.0f });
    table->setPosition(125.0f, 107.5f);
    m_mainLayer->addChild(table);

    for (int i = 1; i < 7; i++) {
        auto toggle = CCMenuItemExt::createSpriteExtraWithFrameName(fmt::format("itzkiba.grandpa_demon/GrD_demon{}_text.png", i - 1).c_str(), 1.0f,
            [this, i](CCMenuItemSpriteExtra* sender) {
                m_grandpaDemonOverride = sender != m_selected ? i : 0;
                if (m_selected) FakeRate::toggle(m_selected->getNormalImage(), false);
                if (sender != m_selected) FakeRate::toggle(sender->getNormalImage(), true);
                m_selected = sender != m_selected ? sender : nullptr;
            });
        FakeRate::toggle(toggle->getNormalImage(), i == m_grandpaDemonOverride);
        m_selected = i == m_grandpaDemonOverride ? toggle : m_selected;
        table->addButton(toggle);
    }

    table->updateAllLayouts();

    auto confirmButton = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("Confirm", "goldFont.fnt", "GJ_button_01.png", 0.8f), [this, callback](auto) {
        callback(m_grandpaDemonOverride);
        onClose(nullptr);
    });
    confirmButton->setPosition(125.0f, 25.0f);
    m_buttonMenu->addChild(confirmButton);

    return true;
}

FRDIBPopup* FRDIBPopup::create(int demonsInBetweenOverride, SetIntCallback callback) {
    auto ret = new FRDIBPopup();
    if (ret->initAnchored(350.0f, 310.0f, demonsInBetweenOverride, callback)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool FRDIBPopup::setup(int demonsInBetweenOverride, SetIntCallback callback) {
    setTitle("Demons In Between");
    m_noElasticity = true;
    m_demonsInBetweenOverride = demonsInBetweenOverride;

    auto table = TableNode::create(5, 4);
    table->setColumnLayout(ColumnLayout::create()->setAxisReverse(true));
    table->setRowLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::Even));
    table->setRowHeight(60.0f);
    table->setContentSize({ 350.0f, 240.0f });
    table->setPosition(175.0f, 160.0f);
    m_mainLayer->addChild(table);

    for (int i = 1; i < 21; i++) {
        auto toggle = CCMenuItemExt::createSpriteExtraWithFrameName(fmt::format("hiimjustin000.demons_in_between/DIB_{:02}_btn2_001.png", i).c_str(), 1.0f,
            [this, i](CCMenuItemSpriteExtra* sender) {
                m_demonsInBetweenOverride = sender != m_selected ? i : 0;
                if (m_selected) FakeRate::toggle(m_selected->getNormalImage(), false);
                if (sender != m_selected) FakeRate::toggle(sender->getNormalImage(), true);
                m_selected = sender != m_selected ? sender : nullptr;
            });
        FakeRate::toggle(toggle->getNormalImage(), i == m_demonsInBetweenOverride);
        m_selected = i == m_demonsInBetweenOverride ? toggle : m_selected;
        table->addButton(toggle);
    }

    table->updateAllLayouts();

    auto confirmButton = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("Confirm", "goldFont.fnt", "GJ_button_01.png", 0.8f), [this, callback](auto) {
        callback(m_demonsInBetweenOverride);
        onClose(nullptr);
    });
    confirmButton->setPosition(175.0f, 25.0f);
    m_buttonMenu->addChild(confirmButton);

    return true;
}

FRGDDPPopup* FRGDDPPopup::create(int gddpIntegrationOverride, SetIntCallback callback) {
    auto ret = new FRGDDPPopup();
    if (ret->initAnchored(350.0f, 250.0f, gddpIntegrationOverride, callback)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool FRGDDPPopup::setup(int gddpIntegrationOverride, SetIntCallback callback) {
    setTitle("GDDP Integration");
    m_noElasticity = true;
    m_gddpIntegrationOverride = gddpIntegrationOverride;

    auto table = TableNode::create(5, 3);
    table->setColumnLayout(ColumnLayout::create()->setAxisReverse(true));
    table->setRowLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::Even));
    table->setRowHeight(60.0f);
    table->setContentSize({ 350.0f, 180.0f });
    table->setPosition(175.0f, 130.0f);
    m_mainLayer->addChild(table);

    for (int i = 1; i < 16; i++) {
        auto toggle = CCMenuItemExt::createSpriteExtraWithFrameName(FakeRate::getGDDPFrame(i, GJDifficultyName::Long).c_str(), 1.0f,
            [this, i](CCMenuItemSpriteExtra* sender) {
                m_gddpIntegrationOverride = sender != m_selected ? i : 0;
                if (m_selected) FakeRate::toggle(m_selected->getNormalImage(), false);
                if (sender != m_selected) FakeRate::toggle(sender->getNormalImage(), true);
                m_selected = sender != m_selected ? sender : nullptr;
            });
        FakeRate::toggle(toggle->getNormalImage(), i == m_gddpIntegrationOverride);
        m_selected = i == m_gddpIntegrationOverride ? toggle : m_selected;
        table->addButton(toggle);
    }

    table->updateAllLayouts();

    auto confirmButton = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("Confirm", "goldFont.fnt", "GJ_button_01.png", 0.8f), [this, callback](auto) {
        callback(m_gddpIntegrationOverride);
        onClose(nullptr);
    });
    confirmButton->setPosition(175.0f, 25.0f);
    m_buttonMenu->addChild(confirmButton);

    return true;
}
