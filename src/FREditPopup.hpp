#include "FRLevelInfoLayer.hpp"

class FREditPopup : public Popup<FRLevelInfoLayer*, GJGameLevel*, int, int, int, int>, SetIDPopupDelegate {
protected:
    FRLevelInfoLayer* m_delegate;
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

    bool setup(FRLevelInfoLayer*, GJGameLevel*, int, int, int, int) override;
    void updateLabels();
public:
    static FREditPopup* create(FRLevelInfoLayer*, GJGameLevel*, int, int, int, int);

    void setIDPopupClosed(SetIDPopup*, int) override;

    ~FREditPopup() override;
};

class FRSetDifficultyPopup : public Popup<int, int, bool, MiniFunction<void(int, int)>> {
protected:
    int m_difficulty;
    int m_moreDifficultiesOverride;
    bool m_legacy;
    CCMenuItemSpriteExtra* m_selected;

    bool setup(int, int, bool, MiniFunction<void(int, int)>) override;
    void createDifficultyToggle(CCMenu*, int, int);
public:
    static FRSetDifficultyPopup* create(int, int, bool, MiniFunction<void(int, int)>);
};

class FRSetFeaturePopup : public Popup<int, int, int, bool, MiniFunction<void(int)>> {
protected:
    GJFeatureState m_feature;
    int m_difficulty;
    int m_moreDifficultiesOverride;
    bool m_legacy;
    CCMenuItemSpriteExtra* m_selected;

    bool setup(int, int, int, bool, MiniFunction<void(int)>) override;
    void createFeatureToggle(CCMenu*, GJFeatureState);
public:
    static FRSetFeaturePopup* create(int, int, int, bool, MiniFunction<void(int)>);
};
