#include "FREffects.hpp"

CCSprite* FREffects::grdInfinity() {
    CCSprite* infinity = CCSprite::createWithSpriteFrameName("itzkiba.grandpa_demon/GrD_demon4_infinity.png");
    infinity->setZOrder(30);
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

std::pair<CCSprite*, CCSprite*> FREffects::infinityBackgrounds(bool grandpa) {
    auto width = CCDirector::sharedDirector()->getWinSize().width;

    auto bg1 = CCSprite::create("itzkiba.grandpa_demon/GrD_demon4_bg.png");
    bg1->setBlendFunc({ 1, 1 });
    bg1->setOpacity(0);
    bg1->setScale(width / bg1->getContentWidth());
    bg1->setColor(grandpa ? ccColor3B { 121, 80, 255 } : ccColor3B { 249, 249, 165 });
    bg1->setAnchorPoint({ 0.5f, 0.0f });
    bg1->setPosition({ width / 2, 0.0f });
    bg1->setZOrder(-5);
    bg1->runAction(CCRepeatForever::create(CCSequence::create(
        CCMoveTo::create(20.0f, { width / 2, -bg1->getScaledContentHeight() * 0.15f }),
        CCMoveTo::create(0.0f, { width / 2, 0.0f }),
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

    auto bg2 = CCSprite::create("itzkiba.grandpa_demon/GrD_demon4_bg.png");
    bg2->setBlendFunc({ 1, 1 });
    bg2->setOpacity(0);
    bg2->setScale(width * 2.0f / bg2->getContentWidth());
    bg2->setColor(grandpa ? ccColor3B { 233, 200, 255 } : ccColor3B { 231, 231, 195 });
    bg2->setAnchorPoint({ 0.5f, 0.0f });
    bg2->setPosition({ width / 2, 0.0f });
    bg2->setZOrder(-4);
    bg2->runAction(CCRepeatForever::create(CCSequence::create(
        CCMoveTo::create(30.0f, { width / 2, -bg2->getScaledContentHeight() * 0.75f }),
        CCMoveTo::create(0.0f, { width / 2, 0 }),
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

    return { bg1, bg2 };
}

CCSprite* FREffects::mythicalBackground() {
    auto width = CCDirector::sharedDirector()->getWinSize().width;

    auto bg1 = CCSprite::create("itzkiba.grandpa_demon/GrD_demon4_bg.png");
    bg1->setBlendFunc({ 1, 1 });
    bg1->setOpacity(0);
    bg1->setScale(width / bg1->getContentWidth());
    bg1->setColor({ 76, 63, 118 });
    bg1->setAnchorPoint({ 0.5f, 0.0f });
    bg1->setPosition({ width / 2, 0.0f });
    bg1->setZOrder(-5);
    bg1->runAction(CCRepeatForever::create(CCSequence::create(
        CCMoveTo::create(20.0f, { width / 2, -bg1->getScaledContentHeight() * 0.35f }),
        CCMoveTo::create(0.0f, { width / 2, 0.0f }),
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

    return bg1;
}

CCSprite* FREffects::legendaryBackground() {
    auto width = CCDirector::sharedDirector()->getWinSize().width;

    auto bg1 = CCSprite::create("itzkiba.grandpa_demon/GrD_demon4_bg.png");
    bg1->setBlendFunc({ 1, 1 });
    bg1->setOpacity(0);
    bg1->setScale(width / bg1->getContentWidth());
    bg1->setColor({ 55, 48, 78 });
    bg1->setAnchorPoint({ 0.5f, 0.0f });
    bg1->setPosition({ width / 2, 0.0f });
    bg1->setZOrder(-5);
    bg1->runAction(CCRepeatForever::create(CCSequence::create(
        CCMoveTo::create(20.0f, { width / 2, -bg1->getScaledContentHeight() * 0.25f }),
        CCMoveTo::create(0.0f, { width / 2, 0.0f }),
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

    return bg1;
}
