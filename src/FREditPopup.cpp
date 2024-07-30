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
    m_coins = data.coins;
    m_legacy = false;

    m_difficultySprite = GJDifficultySprite::create(data.difficulty, GJDifficultyName::Long);
    m_difficultySprite->setPositionX(60.0f);
    m_mainLayer->addChild(m_difficultySprite);

    if (Loader::get()->isModLoaded("uproxide.more_difficulties")) {
        m_legacy = Loader::get()->getLoadedMod("uproxide.more_difficulties")->getSettingValue<bool>("legacy-difficulties");
        m_mdSprite = CCSprite::createWithSpriteFrameName(m_legacy ? "uproxide.more_difficulties/MD_Difficulty04_Legacy.png" : "uproxide.more_difficulties/MD_Difficulty04.png");
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
        m_mainLayer->addChild(m_grdInfinity, 30);
    }
    if (Loader::get()->isModLoaded("hiimjustin000.demons_in_between")) {
        m_dibSprite = CCSprite::createWithSpriteFrameName("hiimjustin000.demons_in_between/DIB_01_btn2_001.png");
        m_dibSprite->setPosition({ 60.0f, 105.0f });
        m_dibSprite->setVisible(false);
        m_mainLayer->addChild(m_dibSprite);
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
        FRSetDifficultyPopup::create(m_difficulty, m_moreDifficultiesOverride, m_grandpaDemonOverride, m_demonsInBetweenOverride, m_legacy,
            [this](int difficulty, int mdo, int gdo, int dbo) {
                m_difficulty = difficulty;
                m_moreDifficultiesOverride = mdo;
                m_grandpaDemonOverride = gdo;
                m_demonsInBetweenOverride = dbo;
                updateLabels();
            })->show();
    });
    difficultyButton->setPosition(200.0f, 150.0f);
    m_buttonMenu->addChild(difficultyButton);

    auto starsButton = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("Stars", "goldFont.fnt", "GJ_button_02.png", 0.8f), [this, data](auto) {
        auto popup = SetIDPopup::create(m_stars, -128, 127, "Set Stars", "Set", true, data.stars, 0.0f, false, false);
        popup->m_inputNode->setAllowedChars("0123456789-");
        popup->m_delegate = this;
        popup->show();
    });
    starsButton->setPosition(200.0f, 110.0f);
    m_buttonMenu->addChild(starsButton);

    auto featureButton = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("Feature", "goldFont.fnt", "GJ_button_02.png", 0.8f), [this](auto) {
        FRSetFeaturePopup::create(m_feature, m_difficulty, m_moreDifficultiesOverride, m_grandpaDemonOverride, m_demonsInBetweenOverride, m_legacy,
            [this](int feature) {
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
        auto it = std::find_if(vec.begin(), vec.end(), [this](FakeRateSaveData const& item) {
            return item.id == m_level->m_levelID;
        });
        if (it != vec.end()) {
            it->stars = m_stars;
            it->feature = m_feature;
            it->difficulty = m_difficulty;
            it->moreDifficultiesOverride = m_moreDifficultiesOverride;
            it->grandpaDemonOverride = m_grandpaDemonOverride;
            it->demonsInBetweenOverride = m_demonsInBetweenOverride;
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
            .coins = m_level->m_coinsVerified > 0
        }, true);
        onClose(nullptr);
    });
    removeButton->setPosition(235.0f, 30.0f);
    m_buttonMenu->addChild(removeButton);

    updateLabels();

    return true;
}

void FREditPopup::setIDPopupClosed(SetIDPopup*, int stars) {
    m_stars = stars;
    updateLabels();
}

