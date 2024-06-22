#include "FREditPopup.hpp"

FREditPopup* FREditPopup::create(FRLevelInfoLayer* delegate, GJGameLevel* level, int stars, int feature, int difficulty, int mdo) {
    auto ret = new FREditPopup();
    if (ret && ret->initAnchored(300.0f, 200.0f, delegate, level, stars, feature, difficulty, mdo)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool FREditPopup::setup(FRLevelInfoLayer* delegate, GJGameLevel* level, int stars, int feature, int difficulty, int mdo) {
    setTitle("Fake Rate");
    m_delegate = delegate;
    m_level = level;
    m_stars = stars;
    m_feature = feature;
    m_difficulty = difficulty;
    m_moreDifficultiesOverride = mdo;
    m_legacy = false;

    m_difficultySprite = GJDifficultySprite::create(difficulty, GJDifficultyName::Long);
    m_difficultySprite->setPositionX(60.0f);
    m_mainLayer->addChild(m_difficultySprite);

    if (Loader::get()->isModLoaded("uproxide.more_difficulties")) {
        m_legacy = Loader::get()->getLoadedMod("uproxide.more_difficulties")->getSettingValue<bool>("legacy-difficulties");
        auto pos = m_legacy ? CCPoint { 60.0f, 100.0f } : CCPoint { 60.25f, 99.9f };

        m_casualSprite = CCSprite::createWithSpriteFrameName(m_legacy ? "uproxide.more_difficulties/MD_Difficulty04_Legacy.png" : "uproxide.more_difficulties/MD_Difficulty04.png");
        m_casualSprite->setPosition(pos);
        m_casualSprite->setVisible(false);
        m_mainLayer->addChild(m_casualSprite);

        m_toughSprite = CCSprite::createWithSpriteFrameName(m_legacy ? "uproxide.more_difficulties/MD_Difficulty07_Legacy.png" : "uproxide.more_difficulties/MD_Difficulty07.png");
        m_toughSprite->setPosition(pos);
        m_toughSprite->setVisible(false);
        m_mainLayer->addChild(m_toughSprite);

        m_cruelSprite = CCSprite::createWithSpriteFrameName(m_legacy ? "uproxide.more_difficulties/MD_Difficulty09_Legacy.png" : "uproxide.more_difficulties/MD_Difficulty09.png");
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

    m_coins = CCArray::create();
    m_coins->retain();

    for (int i = 0; i < m_level->m_coins; i++) {
        auto coin = CCSprite::createWithSpriteFrameName("usercoin_small01_001.png");
        coin->setPositionX(60.0f +
            (i == 2 ? 10.0f : i == 1 && m_level->m_coins == 2 ? 5.0f : i == 0 && m_level->m_coins == 2 ? -5.0f : i == 0 && m_level->m_coins == 3 ? -10.0f : 0.0f));
        m_mainLayer->addChild(coin);
        m_coins->addObject(coin);
    }

    auto difficultyButton = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("Difficulty", "goldFont.fnt", "GJ_button_02.png", 0.8f), [this](auto) {
        FRSetDifficultyPopup::create(m_difficulty, m_moreDifficultiesOverride, m_legacy, [this](int difficulty, int mdo) {
            m_difficulty = difficulty;
            m_moreDifficultiesOverride = mdo;
            updateLabels();
        })->show();
    });
    difficultyButton->setPosition(200.0f, 150.0f);
    m_buttonMenu->addChild(difficultyButton);

    auto starsButton = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("Stars", "goldFont.fnt", "GJ_button_02.png", 0.8f), [this, stars](auto) {
        auto popup = SetIDPopup::create(m_stars, 0, 127, "Set Stars", "Set", true, stars, 60.0f, false, false);
        popup->m_delegate = this;
        popup->show();
    });
    starsButton->setPosition(200.0f, 110.0f);
    m_buttonMenu->addChild(starsButton);

    auto featureButton = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("Feature", "goldFont.fnt", "GJ_button_02.png", 0.8f), [this](auto) {
        FRSetFeaturePopup::create(m_feature, m_difficulty, m_moreDifficultiesOverride, m_legacy, [this](int feature) {
            m_feature = feature;
            updateLabels();
        })->show();
    });
    featureButton->setPosition(200.0f, 70.0f);
    m_buttonMenu->addChild(featureButton);

    auto addButton = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("Add", "goldFont.fnt", "GJ_button_01.png", 0.8f), [this](auto) {
        auto vec = Mod::get()->getSavedValue<std::vector<FakeRateSaveData>>("fake-rate", {});
        auto it = std::find_if(vec.begin(), vec.end(), [this](auto const& item) {
            return item.id == m_level->m_levelID;
        });
        if (it != vec.end()) {
            it->stars = m_stars;
            it->feature = m_feature;
            it->difficulty = m_difficulty;
            it->moreDifficultiesOverride = m_moreDifficultiesOverride;
        }
        else {
            vec.push_back({
                .id = m_level->m_levelID,
                .stars = m_stars,
                .feature = m_feature,
                .difficulty = m_difficulty,
                .moreDifficultiesOverride = m_moreDifficultiesOverride
            });
        }
        Mod::get()->setSavedValue("fake-rate", vec);
        m_delegate->updateFakeRate(m_stars, m_feature, m_difficulty, m_moreDifficultiesOverride, true, true);
        onClose(nullptr);
    });
    addButton->setPosition(150.0f, 30.0f);
    m_buttonMenu->addChild(addButton);

    auto removeButton = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("Remove", "goldFont.fnt", "GJ_button_06.png", 0.8f), [this](auto) {
        auto vec = Mod::get()->getSavedValue<std::vector<FakeRateSaveData>>("fake-rate", {});
        if (vec.empty()) return;
        vec.erase(std::remove_if(vec.begin(), vec.end(), [this](auto const& item) {
            return item.id == m_level->m_levelID;
        }), vec.end());
        Mod::get()->setSavedValue("fake-rate", vec);
        auto stars = m_level->m_stars;
        m_delegate->updateFakeRate(m_level->m_stars, m_level->m_featured > 1 ? m_level->m_isEpic + 1 : 0, FakeRate::getDifficultyFromLevel(m_level),
            stars == 4 || stars == 7 || stars == 9 ? stars : 0, true, false);
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
    m_difficultySprite->updateDifficultyFrame(m_difficulty, GJDifficultyName::Long);
    m_difficultySprite->updateFeatureState((GJFeatureState)m_feature);
    m_difficultySprite->setPositionY(100.0f + (m_difficulty > 5 ? 5.0f : 0.0f) + (m_stars > 0 ? 10.0f : 0.0f));
    m_starSprite->setPosition({ m_difficultySprite->getPositionX() + 8.0f, m_difficultySprite->getPositionY() - 30.0f - (m_difficulty > 5 ? 9.0f : 0.0f) });
    m_starSprite->setVisible(m_stars > 0);
    m_starsLabel->setPosition(m_starSprite->getPositionX() - 8.0f, m_starSprite->getPositionY());
    m_starsLabel->setString(std::to_string(m_stars).c_str());
    m_starsLabel->setVisible(m_stars > 0);
    auto coins = reinterpret_cast<CCSprite**>(m_coins->data->arr);
    for (int i = 0; i < m_level->m_coins; i++) {
        coins[i]->setPositionY(m_difficultySprite->getPositionY() - 31.5f - (m_stars > 0 ? 14.0f : 0.0f) - (m_difficulty > 5 ? 9.0f : 0.0f));
    }
    if (Loader::get()->isModLoaded("uproxide.more_difficulties")) {
        auto pos = m_difficultySprite->getPosition() + (m_legacy ? CCPoint { 0.0f, 0.0f } : CCPoint { 0.25f, -0.1f });
        m_casualSprite->setPosition(pos);
        m_toughSprite->setPosition(pos);
        m_cruelSprite->setPosition(pos);
        m_casualSprite->setVisible(m_moreDifficultiesOverride == 4);
        m_toughSprite->setVisible(m_moreDifficultiesOverride == 7);
        m_cruelSprite->setVisible(m_moreDifficultiesOverride == 9);
        m_difficultySprite->setOpacity(m_moreDifficultiesOverride != 4 && m_moreDifficultiesOverride != 7 && m_moreDifficultiesOverride != 9 ? 255 : 0);
    }
}

