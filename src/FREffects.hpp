#include "FakeRate.hpp"

class FREffects {
public:
    static CCSprite* grdInfinity();
    static std::pair<CCSprite*, CCSprite*> infinityBackgrounds(CCLayer*, bool);
    static CCSprite* mythicalBackground(CCLayer*);
    static CCSprite* legendaryBackground(CCLayer*);
    static CCParticleSystemQuad* legendaryParticles();
    static CCParticleSystemQuad* mythicalParticles();
    static CCParticleSystemQuad* infiniteParticles1(bool);
    static CCParticleSystemQuad* infiniteParticles2();
};
