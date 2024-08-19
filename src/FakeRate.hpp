#include <Geode/Geode.hpp>

using namespace geode::prelude;

struct FakeRateSaveData {
    int id;
    int stars;
    int feature;
    int difficulty;
    int moreDifficultiesOverride;
    int grandpaDemonOverride;
    int demonsInBetweenOverride;
    int gddpIntegrationOverride;
    bool coins;
};

class FakeRate {
public:
    static int getBaseCurrency(int);
    static int getDifficultyFromLevel(GJGameLevel*);
    static std::string getSpriteName(CCSprite*);
    static void toggle(CCNode*, bool);
    static CCPoint getDIBOffset(int, GJDifficultyName);
    static int getGRDOverride(CCSprite*);
    static int getDIBOverride(CCSprite*);
    static int getGDDPOverride(CCSprite*);
    static std::string getGDDPFrame(int, GJDifficultyName);
};

template<>
struct matjson::Serialize<std::vector<FakeRateSaveData>> {
    static std::vector<FakeRateSaveData> from_json(matjson::Value const& value) {
        std::vector<FakeRateSaveData> vec;
        for (auto const& item : value.as_array()) {
            vec.push_back({
                .id = item["id"].as_int(),
                .stars = item["stars"].as_int(),
                .feature = item["feature"].as_int(),
                .difficulty = item["difficulty"].as_int(),
                .moreDifficultiesOverride = item.contains("more-difficulties-override") ? item["more-difficulties-override"].as_int() : 0,
                .grandpaDemonOverride = item.contains("grandpa-demon-override") ? item["grandpa-demon-override"].as_int() : 0,
                .demonsInBetweenOverride = item.contains("demons-in-between-override") ? item["demons-in-between-override"].as_int() : 0,
                .gddpIntegrationOverride = item.contains("gddp-integration-override") ? item["gddp-integration-override"].as_int() : 0,
                .coins = item.contains("coins") ? item["coins"].as_bool() : true
            });
        }
        return vec;
    }

    static matjson::Value to_json(std::vector<FakeRateSaveData> const& vec) {
        matjson::Array arr;
        for (auto const& item : vec) {
            arr.push_back(matjson::Object {
                { "id", item.id },
                { "stars", item.stars },
                { "feature", item.feature },
                { "difficulty", item.difficulty },
                { "more-difficulties-override", item.moreDifficultiesOverride },
                { "grandpa-demon-override", item.grandpaDemonOverride },
                { "demons-in-between-override", item.demonsInBetweenOverride },
                { "gddp-integration-override", item.gddpIntegrationOverride },
                { "coins", item.coins }
            });
        }
        return arr;
    }

    static bool is_json(matjson::Value const& value) {
        return value.is_array();
    }
};
