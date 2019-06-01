#pragma once

#include "HellEngine/Input.h"

namespace HellEngine {

	class WindowsInput : public Input
	{
	public:
		static bool WasKeyPressedLastFrame[HELL_KEY_LAST + 1];

	protected:
		virtual bool IsKeyPressedImpl(int keycode) override;
		virtual bool IsKeyPressedOnceImpl(int keycode) override;

		virtual bool IsMouseButtonPressedImpl(int button) override;
		virtual std::pair<float, float> GetMousePositionImpl() override;
		virtual float GetMouseXImpl() override;
		virtual float GetMouseYImpl() override;
	};

}