void FREditPopup::updateLabels() {
    m_difficultySprite->updateFeatureState((GJFeatureState)m_feature);
    m_difficultySprite->updateDifficultyFrame(m_difficulty, GJDifficultyName::Long);
    m_difficultySprite->setPositionY(100.0f + (m_difficulty > 5 ? 5.0f : 0.0f) + (m_stars != 0 ? 10.0f : 0.0f));
    m_starSprite->setPosition({ m_difficultySprite->getPositionX() + 8.0f, m_difficultySprite->getPositionY() - 30.0f - (m_difficulty > 5 ? 9.0f : 0.0f) });
    m_starSprite->setVisible(m_stars != 0);
    m_starsLabel->setPosition(m_starSprite->getPositionX() - 8.0f, m_starSprite->getPositionY());
    m_starsLabel->setString(std::to_string(m_stars).c_str());
    m_starsLabel->setVisible(m_stars != 0);
    auto coins = reinterpret_cast<CCSprite**>(m_coinSprites->data->arr);
    for (int i = 0; i < m_level->m_coins; i++) {
        coins[i]->setPositionY(m_difficultySprite->getPositionY() - 31.5f - (m_stars != 0 ? 14.0f : 0.0f) - (m_difficulty > 5 ? 9.0f : 0.0f));
        coins[i]->setColor(m_coins ? ccColor3B { 255, 255, 255 } : ccColor3B { 255, 175, 75 });
    }
    m_difficultySprite->setOpacity(255);
    if (Loader::get()->isModLoaded("uproxide.more_difficulties")) {
        if (m_moreDifficultiesOverride == 4 || m_moreDifficultiesOverride == 7 || m_moreDifficultiesOverride == 9) {
            m_mdSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(
                fmt::format("uproxide.more_difficulties/MD_Difficulty{:02d}.png{}", m_moreDifficultiesOverride, m_legacy ? "_Legacy" : "").c_str()));
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
            m_grdInfinity->setPosition(m_difficultySprite->getPosition() + CCPoint { -0.4f, 14.0f });
            m_grdInfinity->setVisible(m_grandpaDemonOverride == 5);
            m_difficultySprite->setOpacity(0);
        }
        else m_grdSprite->setVisible(false);
    }
    if (Loader::get()->isModLoaded("hiimjustin000.demons_in_between")) {
        if (m_demonsInBetweenOverride > 0 && m_demonsInBetweenOverride < 21) {
            m_dibSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(
                fmt::format("hiimjustin000.demons_in_between/DIB_{:02d}_btn2_001.png", m_demonsInBetweenOverride).c_str()));
            m_dibSprite->setPosition(m_difficultySprite->getPosition() + FakeRate::getDIBOffset(m_demonsInBetweenOverride, GJDifficultyName::Long));
            m_dibSprite->setVisible(true);
            m_difficultySprite->setOpacity(0);
        }
        else m_dibSprite->setVisible(false);
    }
}

FREditPopup::~FREditPopup() {
    m_coinSprites->release();
}

