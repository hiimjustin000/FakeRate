#include "FakeRate.hpp"

typedef MiniFunction<void(FakeRateSaveData, bool)> UpdateFakeRateCallback;
typedef MiniFunction<void(int, int, int, int)> SetDifficultyCallback;
typedef MiniFunction<void(int)> SetFeatureCallback;
typedef MiniFunction<void(int)> SetGRDCallback;
typedef MiniFunction<void(int, int)> SetDIBCallback;

class FREditPopup : public Popup<GJGameLevel*, FakeRateSaveData, UpdateFakeRateCallback>, SetIDPopupDelegate {
protected:
    GJGameLevel* m_level;
    int m_stars;
    int m_feature;
    int m_difficulty;
    int m_moreDifficultiesOverride;
    int m_grandpaDemonOverride;
    int m_demonsInBetweenOverride;
    bool m_coins;
    bool m_legacy;
    GJDifficultySprite* m_difficultySprite;
    CCSprite* m_mdSprite;
    CCSprite* m_grdSprite;
    CCSprite* m_dibSprite;
    CCSprite* m_starSprite;
    CCLabelBMFont* m_starsLabel;
    CCArray* m_coinSprites;

    bool setup(GJGameLevel*, FakeRateSaveData, UpdateFakeRateCallback) override;
    void updateLabels();
public:
    static FREditPopup* create(GJGameLevel*, FakeRateSaveData, UpdateFakeRateCallback);

    void setIDPopupClosed(SetIDPopup*, int) override;

    ~FREditPopup() override;
};

class FRSetDifficultyPopup : public Popup<int, int, int, int, bool, SetDifficultyCallback> {
protected:
    int m_difficulty;
    int m_moreDifficultiesOverride;
    int m_grandpaDemonOverride;
    int m_demonsInBetweenOverride;
    bool m_legacy;
    CCMenuItemSpriteExtra* m_selected;

    bool setup(int, int, int, int, bool, SetDifficultyCallback) override;
    void createDifficultyToggle(CCMenu*, int, int);
public:
    static FRSetDifficultyPopup* create(int, int, int, int, bool, SetDifficultyCallback);
};

class FRSetFeaturePopup : public Popup<int, int, int, int, int, bool, SetFeatureCallback> {
protected:
    GJFeatureState m_feature;
    int m_difficulty;
    int m_moreDifficultiesOverride;
    int m_grandpaDemonOverride;
    int m_demonsInBetweenOverride;
    bool m_legacy;
    CCMenuItemSpriteExtra* m_selected;

    bool setup(int, int, int, int, int, bool, SetFeatureCallback) override;
    void createFeatureToggle(CCMenu*, GJFeatureState);
public:
    static FRSetFeaturePopup* create(int, int, int, int, int, bool, SetFeatureCallback);
};

class FRGRDPopup : public Popup<int, SetGRDCallback> {
protected:
    int m_grandpaDemonOverride;
    CCMenuItemSpriteExtra* m_selected;

    bool setup(int, SetGRDCallback) override;
    void createGRDToggle(CCMenu*, int);
public:
    static FRGRDPopup* create(int, SetGRDCallback);
};

class FRDIBPopup : public Popup<int, int, SetDIBCallback> {
protected:
    int m_difficulty;
    int m_demonsInBetweenOverride;
    CCMenuItemSpriteExtra* m_selected;

    bool setup(int, int, SetDIBCallback) override;
    void createDIBToggle(CCMenu*, int, int);
public:
    static FRDIBPopup* create(int, int, SetDIBCallback);
};
