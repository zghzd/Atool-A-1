#include "mainrun.h"
#include "agreements.h"
#include "loadScenes.h"
#include "scenereg.h"
#include "LIGHT_TO_CANDLE_MAP.h"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <map>
#include <queue>
#include <limits>
#include <set>
#include <sstream>
#include <iomanip>

std::unordered_map<int, skyScene*> skySceneMap;
std::vector<skyScene>SceneData;

const float teleport_usedTime = 5.0f;
const float meditate_usedTime = 6.5f;
const int startP = S_HOME;
const coordinate startCD = { 0,0,0 };//constellation at HOME

float get_distance(coordinate A, coordinate B) {
    float dx = A.x - B.x;
    float dy = A.y - B.y;
    float dz = A.z - B.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

skyScene *findSceneBySNum(int target_num) {
    auto it = skySceneMap.find(target_num);
    if (it != skySceneMap.end()) {
        skyScene* result = it->second;
        return result;
    }
    else {
        return nullptr;
    }
}

std::vector<int> whichCanBeTransmitted(int now_scene_num) {
    //@return scene_numWhichIsCanBeTransmittedScene's.
    std::vector<int> result;
    for (const auto& scene : SceneData) {
        for (const auto& from : scene.scene_from) {
            if (from.from_scene_num == now_scene_num) {
                result.push_back(scene.scene_num);
                break;
            }
        }
    }
    return result;
}
//-----------------------------------------
float lightToCandle(float totalLight) {
    if (totalLight <= 0.0f) return 0.0f;
    float candle = 0.0f;
    float prevLight = 0.0f, prevCandle = 0.0f;
    for (const auto& entry : LIGHT_TO_CANDLE_MAP) {
        float lightThreshold = entry.first;
        float candleValue = entry.second;
        if (totalLight <= lightThreshold) {
            float ratio = (totalLight - prevLight) / (lightThreshold - prevLight);
            candle = prevCandle + ratio * (candleValue - prevCandle);
            return candle;
        }
        prevLight = lightThreshold;
        prevCandle = candleValue;
    }

    //超过20蜡烛部分：第21根需2000，第22根需4000，依此类推
    float excessLight = totalLight - prevLight;
    float candle20 = 20.0f;
    float needForNext = 2000.0f;
    int nextCandle = 21;
    while (excessLight > 0.0f) {
        if (excessLight <= needForNext) {
            candle = candle20 + (excessLight / needForNext);
            return candle;
        }
        excessLight -= needForNext;
        candle20 += 1.0f;
        needForNext *= 2.0f;
    }
    return candle20;
}

float maxCandleFromLights() {
    float totalLight = 0.0f;
    for (const auto& scene : SceneData) {
        for (const auto& light : scene.scene_light) {
            totalLight += light.lightNum;
        }
    }
    return lightToCandle(totalLight);
}

float lightNeededForCandle(float currentLight, float needCandle) {
    float low = 0.0f, high = currentLight + 10000000.0f;//上界足够大
    for (int iter = 0; iter < 100; ++iter) {
        float mid = (low + high) * 0.5f;
        float candle = lightToCandle(mid);
        if (candle < needCandle) low = mid;
        else high = mid;
    }
    return high - currentLight;
}

struct LightNode {
    int sceneNum;
    size_t lightIndex;
    coordinate pos;
    float lightValue;
    name_cn_en name;
};

struct RouteStep {
    enum Type { MOVE, TELEPORT, COLLECT, RETURN_HOME };
    Type type;
    int fromScene;
    int toScene;
    coordinate fromPos;
    coordinate toPos;
    std::string descriptionCN;
    std::string descriptionEN;
    float timeCost;
    float lightGain;
};

float computeMinTimeToLight(const coordinate& currentPos, int currentScene,
    const LightNode& target,
    std::vector<RouteStep>& dummySteps) {
    //在同一场景，直接移动
    if (currentScene == target.sceneNum) {
        return get_distance(currentPos, target.pos);
    }
    float bestTime = std::numeric_limits<float>::max();
    skyScene* fromScenePtr = findSceneBySNum(currentScene);
    skyScene* toScenePtr = findSceneBySNum(target.sceneNum);
    if (!fromScenePtr || !toScenePtr) return bestTime;

    for (const auto& from : toScenePtr->scene_from) {
        if (from.from_scene_num == currentScene) {
            float distInCurrent = get_distance(currentPos, from.from_scene_coordinate);
            float distInTarget = get_distance(from.this_coordinate, target.pos);
            float total = distInCurrent + teleport_usedTime + distInTarget;
            if (total < bestTime) bestTime = total;
        }
    }

    return bestTime;
}

//从当前位置到目标的路径
std::vector<RouteStep> planRouteToLight(const coordinate& startPos, int startScene,
    const LightNode& target,
    float& outTimeCost) {
    std::vector<RouteStep> steps;
    outTimeCost = 0.0f;

    if (startScene == target.sceneNum) {
        float dist = get_distance(startPos, target.pos);
        if (dist > 0.01f) {
            steps.push_back({ RouteStep::MOVE, startScene, startScene, startPos, target.pos,
                             "移动到 " + target.name.cn, "Move to " + target.name.en,
                             dist, 0.0f });
        }
        outTimeCost = dist;
        return steps;
    }

    //Dijkstra
    skyScene* toScenePtr = findSceneBySNum(target.sceneNum);
    float bestTime = std::numeric_limits<float>::max();
    sceneFrom bestFrom;
    bool found = false;

    for (const auto& from : toScenePtr->scene_from) {
        if (from.from_scene_num == startScene) {
            float dist1 = get_distance(startPos, from.from_scene_coordinate);
            float dist2 = get_distance(from.this_coordinate, target.pos);
            float total = dist1 + teleport_usedTime + dist2;
            if (total < bestTime) {
                bestTime = total;
                bestFrom = from;
                found = true;
            }
        }
    }

    if (!found) {
        outTimeCost = std::numeric_limits<float>::max();
        return steps;
    }

    //移动到传送点
    float distToPortal = get_distance(startPos, bestFrom.from_scene_coordinate);
    if (distToPortal > 0.01f) {
        steps.push_back({ RouteStep::MOVE, startScene, startScene, startPos, bestFrom.from_scene_coordinate,
                         "移动到传送点", "Move to portal", distToPortal, 0.0f });
    }

    //传送
    skyScene* fromScenePtr = findSceneBySNum(startScene);
    skyScene* toScenePtr2 = findSceneBySNum(target.sceneNum);
    std::string fromNameCN = fromScenePtr ? fromScenePtr->scene_name.cn : std::to_string(startScene);
    std::string toNameCN = toScenePtr2 ? toScenePtr2->scene_name.cn : std::to_string(target.sceneNum);
    std::string fromNameEN = fromScenePtr ? fromScenePtr->scene_name.en : std::to_string(startScene);
    std::string toNameEN = toScenePtr2 ? toScenePtr2->scene_name.en : std::to_string(target.sceneNum);

    steps.push_back({ RouteStep::TELEPORT, startScene, target.sceneNum, bestFrom.from_scene_coordinate, bestFrom.this_coordinate,
                     "从 " + fromNameCN + " 传送到 " + toNameCN,
                     "Teleport from " + fromNameEN + " to " + toNameEN,
                     teleport_usedTime, 0.0f });

    //移动到目标
    float distToLight = get_distance(bestFrom.this_coordinate, target.pos);
    if (distToLight > 0.01f) {
        steps.push_back({ RouteStep::MOVE, target.sceneNum, target.sceneNum, bestFrom.this_coordinate, target.pos,
                         "移动到 " + target.name.cn, "Move to " + target.name.en,
                         distToLight, 0.0f });
    }

    outTimeCost = distToPortal + teleport_usedTime + distToLight;
    return steps;
}

//-----------------------------------------
int mainrun(float req_candle,
	float max_candle,
	std::vector<std::string> use_file,
	std::vector<std::string> no_keyword,
	std::string use_language,
	std::fstream& LogFile) {

	SceneData = {

	};//硬编码的内容，必须在此处注册名称

	for (auto& tmp : use_file) {
		loadfile_sceneconf(tmp, SceneData, LogFile);
	}
    for (auto& tmp : no_keyword) {
        for (auto& tmp1 : SceneData) {
            tmp1.scene_light.erase(
                std::remove_if(
                    tmp1.scene_light.begin(),
                    tmp1.scene_light.end(),
                    [&](const skyLight& tmp2) {
                        return (tmp2.lightName.cn.find(tmp) != std::string::npos) ||
                            (tmp2.lightName.en.find(tmp) != std::string::npos);
                    }
                ),
                tmp1.scene_light.end()
            );
        }
    }

    //-----------------    use:SceneData
    for (auto& SceneDatatmp : SceneData) {
        skySceneMap[SceneDatatmp.scene_num] = &SceneDatatmp;
    }
    float totalCandleAvailable = maxCandleFromLights();
    if (max_candle == 0.0f) max_candle = totalCandleAvailable;
    else max_candle = std::min(max_candle, totalCandleAvailable);

    float needCandle = req_candle;
    float cycleNum = 0.0f;
    std::string strCycleCN, strCycleEN;

    if (req_candle > max_candle) {
        needCandle = max_candle;
        cycleNum = std::ceil(req_candle / max_candle);
        strCycleCN = "循环 " + std::to_string((int)cycleNum) + " 次";
        strCycleEN = "Cycle " + std::to_string((int)cycleNum) + " times";
    }

    if (needCandle <= 0.0f) {
        LogFile << SL::time::current_time_with_offset() << "The demand is 0, no calculation is needed." << std::endl;
        return 0;
    }

    std::vector<LightNode> allLights;
    for (auto& scene : SceneData) {
        for (size_t i = 0; i < scene.scene_light.size(); ++i) {
            const auto& light = scene.scene_light[i];
            allLights.push_back({ scene.scene_num, i, light.lightCoordinate,
                                 light.lightNum, light.lightName });
        }
    }

    std::vector<RouteStep> fullRoute;
    float currentLight = 0.0f;
    float currentCandle = 0.0f;
    float totalTime = 0.0f;
    coordinate currentPos = startCD;
    int currentScene = startP;

    std::vector<bool> collected(allLights.size(), false);
    size_t remaining = allLights.size();

    const float TELEPORT_PENALTY = 5.0f;
    const float EFFICIENCY_WEIGHT = 0.1f;

    while (currentCandle < needCandle && remaining > 0) {
        float needLight = lightNeededForCandle(currentLight, needCandle);

        size_t bestIdx = 0;
        float bestScore = -std::numeric_limits<float>::max();
        float bestTime = 0.0f;
        std::vector<RouteStep> bestSteps;

        for (size_t i = 0; i < allLights.size(); ++i) {
            if (collected[i]) continue;
            const auto& light = allLights[i];

            std::vector<RouteStep> dummy;
            float estTime = computeMinTimeToLight(currentPos, currentScene, light, dummy);
            if (estTime >= 1e9f) continue;//不可达

            float lightGain = light.lightValue;
            int teleportCount = (currentScene != light.sceneNum) ? 1 : 0;
            float penalty = teleportCount * TELEPORT_PENALTY;
            float efficiency = lightGain / (estTime + penalty);
            float score = efficiency;

            if (score > bestScore) {
                bestScore = score;
                bestIdx = i;
                bestTime = estTime;
            }
        }

        if (bestScore < 0) {
            LogFile << SL::time::current_time_with_offset() << "Error: Unable to continue collecting, the path may be disconnected." << std::endl;
            break;
        }

        const auto& targetLight = allLights[bestIdx];
        float actualTime;
        bestSteps = planRouteToLight(currentPos, currentScene, targetLight, actualTime);

        collected[bestIdx] = true;
        remaining--;

        //更新状态
        currentPos = targetLight.pos;
        currentScene = targetLight.sceneNum;
        currentLight += targetLight.lightValue;
        currentCandle = lightToCandle(currentLight);
        totalTime += actualTime;

        //记录收集步骤
        bestSteps.push_back({ RouteStep::COLLECT, currentScene, currentScene, currentPos, currentPos,
                             "收集 " + targetLight.name.cn + " (+" + std::to_string(targetLight.lightValue) + " 光)",
                             "Collect " + targetLight.name.en + " (+" + std::to_string(targetLight.lightValue) + " light)",
                             0.0f, targetLight.lightValue });
        fullRoute.insert(fullRoute.end(), bestSteps.begin(), bestSteps.end());

        LogFile << SL::time::current_time_with_offset() << "Collect:" << targetLight.name.cn
            << ",Cumulative Candles:" << currentCandle << "/" << needCandle << std::endl;
    }

    //输出结果
    bool isCN = (use_language == "cn");

    std::cout << (isCN ? "=== 路线规划结果 ===" : "=== Route Planning Result ===") << std::endl;
    if (cycleNum > 0) {
        std::cout << (isCN ? strCycleCN : strCycleEN) << std::endl;
        std::cout << (isCN ? "单次路线耗时: " : "Single route time: ") << totalTime << (isCN ? " 秒" : " seconds") << std::endl;
    }
    else {
        std::cout << (isCN ? "总耗时: " : "Total time: ") << totalTime << (isCN ? " 秒" : " seconds") << std::endl;
    }
    std::cout << (isCN ? "收集蜡烛: " : "Candles collected: ") << currentCandle << std::endl;

    std::cout << (isCN ? "\n详细路线:" : "\nDetailed Route:") << std::endl;
    int stepNum = 1;
    for (const auto& step : fullRoute) {
        std::cout << stepNum++ << ". ";
        if (isCN) {
            std::cout << step.descriptionCN;
            if (step.timeCost > 0) std::cout << " (耗时 " << step.timeCost << "s)";
            if (step.lightGain > 0) std::cout << " 获得 " << step.lightGain << " 光";
        }
        else {
            std::cout << step.descriptionEN;
            if (step.timeCost > 0) std::cout << " (Time " << step.timeCost << "s)";
            if (step.lightGain > 0) std::cout << " Gain " << step.lightGain << " light";
        }
        std::cout << std::endl;
    }

    if (currentCandle < needCandle) {
        std::cout << (isCN ? "警告：未能达到目标蜡烛数，光之碎片不足。" : "Warning: Target candle amount not reached, insufficient lights.") << std::endl;
    }

    LogFile << SL::time::current_time_with_offset() << "finished\n==========" << std::endl;
    //-------------------
    return 0;
}