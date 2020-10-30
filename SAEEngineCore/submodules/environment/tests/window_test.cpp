#include "Environment.h"

#include <cassert>

namespace eng = sae::engine::core;

int main()
{
	eng::Window _window{ nullptr };
	assert(!_window.good());
	assert(_window.get() == nullptr);

	eng::Window_Ref _wref{ _window };
	assert(!_wref.good());
	assert(_wref.get() == nullptr);

	_window.destroy();
	assert(!_window.good());
	assert(_window.get() == nullptr);

	return 0;
};