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

std::pair<CCSprite*, CCSprite*> FREffects::infinityBackgrounds(CCLayer* layer, bool grandpa) {
    auto bg = static_cast<CCSprite*>(layer->getChildByID("background"));
    bg->setZOrder(-10);
    bg->setOpacity(50);

    static_cast<CCSprite*>(layer->getChildByID("bottom-left-art"))->setOpacity(50);
    static_cast<CCSprite*>(layer->getChildByID("bottom-right-art"))->setOpacity(50);

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto bg1 = CCSprite::create("itzkiba.grandpa_demon/GrD_demon4_bg.png");
    bg1->setBlendFunc({ 1, 1 });
    bg1->setOpacity(0);
    bg1->setScale(winSize.width / bg1->getContentSize().width);
    bg1->setColor(grandpa ? ccColor3B { 121, 80, 255 } : ccColor3B { 249, 249, 165 });
    bg1->setAnchorPoint({ 0.5f, 0.0f });
    bg1->setPosition({ winSize.width / 2, 0.0f });
    bg1->setID("grandpa-background-1"_spr);
    bg1->runAction(CCRepeatForever::create(CCSequence::create(
        CCMoveTo::create(20.0f, { winSize.width / 2, -bg1->getScaledContentSize().height * 0.15f }),
        CCMoveTo::create(0.0f, { winSize.width / 2, 0.0f }),
        nullptr
    )));
    bg1->runAction(CCRepeatForever::create(CCSequence::create(
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 90.0f + 90.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 90.0f + 90.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 90.0f + 90.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 90.0f + 90.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 90.0f + 90.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 90.0f + 90.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 90.0f + 90.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 90.0f + 90.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 90.0f + 90.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 90.0f + 90.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 90.0f + 90.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 90.0f + 90.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 90.0f + 90.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 90.0f + 90.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 90.0f + 90.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 90.0f + 90.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 90.0f + 90.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 90.0f + 90.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 90.0f + 90.0f),
        CCFadeTo::create(1.0f, 0.0f),
        nullptr
    )));
    layer->addChild(bg1, -5);

    auto bg2 = CCSprite::create("itzkiba.grandpa_demon/GrD_demon4_bg.png");
    bg2->setBlendFunc({ 1, 1 });
    bg2->setOpacity(0);
    bg2->setScale(winSize.width * 2.0f / bg2->getContentSize().width);
    bg2->setColor(grandpa ? ccColor3B { 233, 200, 255 } : ccColor3B { 231, 231, 195 });
    bg2->setAnchorPoint({ 0.5f, 0.0f });
    bg2->setPosition({ winSize.width / 2, 0.0f });
    bg2->setID("grandpa-background-2"_spr);
    bg2->runAction(CCRepeatForever::create(CCSequence::create(
        CCMoveTo::create(30.0f, { winSize.width / 2, -bg2->getScaledContentSize().height * 0.75f }),
        CCMoveTo::create(0.0f, { winSize.width / 2, 0 }),
        nullptr
    )));
    bg2->runAction(CCRepeatForever::create(CCSequence::create(
        CCFadeTo::create(1.5f, CCRANDOM_MINUS1_1() * 90.0f + 50.0f),
        CCFadeTo::create(1.5f, CCRANDOM_MINUS1_1() * 90.0f + 50.0f),
        CCFadeTo::create(1.5f, CCRANDOM_MINUS1_1() * 90.0f + 50.0f),
        CCFadeTo::create(1.5f, CCRANDOM_MINUS1_1() * 90.0f + 50.0f),
        CCFadeTo::create(1.5f, CCRANDOM_MINUS1_1() * 90.0f + 50.0f),
        CCFadeTo::create(1.5f, CCRANDOM_MINUS1_1() * 90.0f + 50.0f),
        CCFadeTo::create(1.5f, CCRANDOM_MINUS1_1() * 90.0f + 50.0f),
        CCFadeTo::create(1.5f, CCRANDOM_MINUS1_1() * 90.0f + 50.0f),
        CCFadeTo::create(1.5f, CCRANDOM_MINUS1_1() * 90.0f + 50.0f),
        CCFadeTo::create(1.5f, CCRANDOM_MINUS1_1() * 90.0f + 50.0f),
        CCFadeTo::create(1.5f, CCRANDOM_MINUS1_1() * 90.0f + 50.0f),
        CCFadeTo::create(1.5f, CCRANDOM_MINUS1_1() * 90.0f + 50.0f),
        CCFadeTo::create(1.5f, CCRANDOM_MINUS1_1() * 90.0f + 50.0f),
        CCFadeTo::create(1.5f, CCRANDOM_MINUS1_1() * 90.0f + 50.0f),
        CCFadeTo::create(1.5f, CCRANDOM_MINUS1_1() * 90.0f + 50.0f),
        CCFadeTo::create(1.5f, CCRANDOM_MINUS1_1() * 90.0f + 50.0f),
        CCFadeTo::create(1.5f, CCRANDOM_MINUS1_1() * 90.0f + 50.0f),
        CCFadeTo::create(1.5f, CCRANDOM_MINUS1_1() * 90.0f + 50.0f),
        CCFadeTo::create(1.5f, CCRANDOM_MINUS1_1() * 90.0f + 50.0f),
        CCFadeTo::create(1.5f, 0.0f),
        nullptr
    )));
    layer->addChild(bg2, -4);

    return { bg1, bg2 };
}