FREditPopup::~FREditPopup() {
    m_coins->release();
}

FRSetDifficultyPopup* FRSetDifficultyPopup::create(int difficulty, int moreDifficultiesOverride, bool legacy, MiniFunction<void(int, int)> callback) {
    auto ret = new FRSetDifficultyPopup();
    if (ret && ret->initAnchored(300.0f, 250.0f, difficulty, moreDifficultiesOverride, legacy, callback)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool FRSetDifficultyPopup::setup(int difficulty, int moreDifficultiesOverride, bool legacy, MiniFunction<void(int, int)> callback) {
    setTitle("Set Difficulty");
    m_difficulty = difficulty;
    m_moreDifficultiesOverride = moreDifficultiesOverride;
    m_legacy = legacy;
    
    auto menuRow1 = CCMenu::create();
    menuRow1->setLayout(RowLayout::create()->setGap(20.0f));
    menuRow1->setPosition(150.0f, 185.0f);
    menuRow1->setContentSize({ 300.0f, 45.0f });
    m_mainLayer->addChild(menuRow1);

    auto menuRow2 = CCMenu::create();
    menuRow2->setLayout(RowLayout::create()->setGap(20.0f));
    menuRow2->setPosition(150.0f, 135.0f);
    menuRow2->setContentSize({ 300.0f, 45.0f });
    m_mainLayer->addChild(menuRow2);

    auto menuRow3 = CCMenu::create();
    menuRow3->setLayout(RowLayout::create()->setGap(20.0f));
    menuRow3->setPosition(150.0f, 80.0f);
    menuRow3->setContentSize({ 300.0f, 65.0f });
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
        callback(m_difficulty, m_moreDifficultiesOverride);
        onClose(nullptr);
    });
    confirmButton->setPosition(150.0f, 30.0f);
    m_buttonMenu->addChild(confirmButton);

    return true;
}