FRSetDifficultyPopup* FRSetDifficultyPopup::create(int difficulty, int mdo, int gdo, int dbo, bool legacy, SetDifficultyCallback callback) {
    auto ret = new FRSetDifficultyPopup();
    if (ret->initAnchored(300.0f, 250.0f, difficulty, mdo, gdo, dbo, legacy, callback)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool FRSetDifficultyPopup::setup(int difficulty, int mdo, int gdo, int dbo, bool legacy, SetDifficultyCallback callback) {
    setTitle("Select Difficulty");
    m_difficulty = difficulty;
    m_moreDifficultiesOverride = mdo;
    m_grandpaDemonOverride = gdo;
    m_demonsInBetweenOverride = dbo;
    m_legacy = legacy;

    auto menuRow1 = CCMenu::create();
    menuRow1->setLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::Even));
    menuRow1->setPosition(150.0f, 185.0f);
    menuRow1->setContentSize({ 300.0f, 45.0f });
    m_mainLayer->addChild(menuRow1);

    auto menuRow2 = CCMenu::create();
    menuRow2->setLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::Even));
    menuRow2->setPosition(150.0f, 135.0f);
    menuRow2->setContentSize({ 300.0f, 45.0f });
    m_mainLayer->addChild(menuRow2);

    auto menuRow3 = CCMenu::create();
    menuRow3->setLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::Even));
    menuRow3->setPosition(150.0f, 80.0f);
    menuRow3->setContentSize({ 300.0f, 65.0f });
    menuRow3->setTag(3);
    m_mainLayer->addChild(menuRow3);

    createDifficultyToggle(menuRow1, 0, 0);
    createDifficultyToggle(menuRow1, -1, 0);
    createDifficultyToggle(menuRow1, 1, 0);
    createDifficultyToggle(menuRow1, 2, 0);
    createDifficultyToggle(menuRow1, 3, 4);
    menuRow1->updateLayout();

    createDifficultyToggle(menuRow2, 3, 0);
    createDifficultyToggle(menuRow2, 4, 0);
    createDifficultyToggle(menuRow2, 4, 7);
    createDifficultyToggle(menuRow2, 5, 0);
    createDifficultyToggle(menuRow2, 5, 9);
    menuRow2->updateLayout();

    createDifficultyToggle(menuRow3, 7, 0);
    createDifficultyToggle(menuRow3, 8, 0);
    createDifficultyToggle(menuRow3, 6, 0);
    createDifficultyToggle(menuRow3, 9, 0);
    createDifficultyToggle(menuRow3, 10, 0);
    menuRow3->updateLayout();

    auto confirmButton = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("Confirm", "goldFont.fnt", "GJ_button_01.png", 0.8f), [this, callback](auto) {
        callback(m_difficulty, m_moreDifficultiesOverride, m_grandpaDemonOverride, m_demonsInBetweenOverride);
        onClose(nullptr);
    });
    confirmButton->setPosition(150.0f, 25.0f);
    m_buttonMenu->addChild(confirmButton);

    auto demonsInBetween = Loader::get()->isModLoaded("hiimjustin000.demons_in_between");
    if (Loader::get()->isModLoaded("itzkiba.grandpa_demon")) {
        auto grdButton = CCMenuItemExt::createSpriteExtraWithFilename("FR_grdBtn_001.png"_spr, 0.65f, [this](auto) {
            FRGRDPopup::create(m_grandpaDemonOverride, [this](int grd) {
                m_difficulty = 10;
                m_grandpaDemonOverride = grd;
                m_demonsInBetweenOverride = 0;
                auto sender = static_cast<CCMenuItemSpriteExtra*>(m_mainLayer->getChildByTag(3)->getChildByTag(10));
                FakeRate::toggle(m_selected->getNormalImage(), false);
                FakeRate::toggle(sender->getNormalImage(), true);
                m_selected = sender;
            })->show();
        });
        grdButton->setPosition(275.0f - (demonsInBetween ? 30.0f : 0.0f), 25.0f);
        m_buttonMenu->addChild(grdButton);
    }
    if (demonsInBetween) {
        auto dibButton = CCMenuItemExt::createSpriteExtraWithFilename("FR_dibBtn_001.png"_spr, 0.75f, [this](auto) {
            FRDIBPopup::create(m_difficulty, m_demonsInBetweenOverride, [this](int diff, int dib) {
                m_difficulty = diff;
                m_grandpaDemonOverride = 0;
                m_demonsInBetweenOverride = dib;
                auto sender = static_cast<CCMenuItemSpriteExtra*>(m_mainLayer->getChildByTag(3)->getChildByTag(diff));
                FakeRate::toggle(m_selected->getNormalImage(), false);
                FakeRate::toggle(sender->getNormalImage(), true);
                m_selected = sender;
            })->show();
        });
        dibButton->setPosition(275.0f, 25.0f);
        m_buttonMenu->addChild(dibButton);
    }

    return true;
}

void FRSetDifficultyPopup::createDifficultyToggle(CCMenu* menu, int difficulty, int moreDifficultiesOverride) {
    auto num = difficulty == -1 ? "auto" : fmt::format("{:02d}", difficulty);
    auto frameName = difficulty > 5 ? fmt::format("difficulty_{}_btn2_001.png", num) : fmt::format("difficulty_{}_btn_001.png", num);
    if (Loader::get()->isModLoaded("uproxide.more_difficulties") && moreDifficultiesOverride > 0)
        frameName = m_legacy ? fmt::format("uproxide.more_difficulties/MD_Difficulty{:02d}_Legacy.png", moreDifficultiesOverride)
            : fmt::format("uproxide.more_difficulties/MD_Difficulty{:02d}.png", moreDifficultiesOverride);
    else if (moreDifficultiesOverride > 0) return;
    auto toggle = CCMenuItemExt::createSpriteExtraWithFrameName(frameName.c_str(), 1.0f, [this, difficulty, moreDifficultiesOverride](CCMenuItemSpriteExtra* sender) {
        if (sender == m_selected) return;
        m_difficulty = difficulty;
        m_moreDifficultiesOverride = moreDifficultiesOverride;
        if (m_difficulty < 10 && m_grandpaDemonOverride > 0) m_grandpaDemonOverride = 0;
        if (m_difficulty < 6 && m_demonsInBetweenOverride > 0) m_demonsInBetweenOverride = 0;
        else if (m_difficulty > 5 && m_demonsInBetweenOverride > 0) switch (m_difficulty) {
            case 7: m_demonsInBetweenOverride = 4; break;
            case 8: m_demonsInBetweenOverride = 7; break;
            case 6: m_demonsInBetweenOverride = 11; break;
            case 9: m_demonsInBetweenOverride = 15; break;
            case 10: m_demonsInBetweenOverride = 20; break;
            default: m_demonsInBetweenOverride = 0; break;
        }
        FakeRate::toggle(m_selected->getNormalImage(), false);
        FakeRate::toggle(sender->getNormalImage(), true);
        m_selected = sender;
    });
    if (difficulty > 5) toggle->setTag(difficulty);
    auto isToggled = moreDifficultiesOverride == m_moreDifficultiesOverride && (m_moreDifficultiesOverride <= 0 ? difficulty == m_difficulty : true);
    FakeRate::toggle(toggle->getNormalImage(), isToggled);
    m_selected = isToggled ? toggle : m_selected;
    menu->addChild(toggle);
}

