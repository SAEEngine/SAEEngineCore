#include "SAEEngineCore_Environment.h"

#include <functional>
#include <cassert>

namespace sae::engine::core
{

	bool GLFWLib::init()
	{
		if (!this->good())
		{
			this->good_ = glfwInit();
#ifdef SAE_ENGINE_CORE_HARD_ERRORS
			assert(this->good());
#endif
		};
		return this->good();
	};
	bool GLFWLib::good() const noexcept
	{
		return this->good_;
	};
	void GLFWLib::destroy()
	{
		if (this->good())
		{
			glfwTerminate();
			this->good_ = false;
		};
	};
	void GLFWLib::poll_events()
	{
		glfwPollEvents();
	};

	GLFWLib::GLFWLib()
	{
		this->init();
	};
	GLFWLib::~GLFWLib()
	{
		this->destroy();
	};

}

namespace sae::engine::core
{

	bool GLFWLib_Ref::good() const noexcept
	{
		return (bool)this->lib_;
	};
	GLFWLib_Ref::operator bool() const noexcept
	{
		return this->good();
	};

	GLFWLib_Ref::pointer GLFWLib_Ref::get() const noexcept
	{ 
		return this->lib_.get();
	};
	GLFWLib_Ref::pointer GLFWLib_Ref::operator->() const noexcept 
	{ 
		return this->lib_.get(); 
	};

	GLFWLib_Ref::reference GLFWLib_Ref::operator*()
	{
		return *this->get();
	};
	GLFWLib_Ref::const_reference GLFWLib_Ref::operator*() const
	{
		return *this->get();
	};

	GLFWLib_Ref::GLFWLib_Ref(std::shared_ptr<value_type> _lib) : 
		lib_{ _lib }
	{};
	GLFWLib_Ref& GLFWLib_Ref::operator=(std::shared_ptr<value_type> _lib)
	{
		this->lib_ = _lib;
		return *this;
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

	bool Window::is_current() const
	{
		return (this->good() && glfwGetCurrentContext() == this->get());
	};

	void Window::make_current()
	{
		if (!this->is_current())
		{
			glfwMakeContextCurrent(this->get());
		};
	};

	void Window::swap_buffers()
	{
		glfwSwapBuffers(this->get());
	};

	Window::Window(pointer _ptr) :
		ptr_{ _ptr }
	{
		if (glfwGetCurrentContext() == NULL)
		{
			glfwMakeContextCurrent(this->get());
			gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
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