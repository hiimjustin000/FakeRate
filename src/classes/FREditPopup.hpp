#include "../FakeRate.hpp"

typedef std::function<void(FakeRateSaveData, bool)> const& UpdateFakeRateCallback;
typedef std::function<void(int, int, int, int, int)> const& SetDifficultyCallback;
typedef std::function<void(int)> const& SetIntCallback;

class FREditPopup : public geode::Popup<GJGameLevel*, FakeRateSaveData, UpdateFakeRateCallback> {
protected:
    GJGameLevel* m_level;
    int m_stars;
    int m_feature;
    int m_difficulty;
    int m_moreDifficultiesOverride;
    int m_grandpaDemonOverride;
    int m_demonsInBetweenOverride;
    int m_gddpIntegrationOverride;
    bool m_coins;
    bool m_legacy;
    GJDifficultySprite* m_difficultySprite;
    cocos2d::CCSprite* m_mdSprite;
    cocos2d::CCSprite* m_grdSprite;
    cocos2d::CCSprite* m_grdInfinity;
    cocos2d::CCSprite* m_dibSprite;
    cocos2d::CCSprite* m_gddpSprite;
    cocos2d::CCSprite* m_starSprite;
    cocos2d::CCLabelBMFont* m_starsLabel;
    cocos2d::CCArray* m_coinSprites;

    bool setup(GJGameLevel*, FakeRateSaveData, UpdateFakeRateCallback) override;
    void updateLabels();
public:
    static FREditPopup* create(GJGameLevel*, FakeRateSaveData, UpdateFakeRateCallback);

    ~FREditPopup() override;
};

class FRSetDifficultyPopup : public geode::Popup<FakeRateSaveData, bool, SetDifficultyCallback> {
protected:
    inline static std::vector<std::pair<int, int>> DIFFICULTIES = {
        { 0, 0 }, { -1, 0 }, { 1, 0 }, { 2, 0 }, { 3, 4 }, { 3, 0 }, { 4, 0 }, { 4, 7 },
        { 5, 0 }, { 5, 9 }, { 7, 0 }, { 8, 0 }, { 6, 0 }, { 9, 0 }, { 10, 0 }
    };

    int m_difficulty;
    int m_moreDifficultiesOverride;
    int m_grandpaDemonOverride;
    int m_demonsInBetweenOverride;
    int m_gddpIntegrationOverride;
    bool m_legacy;
    CCMenuItemSpriteExtra* m_selected;

    bool setup(FakeRateSaveData, bool, SetDifficultyCallback) override;
public:
    static FRSetDifficultyPopup* create(FakeRateSaveData, bool, SetDifficultyCallback);
};

class FRSetStarsPopup : public geode::Popup<int, bool, SetIntCallback> {
protected:
    int m_stars;
    geode::TextInput* m_input;
    cocos2d::CCLabelBMFont* m_label;
    CCNode* m_starLayout;

    bool setup(int, bool, SetIntCallback) override;
public:
    static FRSetStarsPopup* create(int, bool, SetIntCallback);
};

class FRSetFeaturePopup : public geode::Popup<FakeRateSaveData, bool, SetIntCallback> {
protected:
    GJFeatureState m_feature;
    int m_difficulty;
    int m_moreDifficultiesOverride;
    int m_grandpaDemonOverride;
    int m_demonsInBetweenOverride;
    int m_gddpIntegrationOverride;
    bool m_legacy;
    CCMenuItemSpriteExtra* m_selected;

    bool setup(FakeRateSaveData, bool, SetIntCallback) override;
public:
    static FRSetFeaturePopup* create(FakeRateSaveData, bool, SetIntCallback);
};

class FRGRDPopup : public geode::Popup<int, SetIntCallback> {
protected:
    int m_grandpaDemonOverride;
    CCMenuItemSpriteExtra* m_selected;

    bool setup(int, SetIntCallback) override;
public:
    static FRGRDPopup* create(int, SetIntCallback);
};

class FRDIBPopup : public geode::Popup<int, SetIntCallback> {
protected:
    int m_demonsInBetweenOverride;
    CCMenuItemSpriteExtra* m_selected;

    bool setup(int, SetIntCallback) override;
public:
    static FRDIBPopup* create(int, SetIntCallback);
};

class FRGDDPPopup : public geode::Popup<int, SetIntCallback> {
protected:
    int m_gddpIntegrationOverride;
    CCMenuItemSpriteExtra* m_selected;

    bool setup(int, SetIntCallback) override;
public:
    static FRGDDPPopup* create(int, SetIntCallback);
};