FRSetFeaturePopup* FRSetFeaturePopup::create(int feature, int difficulty, int mdo, int gdo, int dbo, bool legacy, SetFeatureCallback callback) {
    auto ret = new FRSetFeaturePopup();
    if (ret->initAnchored(300.0f, 150.0f, feature, difficulty, mdo, gdo, dbo, legacy, callback)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool FRSetFeaturePopup::setup(int feature, int difficulty, int mdo, int gdo, int dbo, bool legacy, SetFeatureCallback callback) {
    setTitle("Select Feature");
    m_feature = static_cast<GJFeatureState>(feature);
    m_difficulty = difficulty;
    m_moreDifficultiesOverride = mdo;
    m_grandpaDemonOverride = gdo;
    m_demonsInBetweenOverride = dbo;
    m_legacy = legacy;

    auto menuRow = CCMenu::create();
    menuRow->setLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::Even));
    menuRow->setPosition(150.0f, 80.0f + (difficulty > 5 ? 5.0f : 0.0f));
    menuRow->setContentSize({ 300.0f, 50.0f });
    m_mainLayer->addChild(menuRow);

    createFeatureToggle(menuRow, GJFeatureState::None);
    createFeatureToggle(menuRow, GJFeatureState::Featured);
    createFeatureToggle(menuRow, GJFeatureState::Epic);
    createFeatureToggle(menuRow, GJFeatureState::Legendary);
    createFeatureToggle(menuRow, GJFeatureState::Mythic);
    menuRow->updateLayout();

    auto confirmButton = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("Confirm", "goldFont.fnt", "GJ_button_01.png", 0.8f), [this, callback](auto) {
        callback(static_cast<int>(m_feature));
        onClose(nullptr);
    });
    confirmButton->setPosition(150.0f, 25.0f);
    m_buttonMenu->addChild(confirmButton);

    return true;
}