CCSprite* FREffects::mythicalBackground(CCLayer* layer) {
    auto bg = static_cast<CCSprite*>(layer->getChildByID("background"));
    bg->setZOrder(-10);
    bg->setOpacity(50);

    static_cast<CCSprite*>(layer->getChildByID("bottom-left-art"))->setOpacity(50);
    static_cast<CCSprite*>(layer->getChildByID("bottom-right-art"))->setOpacity(50);

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto bg1 = CCSprite::create("itzkiba.grandpa_demon/GrD_demon4_bg.png");
    bg1->setBlendFunc({ 1, 1 });
    bg1->setOpacity(0);
    bg1->setScale(winSize.width / bg1->getContentSize().width);
    bg1->setColor({ 76, 63, 118 });
    bg1->setAnchorPoint({ 0.5f, 0.0f });
    bg1->setPosition({ winSize.width / 2, 0.0f });
    bg1->setID("grandpa-background-1"_spr);
    bg1->runAction(CCRepeatForever::create(CCSequence::create(
        CCMoveTo::create(20.0f, { winSize.width / 2, -bg1->getScaledContentSize().height * 0.35f }),
        CCMoveTo::create(0.0f, { winSize.width / 2, 0.0f }),
        nullptr
    )));
    bg1->runAction(CCRepeatForever::create(CCSequence::create(
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 50.0f + 130.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 50.0f + 130.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 50.0f + 130.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 50.0f + 130.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 50.0f + 130.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 50.0f + 130.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 50.0f + 130.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 50.0f + 130.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 50.0f + 130.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 50.0f + 130.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 50.0f + 130.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 50.0f + 130.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 50.0f + 130.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 50.0f + 130.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 50.0f + 130.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 50.0f + 130.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 50.0f + 130.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 50.0f + 130.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 50.0f + 130.0f),
        CCFadeTo::create(1.0f, 0.0f),
        nullptr
    )));
    layer->addChild(bg1, -5);

    return bg1;
}

CCSprite* FREffects::legendaryBackground(CCLayer* layer) {
    auto bg = static_cast<CCSprite*>(layer->getChildByID("background"));
    bg->setZOrder(-10);
    bg->setOpacity(50);

    static_cast<CCSprite*>(layer->getChildByID("bottom-left-art"))->setOpacity(50);
    static_cast<CCSprite*>(layer->getChildByID("bottom-right-art"))->setOpacity(50);

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto bg1 = CCSprite::create("itzkiba.grandpa_demon/GrD_demon4_bg.png");
    bg1->setBlendFunc({ 1, 1 });
    bg1->setOpacity(0);
    bg1->setScale(winSize.width / bg1->getContentSize().width);
    bg1->setColor({ 55, 48, 78 });
    bg1->setAnchorPoint({ 0.5f, 0.0f });
    bg1->setPosition({ winSize.width / 2, 0.0f });
    bg1->setID("grandpa-background-1"_spr);
    bg1->runAction(CCRepeatForever::create(CCSequence::create(
        CCMoveTo::create(20.0f, { winSize.width / 2, -bg1->getScaledContentSize().height * 0.25f }),
        CCMoveTo::create(0.0f, { winSize.width / 2, 0.0f }),
        nullptr
    )));
    bg1->runAction(CCRepeatForever::create(CCSequence::create(
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 40.0f + 100.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 40.0f + 100.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 40.0f + 100.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 40.0f + 100.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 40.0f + 100.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 40.0f + 100.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 40.0f + 100.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 40.0f + 100.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 40.0f + 100.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 40.0f + 100.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 40.0f + 100.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 40.0f + 100.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 40.0f + 100.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 40.0f + 100.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 40.0f + 100.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 40.0f + 100.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 40.0f + 100.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 40.0f + 100.0f),
        CCFadeTo::create(1.0f, CCRANDOM_MINUS1_1() * 40.0f + 100.0f),
        CCFadeTo::create(1.0f, 0.0f),
        nullptr
    )));
    layer->addChild(bg1, -5);

    return bg1;
}

