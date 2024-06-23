#include "FakeRate.hpp"

typedef MiniFunction<void(int, int, int, int, bool, bool)> UpdateFakeRateCallback;
typedef MiniFunction<void(int, int)> SetDifficultyCallback;
typedef MiniFunction<void(int)> SetFeatureCallback;

class FREditPopup : public Popup<GJGameLevel*, int, int, int, int, UpdateFakeRateCallback>, SetIDPopupDelegate {
protected:
    GJGameLevel* m_level;
    int m_stars;
    int m_feature;
    int m_difficulty;
    int m_moreDifficultiesOverride;
    bool m_legacy;
    GJDifficultySprite* m_difficultySprite;
    CCSprite* m_casualSprite;
    CCSprite* m_toughSprite;
    CCSprite* m_cruelSprite;
    CCSprite* m_starSprite;
    CCLabelBMFont* m_starsLabel;
    CCArray* m_coins;

    bool setup(GJGameLevel*, int, int, int, int, UpdateFakeRateCallback) override;
    void updateLabels();
public:
    static FREditPopup* create(GJGameLevel*, int, int, int, int, UpdateFakeRateCallback);

    void setIDPopupClosed(SetIDPopup*, int) override;

    ~FREditPopup() override;
};

class FRSetDifficultyPopup : public Popup<int, int, bool, SetDifficultyCallback> {
protected:
    int m_difficulty;
    int m_moreDifficultiesOverride;
    bool m_legacy;
    CCMenuItemSpriteExtra* m_selected;

    bool setup(int, int, bool, SetDifficultyCallback) override;
    void createDifficultyToggle(CCMenu*, int, int);
public:
    static FRSetDifficultyPopup* create(int, int, bool, SetDifficultyCallback);
};

class FRSetFeaturePopup : public Popup<int, int, int, bool, SetFeatureCallback> {
protected:
    GJFeatureState m_feature;
    int m_difficulty;
    int m_moreDifficultiesOverride;
    bool m_legacy;
    CCMenuItemSpriteExtra* m_selected;

    bool setup(int, int, int, bool, SetFeatureCallback) override;
    void createFeatureToggle(CCMenu*, GJFeatureState);
public:
    static FRSetFeaturePopup* create(int, int, int, bool, SetFeatureCallback);
};