void FRSetFeaturePopup::createFeatureToggle(CCMenu* menu, GJFeatureState feature) {
    auto difficultySprite = GJDifficultySprite::create(m_difficulty, GJDifficultyName::Long);
    difficultySprite->updateFeatureState(feature);
    if (Loader::get()->isModLoaded("uproxide.more_difficulties") && m_moreDifficultiesOverride > 0) {
        auto mdSprite = CCSprite::createWithSpriteFrameName((m_legacy ? fmt::format("uproxide.more_difficulties/MD_Difficulty{:02d}_Legacy.png", m_moreDifficultiesOverride)
            : fmt::format("uproxide.more_difficulties/MD_Difficulty{:02d}.png", m_moreDifficultiesOverride)).c_str());
        difficultySprite->addChild(mdSprite);
        mdSprite->setPosition(difficultySprite->getContentSize() / 2 + (m_legacy ? CCPoint { 0.0f, 0.0f } : CCPoint { 0.25f, -0.1f }));
        difficultySprite->setOpacity(0);
    }
    if (Loader::get()->isModLoaded("itzkiba.grandpa_demon") && m_grandpaDemonOverride > 0) {
        auto grdSprite = CCSprite::createWithSpriteFrameName(fmt::format("itzkiba.grandpa_demon/GrD_demon{}_text.png", m_grandpaDemonOverride - 1).c_str());
        difficultySprite->addChild(grdSprite);
        grdSprite->setPosition(difficultySprite->getContentSize() / 2);
        difficultySprite->setOpacity(0);
    }
    if (Loader::get()->isModLoaded("hiimjustin000.demons_in_between") && m_demonsInBetweenOverride > 0) {
        auto dibSprite = CCSprite::createWithSpriteFrameName(fmt::format("hiimjustin000.demons_in_between/DIB_{:02d}_btn2_001.png", m_demonsInBetweenOverride).c_str());
        difficultySprite->addChild(dibSprite);
        dibSprite->setPosition(difficultySprite->getContentSize() / 2 + FakeRate::getDIBOffset(m_demonsInBetweenOverride, GJDifficultyName::Long));
        difficultySprite->setOpacity(0);
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
    menu->addChild(toggle);
}

FRGRDPopup* FRGRDPopup::create(int grandpaDemonOverride, SetGRDCallback callback) {
    auto ret = new FRGRDPopup();
    if (ret->initAnchored(250.0f, 200.0f, grandpaDemonOverride, callback)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool FRGRDPopup::setup(int grandpaDemonOverride, SetGRDCallback callback) {
    setTitle("Grandpa Demon");
    m_grandpaDemonOverride = grandpaDemonOverride;

    auto menuRow1 = CCMenu::create();
    menuRow1->setLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::Even));
    menuRow1->setPosition(125.0f, 140.0f);
    menuRow1->setContentSize({ 250.0f, 65.0f });
    m_mainLayer->addChild(menuRow1);

    createGRDToggle(menuRow1, 1);
    createGRDToggle(menuRow1, 2);
    createGRDToggle(menuRow1, 3);
    menuRow1->updateLayout();

    auto menuRow2 = CCMenu::create();
    menuRow2->setLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::Even));
    menuRow2->setPosition(125.0f, 75.0f);
    menuRow2->setContentSize({ 250.0f, 65.0f });
    m_mainLayer->addChild(menuRow2);

    createGRDToggle(menuRow2, 4);
    createGRDToggle(menuRow2, 5);
    createGRDToggle(menuRow2, 6);
    menuRow2->updateLayout();

    auto confirmButton = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("Confirm", "goldFont.fnt", "GJ_button_01.png", 0.8f), [this, callback](auto) {
        callback(m_grandpaDemonOverride);
        onClose(nullptr);
    });
    confirmButton->setPosition(125.0f, 25.0f);
    m_buttonMenu->addChild(confirmButton);

    return true;
}

void FRGRDPopup::createGRDToggle(CCMenu* menu, int grandpaDemonOverride) {
    auto toggle = CCMenuItemExt::createSpriteExtraWithFrameName(fmt::format("itzkiba.grandpa_demon/GrD_demon{}_text.png", grandpaDemonOverride - 1).c_str(), 1.0f,
        [this, grandpaDemonOverride](CCMenuItemSpriteExtra* sender) {
            if (sender == m_selected) return;
            m_grandpaDemonOverride = grandpaDemonOverride;
            if (m_selected) FakeRate::toggle(m_selected->getNormalImage(), false);
            FakeRate::toggle(sender->getNormalImage(), true);
            m_selected = sender;
        });
    FakeRate::toggle(toggle->getNormalImage(), grandpaDemonOverride == m_grandpaDemonOverride);
    m_selected = grandpaDemonOverride == m_grandpaDemonOverride ? toggle : m_selected;
    menu->addChild(toggle);
}

