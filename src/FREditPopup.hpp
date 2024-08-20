#include "FREffects.hpp"

typedef MiniFunction<void(FakeRateSaveData, bool)> UpdateFakeRateCallback;
typedef MiniFunction<void(int, int, int, int, int)> SetDifficultyCallback;
typedef MiniFunction<void(int)> SetIntCallback;

class FREditPopup : public Popup<GJGameLevel*, FakeRateSaveData, UpdateFakeRateCallback> {
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
    CCSprite* m_mdSprite;
    CCSprite* m_grdSprite;
    CCSprite* m_grdInfinity;
    CCSprite* m_dibSprite;
    CCSprite* m_gddpSprite;
    CCSprite* m_starSprite;
    CCLabelBMFont* m_starsLabel;
    CCArray* m_coinSprites;

    bool setup(GJGameLevel*, FakeRateSaveData, UpdateFakeRateCallback) override;
    void updateLabels();
public:
    static FREditPopup* create(GJGameLevel*, FakeRateSaveData, UpdateFakeRateCallback);

    ~FREditPopup() override;
};

class FRSetDifficultyPopup : public Popup<FakeRateSaveData, bool, SetDifficultyCallback> {
protected:
    inline static std::vector<std::pair<int, int>> DIFFICULTIES = {
        { 0, 0 }, { -1, 0 }, { 1, 0 }, { 2, 0 }, { 3, 4 }, { 3, 0 }, { 4, 0 }, { 4, 7 },
        { 5, 0 }, { 5, 9 }, { 6, 0 }, { 7, 0 }, { 8, 0 }, { 9, 0 }, { 10, 0 }
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

class FRSetStarsPopup : public Popup<int, bool, SetIntCallback> {
protected:
    int m_stars;
    TextInput* m_input;
    CCLabelBMFont* m_label;
    CCNode* m_starLayout;

    bool setup(int, bool, SetIntCallback) override;
public:
    static FRSetStarsPopup* create(int, bool, SetIntCallback);
};

class FRSetFeaturePopup : public Popup<FakeRateSaveData, bool, SetIntCallback> {
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

class FRGRDPopup : public Popup<int, SetIntCallback> {
protected:
    int m_grandpaDemonOverride;
    CCMenuItemSpriteExtra* m_selected;

    bool setup(int, SetIntCallback) override;
public:
    static FRGRDPopup* create(int, SetIntCallback);
};

class FRDIBPopup : public Popup<int, SetIntCallback> {
protected:
    int m_demonsInBetweenOverride;
    CCMenuItemSpriteExtra* m_selected;

    bool setup(int, SetIntCallback) override;
public:
    static FRDIBPopup* create(int, SetIntCallback);
};

class FRGDDPPopup : public Popup<int, SetIntCallback> {
protected:
    int m_gddpIntegrationOverride;
    CCMenuItemSpriteExtra* m_selected;

    bool setup(int, SetIntCallback) override;
public:
    static FRGDDPPopup* create(int, SetIntCallback);
};
