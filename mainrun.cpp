#include "mainrun.h"
#include "agreements.h"
#include "loadScenes.h"
#include "scenereg.h"
#include <fstream>
#include <algorithm>

int mainrun(float req_candle,
	float max_candle,
	std::vector<std::string> use_file,
	std::vector<std::string> no_keyword,
	std::string use_language,
	std::fstream& LogFile) {
	std::vector<skyScene>SceneData;
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
    /*for (auto& tmp : SceneData) {
        if ((tmp.ableNum == 0) || (SL::time::days_diff(tmp.baseline_time, SL::time::current_time_with_offset()) % tmp.ableNum != 0)) {
            std::erase_if(SceneData, [](const skyScene& tmp) {
                if (tmp.ableNum == 0) return true;
                int diff = SL::time::days_diff(tmp.baseline_time, SL::time::current_time_with_offset());
                return diff % tmp.ableNum != 0;
                });
        }
    }*///硬编码的数据是常驻数据，不应出现此类现象

    //TODO:计算

    return 0;
}