void FRSetDifficultyPopup::createDifficultyToggle(CCMenu* menu, int difficulty, int moreDifficultiesOverride) {
    auto num = difficulty == -1 ? "auto" : fmt::format("{:02d}", difficulty);
    auto frameName = difficulty > 5 ? fmt::format("difficulty_{}_btn2_001.png", num) : fmt::format("difficulty_{}_btn_001.png", num);
    if (Loader::get()->isModLoaded("uproxide.more_difficulties") && moreDifficultiesOverride > 0)
        frameName = m_legacy ? fmt::format("uproxide.more_difficulties/MD_Difficulty{:02d}_Legacy.png", moreDifficultiesOverride)
            : fmt::format("uproxide.more_difficulties/MD_Difficulty{:02d}.png", moreDifficultiesOverride);
    else if (moreDifficultiesOverride > 0) return;
    auto toggle = CCMenuItemExt::createSpriteExtraWithFrameName(frameName.c_str(), 1.0f, [this, difficulty, moreDifficultiesOverride](auto sender) {
        m_difficulty = difficulty;
        m_moreDifficultiesOverride = moreDifficultiesOverride;
        FakeRate::toggle(m_selected->getNormalImage(), false);
        FakeRate::toggle(sender->getNormalImage(), true);
        m_selected = sender;
    });
    FakeRate::toggle(toggle->getNormalImage(), difficulty == m_difficulty && moreDifficultiesOverride == m_moreDifficultiesOverride);
    m_selected = difficulty == m_difficulty && moreDifficultiesOverride == m_moreDifficultiesOverride ? toggle : m_selected;
    menu->addChild(toggle);
}

FRSetFeaturePopup* FRSetFeaturePopup::create(int feature, int difficulty, int moreDifficultiesOverride, bool legacy, MiniFunction<void(int)> callback) {
    auto ret = new FRSetFeaturePopup();
    if (ret && ret->initAnchored(300.0f, 150.0f, feature, difficulty, moreDifficultiesOverride, legacy, callback)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool FRSetFeaturePopup::setup(int feature, int difficulty, int moreDifficultiesOverride, bool legacy, MiniFunction<void(int)> callback) {
    setTitle("Set Feature");
    m_feature = static_cast<GJFeatureState>(feature);
    m_difficulty = difficulty;
    m_moreDifficultiesOverride = moreDifficultiesOverride;
    m_legacy = legacy;

    auto menuRow = CCMenu::create();
    menuRow->setLayout(RowLayout::create()->setGap(25.0f));
    menuRow->setPosition(150.0f, 75.0f);
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
    confirmButton->setPosition(150.0f, 30.0f);
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
    auto toggle = CCMenuItemExt::createSpriteExtra(difficultySprite, [this, feature](auto sender) {
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
