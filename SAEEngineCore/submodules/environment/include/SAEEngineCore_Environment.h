#pragma once
#ifndef SAE_ENGINE_CORE_ENVIRONMENT_H
#define SAE_ENGINE_CORE_ENVIRONMENT_H

#include <GLFW/glfw3.h>

#include <memory>

namespace sae::engine::core
{

	class GLFWLib
	{
	public:

		bool init();
		bool good() const noexcept;

		void destroy();
		void poll_events();

		GLFWLib();
		~GLFWLib();

		GLFWLib(const GLFWLib& other) = delete;
		GLFWLib& operator=(const GLFWLib& other) = delete;

		GLFWLib(GLFWLib&& other) noexcept = delete;
		GLFWLib& operator=(GLFWLib&& other) noexcept = delete;

	private:
		bool good_ = false;
	};

	class GLFWLib_Ref
	{
	public:
		using value_type = GLFWLib;
		using pointer = value_type*;
		using reference = value_type&;
		using const_reference = const value_type&;

		bool good() const noexcept;
		explicit operator bool() const noexcept;

		pointer get() const noexcept;
		pointer operator->() const noexcept;

		reference operator*();
		const_reference operator*() const;

		GLFWLib_Ref() = default;

		GLFWLib_Ref(std::shared_ptr<value_type> _lib);
		GLFWLib_Ref& operator=(std::shared_ptr<value_type> _lib);

	private:
		std::shared_ptr<value_type> lib_{};
	};
	

	/**
	 * @brief Wraps a GLFWwindow and owns its memory. Destroys window on destructor
	*/
	class Window
	{
	public:

		using pointer = GLFWwindow*;

		bool good() const noexcept { return this->ptr_ != nullptr; };
		
		bool is_open() const;

		pointer get() const noexcept { return this->ptr_; };
		void release() noexcept { this->ptr_ = nullptr; };

		bool is_current() const;
		void make_current();

		void swap_buffers();

		operator pointer() const noexcept { return this->get(); };

		friend inline bool operator==(const Window& _lhs, pointer _rhs) noexcept
		{
			return (_lhs.get() == _rhs);
		};
		friend inline bool operator!=(const Window& _lhs, pointer _rhs) noexcept
		{
			return (_lhs.get() != _rhs);
		};

		friend inline bool operator==(pointer _lhs, const Window& _rhs) noexcept
		{
			return (_lhs == _rhs.get());
		};
		friend inline bool operator!=(pointer _lhs, const Window& _rhs) noexcept
		{
			return (_lhs != _rhs.get());
		};

		void destroy();

		/**
		 * @brief Creates a new window
		 * @param _ptr GLFWwindow pointer
		*/
		explicit Window(pointer _ptr);

		Window(Window&& other) noexcept;
		Window& operator=(Window&& other) noexcept;

		Window(const Window& other) = delete;
		Window& operator=(const Window& other) = delete;

		~Window();

	private:
		pointer ptr_ = nullptr;

	};

	class WindowRef
	{
	public:

		using pointer = GLFWwindow*;

		pointer get() const noexcept { return this->ptr_; };
		operator pointer() const noexcept { return this->get(); };

		bool good() const noexcept { return this->get() != nullptr; };
		explicit operator bool() const noexcept { return this->good(); };

		friend inline bool operator==(const WindowRef& _lhs, const WindowRef& _rhs) noexcept
		{
			return (_lhs.get() == _rhs.get());
		};
		friend inline bool operator!=(const WindowRef& _lhs, const WindowRef& _rhs) noexcept
		{
			return (_lhs.get() != _rhs.get());
		};

		friend inline bool operator==(const WindowRef& _lhs, pointer _rhs) noexcept
		{
			return (_lhs.get() == _rhs);
		};
		friend inline bool operator!=(const WindowRef& _lhs, pointer _rhs) noexcept
		{
			return (_lhs.get() != _rhs);
		};

		friend inline bool operator==(pointer _lhs, const WindowRef& _rhs) noexcept
		{
			return (_lhs == _rhs.get());
		};
		friend inline bool operator!=(pointer _lhs, const WindowRef& _rhs) noexcept
		{
			return (_lhs != _rhs.get());
		};

		friend inline bool operator==(const WindowRef& _lhs, const Window& _rhs) noexcept
		{
			return (_lhs.get() == _rhs.get());
		};
		friend inline bool operator!=(const WindowRef& _lhs, const Window& _rhs) noexcept
		{
			return (_lhs.get() != _rhs.get());
		};

		friend inline bool operator==(const Window& _lhs, const WindowRef& _rhs) noexcept
		{
			return (_lhs.get() == _rhs.get());
		};
		friend inline bool operator!=(const Window& _lhs, const WindowRef& _rhs) noexcept
		{
			return (_lhs.get() != _rhs.get());
		};

		WindowRef() noexcept = default;
		WindowRef(pointer _window) noexcept :
			ptr_{ _window }
		{};
		WindowRef& operator=(pointer _window) noexcept;

		WindowRef(const Window& _window) noexcept :
			ptr_{ _window.get() }
		{};
		WindowRef& operator=(const Window& _window) noexcept;
		
	private:
		pointer ptr_ = nullptr;

	};

}

#endif