CCParticleSystemQuad* FREffects::legendaryParticles() {
    auto dict = CCDictionary::createWithContentsOfFileThreadSafe("dragEffect.plist");
    dict->setObject(CCString::create("1"), "emitterType");
    dict->setObject(CCString::create("-1"), "duration");
    dict->setObject(CCString::create("50"), "maxParticles");
    dict->setObject(CCString::create("1.5"), "particleLifespan");
    dict->setObject(CCString::create("0.5"), "particleLifespanVariance");
    dict->setObject(CCString::create("90"), "angle");
    dict->setObject(CCString::create("180"), "angleVariance");
    dict->setObject(CCString::create("0"), "sourcePositionx");
    dict->setObject(CCString::create("0"), "sourcePositiony");
    dict->setObject(CCString::create("0"), "sourcePositionVariancex");
    dict->setObject(CCString::create("0"), "sourcePositionVariancey");
    dict->setObject(CCString::create("0"), "maxRadius");
    dict->setObject(CCString::create("30"), "minRadius");
    dict->setObject(CCString::create("3"), "startParticleSize");
    dict->setObject(CCString::create("1"), "finishParticleSize");
    dict->setObject(CCString::create("1"), "startParticleSizeVariance");
    dict->setObject(CCString::create("10"), "speed");
    dict->setObject(CCString::create("5"), "speedVariance");
    dict->setObject(CCString::create("0"), "gravityx");
    dict->setObject(CCString::create("0"), "gravityy");
    dict->setObject(CCString::create("0.65"), "startColorRed");
    dict->setObject(CCString::create("0"), "startColorGreen");
    dict->setObject(CCString::create("1"), "startColorBlue");
    dict->setObject(CCString::create("0.5"), "startColorAlpha");
    dict->setObject(CCString::create("0.1"), "startColorVarianceRed");
    dict->setObject(CCString::create("0.1"), "startColorVarianceBlue");
    dict->setObject(CCString::create("0.1"), "startColorVarianceGreen");
    dict->setObject(CCString::create("0"), "startColorVarianceAlpha");
    dict->setObject(CCString::create("1"), "finishColorRed");
    dict->setObject(CCString::create("1"), "finishColorGreen");
    dict->setObject(CCString::create("1"), "finishColorBlue");
    dict->setObject(CCString::create("0"), "finishColorAlpha");
    dict->setObject(CCString::create("0.3"), "finishColorVarianceRed");
    dict->setObject(CCString::create("0.3"), "finishColorVarianceBlue");
    dict->setObject(CCString::create("0.3"), "finishColorVarianceGreen");
    dict->setObject(CCString::create("0"), "finishColorVarianceAlpha");
    dict->setObject(CCString::create("40"), "rotatePerSecond");
    dict->setObject(CCString::create("10"), "rotatePerSecondVariance");
    dict->setObject(CCString::create("270"), "rotationEndVariance");
    dict->setObject(CCString::create("square.png"), "textureFileName");

    auto emitter = CCParticleSystemQuad::create();
    emitter->initWithDictionary(dict, false);
    emitter->setID("grandpa-particles-1"_spr);
    return emitter;
}



