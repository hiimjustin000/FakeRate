#include <Geode/Geode.hpp>

using namespace geode::prelude;

struct FakeRateSaveData {
    int id;
    int stars;
    int feature;
    int difficulty;
    int moreDifficultiesOverride;
};

class FakeRate {
public:
    static int getBaseCurrency(int);
    static int getDifficultyFromLevel(GJGameLevel*);
    static std::string getSpriteName(CCSprite*);
    static void toggle(CCNode*, bool);
};

template<>
struct matjson::Serialize<std::vector<FakeRateSaveData>> {
    static std::vector<FakeRateSaveData> from_json(matjson::Value const& value) {
        auto vec = std::vector<FakeRateSaveData> {};
        for (auto const& item : value.as_array()) {
            vec.push_back({
                .id = item["id"].as_int(),
                .stars = item["stars"].as_int(),
                .feature = item["feature"].as_int(),
                .difficulty = item["difficulty"].as_int(),
                .moreDifficultiesOverride = item.contains("more-difficulties-override") ? item["more-difficulties-override"].as_int() : 0
            });
        }
        return vec;
    }

    static matjson::Value to_json(std::vector<FakeRateSaveData> const& vec) {
        auto arr = matjson::Array {};
        for (auto const& item : vec) {
            arr.push_back(matjson::Object {
                { "id", item.id },
                { "stars", item.stars },
                { "feature", item.feature },
                { "difficulty", item.difficulty },
                { "more-difficulties-override", item.moreDifficultiesOverride }
            });
        }
        return arr;
    }

    static bool is_json(matjson::Value const& value) {
        return value.is_array();
    }
};
