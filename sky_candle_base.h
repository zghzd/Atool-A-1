#pragma once
#include <string>
#include <vector>
namespace sky_::candle {
	//lights===================================================================================
	inline int	L_CANDLE = 1001;
	inline int L_CLAM = 1002;
	inline int L_DARKNESS_TREE = 1003;
	inline int L_DARKNESS_PLANT = 1004;
	inline int L_UNDERWATER_LIGHTS = 1005;
	//===================================================================================
	struct Axyz {
		float x;//right/left
		float y;//up/down
		float z;//front/back
	};
	struct light {
		int type;
		int num;
		Axyz light_xyz;
	};
	struct tp_scene {
		Axyz tp_xyz;//where
		int s_code;//goal scene code
		Axyz to_tp_xyz;//where you at next scene
	};
	struct scene {
		std::string name;
		int s_code;
		std::vector<light>s_light;
		std::vector<tp_scene>tps;
		int t;//time of every "tps",1t=1s
	};
}