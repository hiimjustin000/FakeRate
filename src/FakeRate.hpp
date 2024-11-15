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
private:
    inline static std::unordered_map<std::string, int> GDDP_INDICES = {
        { "Beginner", 1 },
        { "Bronze", 2 },
        { "Silver", 3 },
        { "Gold", 4 },
        { "Amber", 5 },
        { "Platinum", 6 },
        { "Sapphire", 7 },
        { "Jade", 8 },
        { "Emerald", 9 },
        { "Ruby", 10 },
        { "Diamond", 11 },
        { "Onyx", 12 },
        { "Amethyst", 13 },
        { "Azurite", 14 },
        { "Obsidian", 15 }
    };
public:
    static int getBaseCurrency(int);
    static int getDifficultyFromLevel(GJGameLevel*);
    static std::string getSpriteName(cocos2d::CCSprite*);
    static void toggle(cocos2d::CCNode*, bool);
    static cocos2d::CCPoint getDIBOffset(int, GJDifficultyName);
    static int getGRDOverride(cocos2d::CCSprite*);
    static int getDIBOverride(cocos2d::CCSprite*);
    static int getGDDPOverride(cocos2d::CCSprite*);
    static std::string getGDDPFrame(int, GJDifficultyName);
};

template<>
struct matjson::Serialize<std::vector<FakeRateSaveData>> {
    static geode::Result<std::vector<FakeRateSaveData>> fromJson(matjson::Value const& value) {
        if (!value.isArray()) return geode::Err("Expected array");

        std::vector<FakeRateSaveData> vec;
        for (auto const& item : value.asArray().unwrap()) {
            vec.push_back({
                .id = (int)item["id"].asInt().unwrapOr(0),
                .stars = (int)item["stars"].asInt().unwrapOr(0),
                .feature = (int)item["feature"].asInt().unwrapOr(0),
                .difficulty = (int)item["difficulty"].asInt().unwrapOr(0),
                .moreDifficultiesOverride = (int)item["more-difficulties-override"].asInt().unwrapOr(0),
                .grandpaDemonOverride = (int)item["grandpa-demon-override"].asInt().unwrapOr(0),
                .demonsInBetweenOverride = (int)item["demons-in-between-override"].asInt().unwrapOr(0),
                .gddpIntegrationOverride = (int)item["gddp-integration-override"].asInt().unwrapOr(0),
                .coins = item["coins"].asBool().unwrapOr(true)
            });
        }

        return geode::Ok(vec);
    }

    static matjson::Value toJson(std::vector<FakeRateSaveData> const& vec) {
        std::vector<matjson::Value> arr;
        for (auto const& item : vec) {
            arr.push_back(matjson::makeObject({
                { "id", item.id },
                { "stars", item.stars },
                { "feature", item.feature },
                { "difficulty", item.difficulty },
                { "more-difficulties-override", item.moreDifficultiesOverride },
                { "grandpa-demon-override", item.grandpaDemonOverride },
                { "demons-in-between-override", item.demonsInBetweenOverride },
                { "gddp-integration-override", item.gddpIntegrationOverride },
                { "coins", item.coins }
            }));
        }
        return arr;
    }
};
