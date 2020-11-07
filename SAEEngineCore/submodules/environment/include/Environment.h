#pragma once

struct GLFWwindow;

#include <memory>

namespace sae::engine::core
{

	class GLFWLib
	{
	public:

		bool init();
		bool good() const noexcept;

		void destroy();

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
	


	class Window
	{
	public:

		using pointer = GLFWwindow*;

		constexpr bool good() const noexcept { return this->ptr_ != nullptr; };
		constexpr explicit operator bool() const noexcept { return this->good(); };

		constexpr pointer get() const noexcept { return this->ptr_; };
		void release() noexcept { this->ptr_ = nullptr; };

		friend inline constexpr bool operator==(const Window& _lhs, pointer _rhs) noexcept
		{
			return (_lhs.get() == _rhs);
		};
		friend inline constexpr bool operator!=(const Window& _lhs, pointer _rhs) noexcept
		{
			return (_lhs.get() != _rhs);
		};

		friend inline constexpr bool operator==(pointer _lhs, const Window& _rhs) noexcept
		{
			return (_lhs == _rhs.get());
		};
		friend inline constexpr bool operator!=(pointer _lhs, const Window& _rhs) noexcept
		{
			return (_lhs != _rhs.get());
		};

		void destroy();

		explicit constexpr Window(pointer _ptr) noexcept :
			ptr_{ _ptr }
		{};

		Window(Window&& other) noexcept;
		Window& operator=(Window&& other) noexcept;

		~Window();

		Window(const Window& other) = delete;
		Window& operator=(const Window& other) = delete;

	private:
		pointer ptr_ = nullptr;

	};

	class Window_Ref
	{
	public:

		using pointer = GLFWwindow*;

		constexpr pointer get() const noexcept { return this->ptr_; };
		constexpr operator pointer() const noexcept { return this->get(); };

		constexpr bool good() const noexcept { return this->get() != nullptr; };
		constexpr explicit operator bool() const noexcept { return this->good(); };

		friend inline constexpr bool operator==(const Window_Ref& _lhs, const Window_Ref& _rhs) noexcept
		{
			return (_lhs.get() == _rhs.get());
		};
		friend inline constexpr bool operator!=(const Window_Ref& _lhs, const Window_Ref& _rhs) noexcept
		{
			return (_lhs.get() != _rhs.get());
		};

		friend inline constexpr bool operator==(const Window_Ref& _lhs, pointer _rhs) noexcept
		{
			return (_lhs.get() == _rhs);
		};
		friend inline constexpr bool operator!=(const Window_Ref& _lhs, pointer _rhs) noexcept
		{
			return (_lhs.get() != _rhs);
		};

		friend inline constexpr bool operator==(pointer _lhs, const Window_Ref& _rhs) noexcept
		{
			return (_lhs == _rhs.get());
		};
		friend inline constexpr bool operator!=(pointer _lhs, const Window_Ref& _rhs) noexcept
		{
			return (_lhs != _rhs.get());
		};

		friend inline constexpr bool operator==(const Window_Ref& _lhs, const Window& _rhs) noexcept
		{
			return (_lhs.get() == _rhs.get());
		};
		friend inline constexpr bool operator!=(const Window_Ref& _lhs, const Window& _rhs) noexcept
		{
			return (_lhs.get() != _rhs.get());
		};

		friend inline constexpr bool operator==(const Window& _lhs, const Window_Ref& _rhs) noexcept
		{
			return (_lhs.get() == _rhs.get());
		};
		friend inline constexpr bool operator!=(const Window& _lhs, const Window_Ref& _rhs) noexcept
		{
			return (_lhs.get() != _rhs.get());
		};

		constexpr Window_Ref() noexcept = default;
		constexpr Window_Ref(pointer _window) noexcept :
			ptr_{ _window }
		{};
		Window_Ref& operator=(pointer _window) noexcept;

		constexpr Window_Ref(const Window& _window) noexcept :
			ptr_{ _window.get() }
		{};
		Window_Ref& operator=(const Window& _window) noexcept;
		
	private:
		pointer ptr_ = nullptr;

	};

}