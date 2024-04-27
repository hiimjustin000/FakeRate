#include <Geode/Geode.hpp>

using namespace geode::prelude;

class FRUtilities {
public:
    static int getBaseCurrency(int);
    static CCNode* getChildBySpriteName(CCNode*, const char*);
    static int getDifficultyForStars(int);
    static int getDifficultyFromLevel(GJGameLevel*);
    static bool isSpriteName(CCNode*, const char*);
};