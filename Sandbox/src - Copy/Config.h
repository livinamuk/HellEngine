#pragma once

namespace HellEngine
{
	class Config
	{

	public: // fields
		static inline bool DOF_showFocus = false;		//show debug focus point and focal range (red = focal point, green = focal range)
		static bool DOF_vignetting;		//use optical lens vignetting?
		static float DOF_vignout;		//vignetting outer border
		static float DOF_vignin;		//vignetting inner border
		static float DOF_vignfade;		//f-stops till vignete fades
		static float DOF_CoC;			//circle of confusion size in mm (35mm film = 0.03mm)
		static float DOF_maxblur;		//clamp value of max blur (0.0 = no blur,1.0 default)
		static int DOF_samples;			//samples on the first ring
		static int DOF_rings;			//ring count
		static float DOF_threshold;		//highlight threshold;
		static float DOF_gain;			//highlight gain;
		static float DOF_bias;			//bokeh edge bias
		static float DOF_fringe;		//bokeh chromatic aberration/fringing

	public: // methods

		static void Load()
		{
			DOF_showFocus = false;
			DOF_vignetting = true;
			DOF_vignout = 1.0;
			DOF_vignin = 0.0;
			DOF_vignfade = 122.0;
			DOF_CoC = 0.03;
			DOF_maxblur = 0.6; 
			DOF_samples = 1;	// 3 was default
			DOF_rings = 4;	// 4 was default 
			DOF_threshold = 0.5;
			DOF_gain = 2.0;
			DOF_bias = 0.5;
			DOF_fringe = 0.7;
		}
	};
}