FRDIBPopup* FRDIBPopup::create(int difficulty, int demonsInBetweenOverride, SetDIBCallback callback) {
    auto ret = new FRDIBPopup();
    if (ret->initAnchored(350.0f, 310.0f, difficulty, demonsInBetweenOverride, callback)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool FRDIBPopup::setup(int difficulty, int demonsInBetweenOverride, SetDIBCallback callback) {
    setTitle("Demons In Between");
    m_difficulty = difficulty;
    m_demonsInBetweenOverride = demonsInBetweenOverride;
    if (demonsInBetweenOverride <= 0) switch (m_difficulty) {
        case 7: m_demonsInBetweenOverride = 4; break;
        case 8: m_demonsInBetweenOverride = 7; break;
        case 6: m_demonsInBetweenOverride = 11; break;
        case 9: m_demonsInBetweenOverride = 15; break;
        case 10: m_demonsInBetweenOverride = 20; break;
        default: m_demonsInBetweenOverride = 0; break;
    }

    auto menuRow1 = CCMenu::create();
    menuRow1->setLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::Even));
    menuRow1->setPosition(175.0f, 250.0f);
    menuRow1->setContentSize({ 350.0f, 60.0f });
    m_mainLayer->addChild(menuRow1);

    createDIBToggle(menuRow1, 1, 0);
    createDIBToggle(menuRow1, 2, 0);
    createDIBToggle(menuRow1, 3, 0);
    createDIBToggle(menuRow1, 4, 7);
    createDIBToggle(menuRow1, 5, 0);
    menuRow1->updateLayout();

    auto menuRow2 = CCMenu::create();
    menuRow2->setLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::Even));
    menuRow2->setPosition(175.0f, 190.0f);
    menuRow2->setContentSize({ 350.0f, 60.0f });
    m_mainLayer->addChild(menuRow2);

    createDIBToggle(menuRow2, 6, 0);
    createDIBToggle(menuRow2, 7, 8);
    createDIBToggle(menuRow2, 8, 0);
    createDIBToggle(menuRow2, 9, 0);
    createDIBToggle(menuRow2, 10, 0);
    menuRow2->updateLayout();

    auto menuRow3 = CCMenu::create();
    menuRow3->setLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::Even));
    menuRow3->setPosition(175.0f, 130.0f);
    menuRow3->setContentSize({ 350.0f, 60.0f });
    m_mainLayer->addChild(menuRow3);

    createDIBToggle(menuRow3, 11, 6);
    createDIBToggle(menuRow3, 12, 0);
    createDIBToggle(menuRow3, 13, 0);
    createDIBToggle(menuRow3, 14, 0);
    createDIBToggle(menuRow3, 15, 9);
    menuRow3->updateLayout();

    auto menuRow4 = CCMenu::create();
    menuRow4->setLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::Even));
    menuRow4->setPosition(175.0f, 70.0f);
    menuRow4->setContentSize({ 350.0f, 60.0f });
    m_mainLayer->addChild(menuRow4);

    createDIBToggle(menuRow4, 16, 0);
    createDIBToggle(menuRow4, 17, 0);
    createDIBToggle(menuRow4, 18, 0);
    createDIBToggle(menuRow4, 19, 0);
    createDIBToggle(menuRow4, 20, 10);
    menuRow4->updateLayout();

    auto confirmButton = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("Confirm", "goldFont.fnt", "GJ_button_01.png", 0.8f), [this, callback](auto) {
        auto diff = 0;
        switch (m_demonsInBetweenOverride) {
            case 1: case 2: case 3: case 4: diff = 7; break;
            case 5: case 6: case 7: case 8: case 9: diff = 8; break;
            case 10: case 11: case 12: case 13: diff = 6; break;
            case 14: case 15: case 16: case 17: diff = 9; break;
            case 18: case 19: case 20: diff = 10; break;
        }
        callback(diff, m_demonsInBetweenOverride);
        onClose(nullptr);
    });
    confirmButton->setPosition(175.0f, 25.0f);
    m_buttonMenu->addChild(confirmButton);

    return true;
}

void FRDIBPopup::createDIBToggle(CCMenu* menu, int demonsInBetweenOverride, int difficulty) {
    auto toggle = CCMenuItemExt::createSpriteExtraWithFrameName(fmt::format("hiimjustin000.demons_in_between/DIB_{:02}_btn2_001.png", demonsInBetweenOverride).c_str(),
        1.0f, [this, demonsInBetweenOverride](CCMenuItemSpriteExtra* sender) {
            if (sender == m_selected) return;
            m_demonsInBetweenOverride = demonsInBetweenOverride;
            if (m_selected) FakeRate::toggle(m_selected->getNormalImage(), false);
            FakeRate::toggle(sender->getNormalImage(), true);
            m_selected = sender;
        });
    auto isToggled = m_difficulty > 5 && (m_demonsInBetweenOverride <= 0 ? difficulty == m_difficulty : demonsInBetweenOverride == m_demonsInBetweenOverride);
    FakeRate::toggle(toggle->getNormalImage(), isToggled);
    m_selected = isToggled ? toggle : m_selected;
    menu->addChild(toggle);
}