CCParticleSystemQuad* FREffects::mythicalParticles() {
    auto dict = CCDictionary::createWithContentsOfFileThreadSafe("dragEffect.plist");
    dict->setObject(CCString::create("1"), "emitterType");
    dict->setObject(CCString::create("-1"), "duration");
    dict->setObject(CCString::create("50"), "maxParticles");
    dict->setObject(CCString::create("1.5"), "particleLifespan");
    dict->setObject(CCString::create("0.5"), "particleLifespanVariance");
    dict->setObject(CCString::create("90"), "angle");
    dict->setObject(CCString::create("180"), "angleVariance");
    dict->setObject(CCString::create("0"), "sourcePositionx");
    dict->setObject(CCString::create("0"), "sourcePositiony");
    dict->setObject(CCString::create("0"), "sourcePositionVariancex");
    dict->setObject(CCString::create("0"), "sourcePositionVariancey");
    dict->setObject(CCString::create("0"), "maxRadius");
    dict->setObject(CCString::create("40"), "minRadius");
    dict->setObject(CCString::create("4"), "startParticleSize");
    dict->setObject(CCString::create("1"), "finishParticleSize");
    dict->setObject(CCString::create("2"), "startParticleSizeVariance");
    dict->setObject(CCString::create("15"), "speed");
    dict->setObject(CCString::create("7"), "speedVariance");
    dict->setObject(CCString::create("0"), "gravityx");
    dict->setObject(CCString::create("0"), "gravityy");
    dict->setObject(CCString::create("0.4"), "startColorRed");
    dict->setObject(CCString::create("0.4"), "startColorGreen");
    dict->setObject(CCString::create("1"), "startColorBlue");
    dict->setObject(CCString::create("0.7"), "startColorAlpha");
    dict->setObject(CCString::create("0.1"), "startColorVarianceRed");
    dict->setObject(CCString::create("0.1"), "startColorVarianceBlue");
    dict->setObject(CCString::create("0.1"), "startColorVarianceGreen");
    dict->setObject(CCString::create("0"), "startColorVarianceAlpha");
    dict->setObject(CCString::create("1"), "finishColorRed");
    dict->setObject(CCString::create("1"), "finishColorGreen");
    dict->setObject(CCString::create("1"), "finishColorBlue");
    dict->setObject(CCString::create("0"), "finishColorAlpha");
    dict->setObject(CCString::create("0.1"), "finishColorVarianceRed");
    dict->setObject(CCString::create("0.1"), "finishColorVarianceBlue");
    dict->setObject(CCString::create("0.1"), "finishColorVarianceGreen");
    dict->setObject(CCString::create("0"), "finishColorVarianceAlpha");
    dict->setObject(CCString::create("60"), "rotatePerSecond");
    dict->setObject(CCString::create("20"), "rotatePerSecondVariance");
    dict->setObject(CCString::create("360"), "rotationEndVariance");
    dict->setObject(CCString::create("square.png"), "textureFileName");

    auto emitter = CCParticleSystemQuad::create();
    emitter->initWithDictionary(dict, false);
    emitter->setID("grandpa-particles-1"_spr);
    return emitter;
}



CCParticleSystemQuad* FREffects::infiniteParticles1(bool grandpa) {
    auto dict = CCDictionary::createWithContentsOfFileThreadSafe("dragEffect.plist");
    dict->setObject(CCString::create("1"), "emitterType");
    dict->setObject(CCString::create("-1"), "duration");
    dict->setObject(CCString::create("50"), "maxParticles");
    dict->setObject(CCString::create("1.5"), "particleLifespan");
    dict->setObject(CCString::create("0.5"), "particleLifespanVariance");
    dict->setObject(CCString::create("90"), "angle");
    dict->setObject(CCString::create("180"), "angleVariance");
    dict->setObject(CCString::create("0"), "sourcePositionx");
    dict->setObject(CCString::create("0"), "sourcePositiony");
    dict->setObject(CCString::create("0"), "sourcePositionVariancex");
    dict->setObject(CCString::create("0"), "sourcePositionVariancey");
    dict->setObject(CCString::create("0"), "maxRadius");
    dict->setObject(CCString::create("50"), "minRadius");
    dict->setObject(CCString::create("3"), "startParticleSize");
    dict->setObject(CCString::create("1"), "finishParticleSize");
    dict->setObject(CCString::create("1"), "startParticleSizeVariance");
    dict->setObject(CCString::create("20"), "speed");
    dict->setObject(CCString::create("7"), "speedVariance");
    dict->setObject(CCString::create("0"), "gravityx");
    dict->setObject(CCString::create("0"), "gravityy");
    dict->setObject(CCString::create(grandpa ? "0.95" : "1"), "startColorRed");
    dict->setObject(CCString::create(grandpa ? "0.3" : "0.85"), "startColorGreen");
    dict->setObject(CCString::create(grandpa ? "0.95" : "0"), "startColorBlue");
    dict->setObject(CCString::create("0.7"), "startColorAlpha");
    dict->setObject(CCString::create("0.1"), "startColorVarianceRed");
    dict->setObject(CCString::create("0.1"), "startColorVarianceBlue");
    dict->setObject(CCString::create("0.1"), "startColorVarianceGreen");
    dict->setObject(CCString::create("0"), "startColorVarianceAlpha");
    dict->setObject(CCString::create("1"), "finishColorRed");
    dict->setObject(CCString::create("1"), "finishColorGreen");
    dict->setObject(CCString::create("1"), "finishColorBlue");
    dict->setObject(CCString::create("0"), "finishColorAlpha");
    dict->setObject(CCString::create("0.1"), "finishColorVarianceRed");
    dict->setObject(CCString::create("0.1"), "finishColorVarianceBlue");
    dict->setObject(CCString::create("0.1"), "finishColorVarianceGreen");
    dict->setObject(CCString::create("0"), "finishColorVarianceAlpha");
    dict->setObject(CCString::create("90"), "rotatePerSecond");
    dict->setObject(CCString::create("30"), "rotatePerSecondVariance");
    dict->setObject(CCString::create("360"), "rotationEndVariance");
    dict->setObject(CCString::create("square.png"), "textureFileName");

    auto emitter = CCParticleSystemQuad::create();
    emitter->initWithDictionary(dict, false);
    emitter->setID("grandpa-particles-1"_spr);
    return emitter;
}

