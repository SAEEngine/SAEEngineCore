#pragma once
#ifndef SAE_ENGINE_CORE_INPUT_H
#define SAE_ENGINE_CORE_INPUT_H

#include <SAEEngineCore_Event.h>

#include <cstdint>

struct GLFWwindow;

namespace sae::engine::core
{

	struct Cursor
	{
		struct Position
		{
			int16_t x = 0;
			int16_t y = 0;
		};

		static Position get_position(GLFWwindow* _window);

	};



}

#endif