#pragma once

namespace HellEngine
{
	class Config
	{
	public:
		static inline bool DOF_showFocus = false;		//show debug focus point and focal range (red = focal point, green = focal range)
		static inline bool DOF_vignetting = true;		//use optical lens vignetting?
		static inline float DOF_vignout = 1.0;			//vignetting outer border
		static inline float DOF_vignin = 0.0;			//vignetting inner border
		static inline float DOF_vignfade = 122.0;		//f-stops till vignete fades
		static inline float DOF_CoC = 0.03;				//circle of confusion size in mm (35mm film = 0.03mm)
		static inline float DOF_maxblur = 0.6;			//clamp value of max blur (0.0 = no blur,1.0 default)
		static inline int DOF_samples = 1;				//samples on the first ring
		static inline int DOF_rings = 4;				//ring count
		static inline float DOF_threshold = 0.5;		//highlight threshold;
		static inline float DOF_gain = 2.0;				//highlight gain;
		static inline float DOF_bias = 0.5;				//bokeh edge bias
		static inline float DOF_fringe = 0.7;			//bokeh chromatic aberration/fringing
	};
}