#include "Environment.h"

#include "SAEEngineCoreConfig.h"

#include <GLFW/glfw3.h>

#include <functional>
#include <cassert>

namespace sae::engine::core
{

	bool GLFWLib::init()
	{
		this->status_ = glfwInit();
#ifdef SAE_ENGINE_CORE_HARD_ERRORS
		assert(this->good());
#endif
		return this->good();
	};

	int GLFWLib::status() const noexcept
	{

	};
	bool GLFWLib::good() const noexcept
	{

	};

	void GLFWLib::destroy()
	{

	};

	GLFWLib::GLFWLib()
	{
		this->init();
	};
	GLFWLib::~GLFWLib()
	{

	};

}




namespace sae::engine::core
{

	void Window::destroy()
	{
		if (this->good())
		{
			glfwDestroyWindow(this->ptr_);
			this->ptr_ = nullptr;
		};
	};

	Window::Window(Window&& other) noexcept :
		ptr_{ std::exchange(other.ptr_, nullptr) }
	{};
	Window& Window::operator=(Window&& other) noexcept
	{
		this->destroy();
		this->ptr_ = std::exchange(other.ptr_, nullptr);
		return *this;
	};

	Window::~Window()
	{
		this->destroy();
	};

};

namespace sae::engine::core
{

	Window_Ref& Window_Ref::operator=(pointer _window) noexcept
	{
		this->ptr_ = _window;
		return *this;
	};

	Window_Ref& Window_Ref::operator=(const Window& _window) noexcept
	{
		this->ptr_ = _window.get();
		return *this;
	};

};