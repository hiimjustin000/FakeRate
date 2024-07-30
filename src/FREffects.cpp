#include "FREffects.hpp"

CCSprite* FREffects::grdInfinity() {
    CCSprite* infinity = CCSprite::createWithSpriteFrameName("itzkiba.grandpa_demon/GrD_demon4_infinity.png");
    infinity->setColor({ 255, 233, 136 });
    infinity->setOpacity(100);
    infinity->setScale(0.4f);
    infinity->runAction(CCRepeatForever::create(CCSequence::create(
        CCEaseSineInOut::create(CCFadeTo::create(1.5f, 200)),
        CCEaseSineInOut::create(CCFadeTo::create(1.5f, 60)),
        nullptr
    )));
    infinity->runAction(CCRepeatForever::create(CCSequence::create(
        CCEaseSineInOut::create(CCScaleTo::create(1.5f, 0.45f)),
        CCEaseSineInOut::create(CCScaleTo::create(1.5f, 0.35f)),
        nullptr
    )));
    infinity->setBlendFunc({ 1, 1 });
    return infinity;
}
