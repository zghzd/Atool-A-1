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
const float meditate_usedTime = 5.5f;
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
        if (loadfile_sceneconf(tmp, SceneData, LogFile) != 0) {
            LogFile << SL::time::current_time_with_offset() << " Skipping invalid config file: " << tmp << std::endl;
            continue;
        }
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
        strCycleCN = " 循环 " + std::to_string((int)cycleNum) + " 次";
        strCycleEN = " Cycle " + std::to_string((int)cycleNum) + " times";
    }

    if (needCandle <= 0.0f) {
        LogFile << SL::time::current_time_with_offset() << " The demand is 0, no calculation is needed." << std::endl;
        return 0;
    }

    
    LogFile << SL::time::current_time_with_offset() << "Finished\n==========" << std::endl;
    return 0;
}