#include "ai/UtilityBuilder.h"

std::unique_ptr<Blainn::UtilitySelector> Blainn::UtilityBuilder::Build(sol::table luaTable)
{
    UtilitySelector::Settings settings;

    if (luaTable["normalize"].valid())
        settings.normalize = luaTable["normalize"];

    if (luaTable["hysteresis"].valid())
        settings.hysteresis = luaTable["hysteresis"];

    std::vector<UtilityDecision> decisions;

    sol::table luaDecisions = luaTable["decisions"];
    for (auto& kv : luaDecisions)
    {
        sol::table d = kv.second;

        UtilityDecision decision;
        decision.name = d["name"];
        decision.BTName = d["bt"];
        decision.scoreFn = d["score"];

        if (d["cooldown"].valid())
            decision.cooldown = d["cooldown"];

        decisions.push_back(std::move(decision));
    }

    return std::make_unique<UtilitySelector>(std::move(decisions), settings);
}

// Вот так должен выглядеть table в lua для utility
// UtilityAI =
// {
//     normalize = true,
//     hysteresis = 0.2,

//     decisions =
//     {
//         {
//             name = "Idle",
//             bt   = "Idle",

//             score = function(bb)
//                 return 0.1
//             end
//         },

//         {
//             name = "Chase",
//             bt   = "Chase",

//             cooldown = 1.0,
//             score = function(bb)
//                 return bb.seesEnemy and 1.0 or 0.0
//             end
//         }
//     }
// }