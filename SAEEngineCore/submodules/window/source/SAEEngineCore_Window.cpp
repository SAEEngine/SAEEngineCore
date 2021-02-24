#include "SAEEngineCore_Window.h"

#include <SAEEngineCore_Input.h>
#include <SAEEngineCore_Event.h>
#include <SAEEngineCore_Object.h>

#include <GLFW/glfw3.h>

namespace sae::engine::core
{

	void WindowEventAdapter::glfw_mouse_button_callback(GLFWwindow* _window, int _button, int _action, int _mods)
	{
		auto _ptr = (WindowEventAdapter*)glfwGetWindowUserPointer(_window);
		if (_ptr)
		{
			Event::evMouse _evm{};
			_evm.action = _action;
			_evm.button = _button;
			_evm.mods = _mods;

			auto _cursorPos = Cursor::get_position(_window);
			_evm.cursor_x = _cursorPos.x;
			_evm.cursor_y = _cursorPos.y;

			Event _ev{ _evm };
			_ptr->context_->handle_event(_ev);

		};
	};

	void WindowEventAdapter::glfw_scroll_callback(GLFWwindow* _window, double _x, double _y)
	{
		auto _ptr = (WindowEventAdapter*)glfwGetWindowUserPointer(_window);
		if (_ptr)
		{
			Event::evScroll _event{ _x, _y };
			Event _ev{ _event , true };
			_ptr->context_->handle_event(_ev);
		};
	};

	void WindowEventAdapter::glfw_key_callback(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods)
	{
		auto _ptr = (WindowEventAdapter*)glfwGetWindowUserPointer(_window);
		if (_ptr)
		{
			Event::evKey _event{};
			_event.key = _key;
			_event.scancode = _scancode;
			_event.action = _action;
			_event.mods = _mods;
			Event _ev{ _event, true };
			_ptr->context_->handle_event(_ev);

			if (!_ev)
			{
				_ptr->context_->refresh();
			};

		};
	};
	void WindowEventAdapter::glfw_text_callback(GLFWwindow* _window, unsigned _codepoint)
	{
		auto _ptr = (WindowEventAdapter*)glfwGetWindowUserPointer(_window);
		if (_ptr)
		{
			Event::evText _event{};
			_event.codepoint = _codepoint;
			Event _ev{ _event, true };
			_ptr->context_->handle_event(_ev);

			if (!_ev)
			{
				_ptr->context_->refresh();
			};
		};
	};

	void WindowEventAdapter::glfw_cursor_move_callback(GLFWwindow* _window, double _x, double _y)
	{
		auto _ptr = (WindowEventAdapter*)glfwGetWindowUserPointer(_window);
		if (_ptr)
		{
			Event::evCursorMove _event{};
			_event.cursor_x = (int16_t)_x;
			_event.cursor_y = (int16_t)_y;
			Event _ev{ _event, true };
			_ptr->context_->handle_event(_ev);
			if (!_ev)
			{
				_ptr->context_->refresh();
			};
		};
	};
	void WindowEventAdapter::glfw_cursor_enter_callback(GLFWwindow* _window, int _entered)
	{
		auto _ptr = (WindowEventAdapter*)glfwGetWindowUserPointer(_window);
		if (_ptr)
		{
			Event::evCursorWindowBounds _event{};
			if (_entered)
			{
				_event.action == Event::evCursorWindowBounds::ENTER;
			}
			else
			{
				_event.action == Event::evCursorWindowBounds::EXIT;
			};
			Event _ev{ _event, true };

			_ptr->context_->handle_event(_ev);

			if (!_ev)
			{
				_ptr->context_->refresh();
			};
		};
	};

	void WindowEventAdapter::glfw_framebuffer_resize_callback(GLFWwindow* _window, int _width, int _height)
	{
		auto _ptr = (WindowEventAdapter*)glfwGetWindowUserPointer(_window);

		if (_ptr)
		{
			Event::evGrow _event{};
			auto& _wb = _ptr->context_->bounds();

			_event.dw = _width - _wb.width();
			_event.dh = _height - _wb.height();

			_wb.right() = _width;
			_wb.bottom() = _height;

			Event _ev{ _event, true };
			_ptr->context_->handle_event(_ev);
			_ptr->context_->refresh();
			
		};
	};

	void WindowEventAdapter::glfw_window_close_callback(GLFWwindow* _window)
	{
		auto _ptr = (WindowEventAdapter*)glfwGetWindowUserPointer(_window);
		if (_ptr)
		{
			Event _ev{ Event::evWindowClose{}, true };
			_ptr->context_->handle_event(_ev);
		};
	};

	void WindowEventAdapter::glfw_window_refresh_callback(GLFWwindow* _window)
	{
		auto _ptr = (WindowEventAdapter*)glfwGetWindowUserPointer(_window);
		if (_ptr)
		{
			int _w = 0;
			int _h = 0;
			glfwGetFramebufferSize(_window, &_w, &_h);
			WindowEventAdapter::glfw_framebuffer_resize_callback(_window, _w, _h);

			Event _ev{ Event::evRefresh{} };
			_ptr->context_->handle_event(_ev);

		};

	};

	WindowEventAdapter::WindowEventAdapter(GFXContext* _context) : 
		context_{ _context }
	{
		glfwSetWindowUserPointer(this->context_->window(), this);
		glfwSetMouseButtonCallback(this->context_->window(), &WindowEventAdapter::glfw_mouse_button_callback);

		glfwSetScrollCallback(this->context_->window(), &WindowEventAdapter::glfw_scroll_callback);

		glfwSetKeyCallback(this->context_->window(), &WindowEventAdapter::glfw_key_callback);
		glfwSetCharCallback(this->context_->window(), &WindowEventAdapter::glfw_text_callback);

		glfwSetCursorPosCallback(this->context_->window(), &WindowEventAdapter::glfw_cursor_move_callback);
		glfwSetCursorEnterCallback(this->context_->window(), &WindowEventAdapter::glfw_cursor_enter_callback);

		glfwSetWindowCloseCallback(this->context_->window(), &WindowEventAdapter::glfw_window_close_callback);
		glfwSetFramebufferSizeCallback(this->context_->window(), &WindowEventAdapter::glfw_framebuffer_resize_callback);

		glfwSetWindowRefreshCallback(this->context_->window(), &WindowEventAdapter::glfw_window_refresh_callback);

		int _winX = 0;
		int _winY = 0;
		glfwGetWindowPos(this->context_->window(), &_winX, &_winY);

	}

}