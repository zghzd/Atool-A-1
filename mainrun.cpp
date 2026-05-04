#include "mainrun.h"
#include "agreements.h"
#include "loadScenes.h"
#include "LIGHT_TO_CANDLE_MAP.h"
#include "sky_scene_sceneenum.h"
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

#include "calcumain_v1.h"

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

//-----------------------------------------
int mainrun(float req_candle,
	float max_candle,
	std::vector<std::string> use_file,
	std::vector<std::string> no_keyword,
	std::string use_language,
	std::fstream& LogFile) {
    float total_candle;
    if (max_candle < req_candle) { total_candle = max_candle; }
    else { total_candle = req_candle; }
    std::vector<skyScene>skySceneS;
    auto LDreturnCode = ldFileData(skySceneS,
        use_file,
        no_keyword,
        use_language,
        LogFile);
    if (LDreturnCode < 0) {
        LogFile << SL::time::current_time_with_offset() << "Error: Function ldFileData 's return is " << LDreturnCode
            << ". It's not finished or at known state!\n";
        return LDreturnCode;
    }

    calcumain_v1(total_candle, skySceneS, LogFile);

    LogFile << SL::time::current_time_with_offset() << "Finished\n==========" << std::endl;
    return 0;
}