CCParticleSystemQuad* FREffects::infiniteParticles2() {
    auto dict = CCDictionary::createWithContentsOfFileThreadSafe("dragEffect.plist");
    dict->setObject(CCString::create("1"), "emitterType");
    dict->setObject(CCString::create("-1"), "duration");
    dict->setObject(CCString::create("50"), "maxParticles");
    dict->setObject(CCString::create("1.5"), "particleLifespan");
    dict->setObject(CCString::create("0.5"), "particleLifespanVariance");
    dict->setObject(CCString::create("90"), "angle");
    dict->setObject(CCString::create("180"), "angleVariance");
    dict->setObject(CCString::create("0"), "sourcePositionx");
    dict->setObject(CCString::create("0"), "sourcePositiony");
    dict->setObject(CCString::create("0"), "sourcePositionVariancex");
    dict->setObject(CCString::create("0"), "sourcePositionVariancey");
    dict->setObject(CCString::create("10"), "maxRadius");
    dict->setObject(CCString::create("30"), "minRadius");
    dict->setObject(CCString::create("12"), "startParticleSize");
    dict->setObject(CCString::create("3"), "finishParticleSize");
    dict->setObject(CCString::create("5"), "startParticleSizeVariance");
    dict->setObject(CCString::create("20"), "speed");
    dict->setObject(CCString::create("7"), "speedVariance");
    dict->setObject(CCString::create("0"), "gravityx");
    dict->setObject(CCString::create("0"), "gravityy");
    dict->setObject(CCString::create("0.95"), "startColorRed");
    dict->setObject(CCString::create("0.95"), "startColorGreen");
    dict->setObject(CCString::create("0.95"), "startColorBlue");
    dict->setObject(CCString::create("1"), "startColorAlpha");
    dict->setObject(CCString::create("0.1"), "startColorVarianceRed");
    dict->setObject(CCString::create("0.1"), "startColorVarianceBlue");
    dict->setObject(CCString::create("0.1"), "startColorVarianceGreen");
    dict->setObject(CCString::create("0"), "startColorVarianceAlpha");
    dict->setObject(CCString::create("1"), "finishColorRed");
    dict->setObject(CCString::create("1"), "finishColorGreen");
    dict->setObject(CCString::create("1"), "finishColorBlue");
    dict->setObject(CCString::create("0"), "finishColorAlpha");
    dict->setObject(CCString::create("0.1"), "finishColorVarianceRed");
    dict->setObject(CCString::create("0.1"), "finishColorVarianceBlue");
    dict->setObject(CCString::create("0.1"), "finishColorVarianceGreen");
    dict->setObject(CCString::create("0"), "finishColorVarianceAlpha");
    dict->setObject(CCString::create("0"), "rotatePerSecond");
    dict->setObject(CCString::create("60"), "rotatePerSecondVariance");
    dict->setObject(CCString::create("360"), "rotationEndVariance");
    dict->setObject(CCString::create("sun.png"), "textureFileName");

    auto emitter = CCParticleSystemQuad::create();
    emitter->initWithDictionary(dict, false);
    emitter->setID("grandpa-particles-2"_spr);
    return emitter;
}
