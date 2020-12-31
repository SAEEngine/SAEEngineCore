#pragma once

struct GLFWwindow;

namespace sae::engine::core
{
	class GFXContext;

	class WindowEventAdapter
	{
	private:
		static void glfw_mouse_button_callback(GLFWwindow* _window, int _button, int _action, int _mods);

		static void glfw_scroll_callback(GLFWwindow* _window, double _x, double _y);

		static void glfw_key_callback(GLFWwindow* _window, int _key, int _scancode, int _action, int _modss);
		static void glfw_text_callback(GLFWwindow* _window, unsigned _codepoint);

		static void glfw_cursor_move_callback(GLFWwindow* _window, double _x, double _y);
		static void glfw_cursor_enter_callback(GLFWwindow* _window, int _entered);

		static void glfw_framebuffer_resize_callback(GLFWwindow* _window, int _width, int _height);
		static void glfw_window_close_callback(GLFWwindow* _window);

		static void glfw_window_refresh_callback(GLFWwindow* _window);

	public:

		WindowEventAdapter(GFXContext* _context);

	private:
		GFXContext* context_ = nullptr;
	};

}