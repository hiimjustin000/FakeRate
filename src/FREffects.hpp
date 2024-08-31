#include "TableNode.hpp"

class FireSprite : public CCSprite {
protected:
    int m_currentFrame = 1;
    GJFeatureState m_featureState;
public:
    static FireSprite* create(GJFeatureState);
    bool init(GJFeatureState);
    void fireAnimation(float);
};

class FREffects {
public:
    static CCSprite* grdInfinity();
    static std::pair<CCSprite*, CCSprite*> infinityBackgrounds(bool);
    static CCSprite* mythicalBackground();
    static CCSprite* legendaryBackground();
};
