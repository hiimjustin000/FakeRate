#include <Geode/Geode.hpp>

using namespace geode::prelude;

class FRUtilities {
public:
    static int getBaseCurrency(int);
    static int getDifficultyForStars(int);
    static int getDifficultyFromLevel(GJGameLevel*);
    static std::string getSpriteName(CCNode*);
};