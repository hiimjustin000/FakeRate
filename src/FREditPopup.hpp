#include <vector>
#include <Geode/modify/LevelInfoLayer.hpp>
#include "FRUtilities.hpp"

struct FakeRateSaveData {
    int id;
    int stars;
    int feature;
    int difficulty;
};

class FRLevelInfoLayerDummy; struct FRLevelInfoLayer : Modify<FRLevelInfoLayer, LevelInfoLayer> {
    struct Fields {
        FakeRateSaveData m_fakeRateData;
    };
    static void onModify(auto& self);
    bool init(GJGameLevel*, bool);
    void levelDownloadFinished(GJGameLevel*);
    void levelUpdateFinished(GJGameLevel*, UpdateResponse);
    void likedItem(LikeItemType, int, bool);
    void checkFakeRate();
    void updateFakeRate(int, int, int, bool, bool);
    void onFakeRate(CCObject*);
    void fixMoreDifficultiesIncompatibility();
};

class FREditPopup : public Popup<GJGameLevel*, int, int, int> {
protected:
    GJGameLevel* m_level;
    int m_stars;
    int m_feature;
    int m_difficulty;
    GJDifficultySprite* m_difficultySprite;
    CCSprite* m_casualSprite;
    CCSprite* m_toughSprite;
    CCSprite* m_cruelSprite;
    CCSprite* m_starSprite;
    CCLabelBMFont* m_starsLabel;
    CCMenuItemSpriteExtra* m_starLeftArrow;
    CCMenuItemSpriteExtra* m_starRightArrow;
    CCMenuItemSpriteExtra* m_difficultyLeftArrow;
    CCMenuItemSpriteExtra* m_difficultyRightArrow;
    CCMenuItemSpriteExtra* m_featureLeftArrow;
    CCMenuItemSpriteExtra* m_featureRightArrow;

    bool setup(GJGameLevel*, int, int, int) override;
    void updateLabels();
public:
    FRLevelInfoLayer* m_delegate;
    static FREditPopup* create(GJGameLevel*, int, int, int);

    void onStarLeft(CCObject*);
    void onStarRight(CCObject*);
    void onDifficultyLeft(CCObject*);
    void onDifficultyRight(CCObject*);
    void onFeatureLeft(CCObject*);
    void onFeatureRight(CCObject*);
    void onAdd(CCObject*);
    void onRemove(CCObject*);
};

template<>
struct matjson::Serialize<std::vector<FakeRateSaveData>> {
    static std::vector<FakeRateSaveData> from_json(matjson::Value const& value) {
        auto vec = std::vector<FakeRateSaveData>{};
        for (auto const& item : value.as_array()) {
            vec.push_back({
                .id = item["id"].as_int(),
                .stars = item["stars"].as_int(),
                .feature = item["feature"].as_int(),
                .difficulty = item["difficulty"].as_int()
            });
        }
        return vec;
    }

    static matjson::Value to_json(std::vector<FakeRateSaveData> const& vec) {
        auto arr = matjson::Array{};
        for (auto const& item : vec) {
            arr.push_back(matjson::Object {
                { "id", item.id },
                { "stars", item.stars },
                { "feature", item.feature },
                { "difficulty", item.difficulty }
            });
        }
        return arr;
    }

    static bool is_json(matjson::Value const& value) {
        return value.is_array();
    }
};
