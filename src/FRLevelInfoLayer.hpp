#include "FakeRate.hpp"

#include <Geode/modify/LevelInfoLayer.hpp>
class $modify(FRLevelInfoLayer, LevelInfoLayer) {
    struct Fields {
        FakeRateSaveData m_fakeRateData;
    };
    static void onModify(auto& self);
    bool init(GJGameLevel*, bool);
    void levelDownloadFinished(GJGameLevel*) override;
    void levelUpdateFinished(GJGameLevel*, UpdateResponse) override;
    void likedItem(LikeItemType, int, bool) override;
    void checkFakeRate();
    void updateFakeRate(int, int, int, int, bool, bool);
    void fixMoreDifficultiesIncompatibility(CCNode*, int, bool);
};
