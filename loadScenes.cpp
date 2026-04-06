#include "loadScenes.h"
#include "agreements.h"
#include <fstream>
#include <algorithm>

static bool coordEqual(const coordinate& a, const coordinate& b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}
std::vector<std::string> extractBracedBlocks(const std::string& input) {
    std::vector<std::string> result;
    int braceCount = 0;
    size_t startPos = std::string::npos;

    for (size_t i = 0; i < input.size(); ++i) {
        char c = input[i];
        if (c == '{') {
            if (braceCount == 0) {
                startPos = i + 1;
            }
            ++braceCount;
        }
        else if (c == '}') {
            if (braceCount == 0) {
                throw std::runtime_error("Unmatched closing brace '}'");
            }
            --braceCount;
            if (braceCount == 0 && startPos != std::string::npos) {
                std::string block = input.substr(startPos, i - startPos);
                result.push_back(block);
                startPos = std::string::npos;
            }
        }
    }

    if (braceCount != 0) {
        throw std::runtime_error("Unmatched opening brace '{'");
    }

    return result;
}
int loadfile_sceneconf(std::string filename, std::vector<skyScene>& SceneData, std::fstream& LogFile) {
    LogFile << SL::time::current_time_with_offset() << " Info:Configuring scene data - Loading from file -> " << filename << std::endl;
    skyScene SceneDataFromFile;
    std::string SceneDataFromFile_filedata = base::fio::file_read_all(filename);
    SceneDataFromFile_filedata.erase(std::remove_if(SceneDataFromFile_filedata.begin(), SceneDataFromFile_filedata.end(),
        [](unsigned char c) { return std::isspace(c); }),
        SceneDataFromFile_filedata.end());
    std::string prefix__s = "Atool-A-1:config:1.0:sky_candle:scene";
    if (SceneDataFromFile_filedata.find(prefix__s) == 0) {
        SceneDataFromFile_filedata.erase(0, prefix__s.length());
    }
    else {
        LogFile << SL::time::current_time_with_offset() << " Error: The configuration file format is not as expected." << std::endl;
        exit(-1);
    }
    std::vector<std::string>  SceneDataFromFile_filedata_v;
    try {
        SceneDataFromFile_filedata_v = extractBracedBlocks(SceneDataFromFile_filedata);
    }
    catch (const std::exception& e) {
        LogFile << SL::time::current_time_with_offset() << e.what() << std::endl;
        exit(-1);
    }
    if (SceneDataFromFile_filedata_v.size() < 4) {
        LogFile << SL::time::current_time_with_offset() << " Error: The configuration file format is not as expected." << std::endl;
        exit(-1);
    }

    auto SceneDataFromFile_filedata_time = base::data_process::part_str(SceneDataFromFile_filedata_v[3], ";");
    if (SL::time::days_diff(SceneDataFromFile_filedata_time[0], SL::time::current_time_with_offset()) % std::stoi(SceneDataFromFile_filedata_time[1]) != 0) {
        return 0;//skip
    }
    auto SceneDataFromFile_filedata_numandname = base::data_process::part_str_once(SceneDataFromFile_filedata_v[0], ";");
    auto SceneDataFromFile_filedata_t = base::data_process::part_str(SceneDataFromFile_filedata_v[1], "/");
    std::vector<sceneFrom> SceneDataFromFile_filedata_from;
    for (auto& tmp : SceneDataFromFile_filedata_t) {
        auto tmptmp = base::data_process::part_str(tmp, ";");
        sceneFrom tmp_1;
        auto tmp11 = base::data_process::part_str(tmptmp[1], ",");
        auto tmp12 = base::data_process::part_str(tmptmp[2], ",");
        tmp_1 = {
            std::stoi(tmptmp[0]),
            {std::stof(tmp11[0]),std::stof(tmp11[1]),std::stof(tmp11[2])},
            {std::stof(tmp12[0]),std::stof(tmp12[1]),std::stof(tmp12[2])}
        };
        SceneDataFromFile_filedata_from.push_back(tmp_1);
    }
    auto SceneDataFromFile_filedata_L = base::data_process::part_str(SceneDataFromFile_filedata_v[2], "/");
    std::vector<skyLight> SceneDataFromFile_filedata_light;
    for (auto& tmp : SceneDataFromFile_filedata_L) {
        auto tmptmp = base::data_process::part_str(tmp, ";");
        skyLight tmp_1;
        auto tmp11 = base::data_process::part_str(tmptmp[1], ",");
        tmp_1 = {
            {std::stof(tmp11[0]),std::stof(tmp11[1]),std::stof(tmp11[2])},
            std::stof(tmptmp[0]),
            {tmptmp[2],tmptmp[2]}
        };
        SceneDataFromFile_filedata_light.push_back(tmp_1);
    }
    SceneDataFromFile = {
        std::stoi(SceneDataFromFile_filedata_numandname[0]),
        {SceneDataFromFile_filedata_numandname[1],SceneDataFromFile_filedata_numandname[1]},
        SceneDataFromFile_filedata_from,
        SceneDataFromFile_filedata_light
    };
    //-----
    auto it = std::find_if(SceneData.begin(), SceneData.end(),
        [&](const skyScene& sc) { return sc.scene_num == SceneDataFromFile.scene_num; });

    if (it != SceneData.end()) {
        skyScene& existing = *it;
        existing.scene_name = SceneDataFromFile.scene_name;

        for (const auto& newLight : SceneDataFromFile.scene_light) {
            auto lightIt = std::find_if(existing.scene_light.begin(), existing.scene_light.end(),
                [&](const skyLight& l) { return coordEqual(l.lightCoordinate, newLight.lightCoordinate); });
            if (lightIt != existing.scene_light.end()) {
                lightIt->lightNum = newLight.lightNum;
                lightIt->lightName = newLight.lightName;
            }
            else {
                existing.scene_light.push_back(newLight);
            }
        }
        for (const auto& newFrom : SceneDataFromFile.scene_from) {
            auto fromIt = std::find_if(existing.scene_from.begin(), existing.scene_from.end(),
                [&](const sceneFrom& f) { return coordEqual(f.this_coordinate, newFrom.this_coordinate); });
            if (fromIt != existing.scene_from.end()) {
                fromIt->from_scene_num = newFrom.from_scene_num;
                fromIt->from_scene_coordinate = newFrom.from_scene_coordinate;
            }
            else {
                existing.scene_from.push_back(newFrom);
            }
        }

        LogFile << SL::time::current_time_with_offset() << " Info:Merged scene_num" << SceneDataFromFile.scene_num << std::endl;
    }
    else {
        SceneData.push_back(SceneDataFromFile);
        LogFile << SL::time::current_time_with_offset() << " Info:Added new scene_num" << SceneDataFromFile.scene_num << std::endl;
    }

    return 0;
}