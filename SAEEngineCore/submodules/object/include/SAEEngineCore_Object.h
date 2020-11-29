#pragma once
#ifndef SAE_ENGINE_CORE_OBJECT_H
#define SAE_ENGINE_CORE_OBJECT_H

#include <SAEEngineCore_Environment.h>
#include <SAEEngineCore_Event.h>

#include <glm/gtc/matrix_transform.hpp>

#include <SAELib_Either.h>

#include <cstdint>
#include <vector>
#include <memory>
#include <algorithm>
#include <string>
#include <variant>

namespace sae::engine::core
{

	/**
	 * @brief POD type for resprenting a point on the screen (given in pixels; origin is upper left of window)
	*/
	struct ScreenPoint
	{
		/**
		 * @brief Integer type for holding a value in pixels
		*/
		using pixels_t = int16_t;
		
		pixels_t x = 0;
		pixels_t y = 0;
	};

	/**
	 * @brief POD type for representing a region on the screen in pixels
	*/
	struct UIRect
	{
		using pixels_t = typename ScreenPoint::pixels_t;

		// The upper left point of the region
		ScreenPoint a;

		// The lower right point of the region
		ScreenPoint b;

		/**
		 * @brief Returns the width of the region
		 * @return value in pixels
		*/
		constexpr pixels_t width() const noexcept { return this->b.x - this->a.x; };

		/**
		 * @brief Returns the height of the region
		 * @return value in pixels
		*/
		constexpr pixels_t height() const noexcept { return this->b.y - this->a.y; };

		/**
		 * @brief Shifts the region defined by this rect by _dx horizontally, and _dy and vertically
		 * @param _dx Change in x position
		 * @param _dy Change in y position
		*/
		UIRect& shift(pixels_t _dx, pixels_t _dy) noexcept;

		/**
		 * @brief Changes the width of the rectangle by _dw * 2 and the height by _dh * 2. The center of the rectangle doesn't move.
		 * @param _dw Change in width (actual change would be _dw * 2)
		 * @param _dh Change in height (actual change would be _dw * 2)
		*/
		UIRect& grow(pixels_t _dw, pixels_t _dh) noexcept;

		/**
		 * @brief Checks if a point on the screen intersects this rectangle
		 * @param _p Point
		 * @return true if intersecting, false if not
		*/
		constexpr bool intersects(ScreenPoint _p) const noexcept
		{
			return (this->a.x <= _p.x && _p.x < this->b.x && this->a.y <= _p.y && _p.y < this->b.y);
		};

		/**
		 * @brief Creates a 4x4 matrix using the region as the inputs to glm::ortho
		*/
		glm::mat4 ortho() const noexcept;

	};

	/**
	 * @brief Represents an R color with 8 bits for each color value (8 for the full color)
	*/
	union ColorR_8
	{
		struct
		{
			uint8_t r;
		};
		uint8_t col[1]{};
	};

	/**
	 * @brief Represents an RGB color with 8 bits for each color value (24 for the full color)
	*/
	union ColorRGB_8
	{
		struct
		{
			uint8_t r;
			uint8_t g;
			uint8_t b;
		};
		uint8_t col[3]{};
	};

	/**
	 * @brief Represents an RGBA color with 8 bits for each color value (32 for the full color)
	*/
	union ColorRGBA_8
	{
		struct
		{
			uint8_t r;
			uint8_t g;
			uint8_t b;
			uint8_t a;
		};
		uint8_t col[4]{};
	};


	class Palette
	{
	public:
		using color_type = ColorRGBA_8;
		using value_type = std::variant<uint16_t, color_type>;

		auto& at(size_t i) { return this->entries_.at(i); };
		const auto& at(size_t i) const { return this->entries_.at(i); };

		Palette() = default;
		Palette(std::initializer_list<value_type> _iList) :
			entries_{ _iList }
		{};

	private:
		std::vector<value_type> entries_{};

	};






#if false
	/**
	 * @brief Transforms a rectangle to be relative to the provided parent rectangle
	 * @param _parent Parent rectangle
	 * @param _child Child rectangle
	 * @return The transformed version of _child, now relative to _parent
	*/
	UIRect& relative(const UIRect& _parent, UIRect& _child) noexcept;

	/**
	 * @brief Transforms a range of rectangles to be relative to the provided parent rectangle
	 * @tparam _FwdIter Forward iterator type
	 * @param _parent Parent rectangle
	 * @param _firstChild Iterator to first child rectangle
	 * @param _lastChild Iterator to one past the final child rectangle
	 * @return Iterator to one past the final rectangle that was transformed
	*/
	template <typename _FwdIter>
	static inline _FwdIter relative(const UIRect& _parent, const _FwdIter _firstChild, const _FwdIter _lastChild)
	{
		_FwdIter _it = _firstChild;
		for (_it; _it != _lastChild; ++_it)
		{
			relative(_parent, *_it);
		};
		return _it;
	};
#endif
	

	class GFXContext;

	class GFXBase
	{
	public:
		GFXContext* context() const noexcept;

	protected:
		virtual ~GFXBase() = default;

	private:
		void set_context(GFXContext* _context) noexcept;
		friend GFXContext;

		GFXContext* context_ = nullptr;

	};

	class UIGroup;

	class GFXContext
	{
	public:
		virtual void push_event(const Event& _ev) = 0;

	protected:
		friend UIGroup;

		void add_to_context(GFXBase* _obj)
		{
			_obj->set_context(this);
		};
		virtual ~GFXContext() = default;

	};

	/**
	 * @brief Base type for representing drawable UIObjects
	*/
	class UIObject : public GFXBase
	{
	public:

		/**
		 * @brief Sets the bounds for the object
		 * @param _r UIRect defining the region
		*/
		void set_bounds(UIRect _r) noexcept;

		/**
		 * @brief Returns the bounds of the object
		 * @return UIRect defining the region 
		*/
		const UIRect& bounds() const noexcept;

		/**
		 * @brief Draws ONLY this object
		*/
		virtual void draw_self();

		/**
		 * @brief Draws the object and any dependents
		*/
		virtual void draw();

	private:
		static inline Palette EMPTY_PALLETE{};

	public:

		/**
		 * @brief Returns the palette used by the object
		*/
		virtual Palette& get_palette() const;
		
		/**
		 * @brief Enumerator to better describe the return value from the handle_event() function
		*/
		enum HANDLE_EVENT_RETURN : bool
		{
			IGNORED = false,
			HANDLED = true
		};

		/**
		 * @brief Handles a standard event
		 * @return IGNORED if the event was ignored, HANDLED if it was handled
		*/
		virtual HANDLE_EVENT_RETURN handle_event(const Event& _event);

		UIObject(UIRect _r);
		virtual ~UIObject() = default;

	private:

		GFXContext* context_ = nullptr;
		UIRect bounds_{};
		float_t z_ = 0.0f;

	};

	/**
	 * @brief Base type for a group of UIObjects
	*/
	class UIGroup
	{
	private:
		using value_type = std::shared_ptr<UIObject>;
		using ContainerT = std::vector<value_type>;
		
		/**
		 * @brief Encapsulation function to return the child objects container
		 * @return NON-Const value reference to the container
		*/
		ContainerT& get_container() noexcept;

		/**
		 * @brief Encapsulation function to return the child objects container
		 * @return Const value reference to the container
		*/
		const ContainerT& get_container() const noexcept;

	protected:
		void add_to_graphics_context(GFXContext* _context, GFXBase* _obj)
		{
			_context->add_to_context(_obj);
		};

	public:

		/**
		 * @brief Adds a child to the group
		 * @param _obj Shared pointer to new child
		*/
		virtual void insert(std::shared_ptr<UIObject> _obj);
		
		/**
		 * @brief Removes a child from the group
		 * @param _obj Pointer to the object
		*/
		virtual void remove(UIObject* _obj);
		
		/**
		 * @brief Returns the number of children in the group
		*/
		size_t child_count() const noexcept;

		/**
		 * @brief Returns a reference to the children container
		*/
		auto& children() noexcept { return this->get_container(); };

		/**
		 * @brief Returns a const-reference to the children container
		*/
		const auto& children() const noexcept { return this->get_container(); };

	private:
		ContainerT objs_{};

	};



	class UIView : public UIObject, public UIGroup
	{
	public:

		/**
		 * @brief Draws this object and any child objects
		*/
		void draw() override
		{
			this->draw_self();
			for (auto& o : this->children())
			{
				o->draw();
			};
		}

		void insert(std::shared_ptr<UIObject> _obj) override;

		void remove(UIObject* _obj) override;



		/**
		 * @brief Passes the event down to children until one of them handles it
		 * @param _ev Event to handle
		 * @return IGNORED if the event was ignored, HANDLED if handled
		*/
		HANDLE_EVENT_RETURN handle_event(const Event& _ev) override;

		UIView(UIRect _r) :
			UIObject{ _r }
		{};

	};

	class UIButton : public UIView
	{
	private:
		EventResponse on_mb1_;
		EventResponse on_mb2_;

		bool is_down_ = false;
		int down_button_ = 0;

		void handle_response(const EventResponse& _ev, const Event& _fromEvent);

	protected:
		HANDLE_EVENT_RETURN handle_mouse_event(const Event::evMouse& _evmouse);

	public:

		HANDLE_EVENT_RETURN handle_event(const Event& _event) override;

		UIButton(UIRect _r, const EventResponse& _onMouse1, const EventResponse& _onMouse2);

	};

	class UIPushButton : public UIView
	{
	private:
		static inline Palette DEFAULT_PALETTE
		{ 
			{ ColorRGBA_8{ 255, 255, 255, 255 } },	// resting state
			{ ColorRGBA_8{ 255, 255, 255, 255 } },	// pushed state
			{ ColorRGBA_8{ 255, 255, 255, 255 } }	// hovered state
		};

		enum BUTTON_STATE
		{
			RESTING,
			PUSHED,
			HOVERED
		};

		BUTTON_STATE state_ = BUTTON_STATE::RESTING;

		EventResponse on_push_;
		EventResponse on_release_;
		EventResponse on_mouse_enter_;
		EventResponse on_mouse_leave_;

		bool is_hovered_ = true;

		bool is_down_ = false;
		int down_button_ = 0;

		void handle_response(const EventResponse& _ev, const Event& _fromEvent);

	protected:
		HANDLE_EVENT_RETURN handle_mouse_event(const Event::evMouse& _evmouse);
		HANDLE_EVENT_RETURN handle_cursor_event(const Event::evCursorMove& _event);



	public:
		Palette& get_palette() const override { return this->DEFAULT_PALETTE; };

		HANDLE_EVENT_RETURN handle_event(const Event& _event) override;

		UIPushButton(	UIRect _r, const EventResponse& _onPush, const EventResponse& _onRelease, 
						const EventResponse& _onMouseEnter, const EventResponse& _onMouseLeave	);

	};

	class UIToggleButton : public UIView
	{
	private:
		static inline Palette DEFAULT_PALETTE
		{
			{ ColorRGBA_8{ 140, 30, 30, 255 } },	// resting-inactive state
			{ ColorRGBA_8{ 30, 140, 30, 255 } },	// resting-active state
			{ ColorRGBA_8{ 20, 80, 20, 255 } },		// pushed-active state
			{ ColorRGBA_8{ 80, 20, 20, 255 } },	// pushed-inactive state
			{ ColorRGBA_8{ 160, 80, 80, 255 } },	// hovered-inactive state
			{ ColorRGBA_8{ 80, 160, 80, 255 } }	// hovered-active state
		};

		enum BUTTON_STATE
		{
			RESTING_INACTIVE,
			RESTING_ACTIVE,
			PUSHED_INACTIVE,
			PUSHED_ACTIVE,
			HOVERED_INACTIVE,
			HOVERED_ACTIVE,
		};

		BUTTON_STATE state_ = BUTTON_STATE::RESTING_INACTIVE;

		bool is_hovered_ = false;
		bool is_down_ = false;
		bool is_active_ = false;

		EventResponse on_toggle_on_;
		EventResponse on_toggle_off_;

		void handle_response(const EventResponse& _ev, const Event& _fromEvent);

	protected:
		HANDLE_EVENT_RETURN handle_mouse_event(const Event::evMouse& _evmouse);
		HANDLE_EVENT_RETURN handle_cursor_event(const Event::evCursorMove& _event);

	public:
		Palette& get_palette() const override { return this->DEFAULT_PALETTE; };

		BUTTON_STATE get_state() const noexcept { return this->state_; };

		HANDLE_EVENT_RETURN handle_event(const Event& _event) override;

		UIToggleButton(UIRect _r, const EventResponse& _onToggleOn, const EventResponse& _onToggleOff);

	};
	






	class GFXWindow : public UIView, public Window, public GFXContext
	{
	private:
		static void glfw_mouse_button_callback(GLFWwindow* _window, int _button, int _action, int _mods);
		static void glfw_cursor_move_callback(GLFWwindow* _window, double _x, double _y);

	public:

		/**
		 * @brief Draws the window's child objects and swaps the window buffers
		*/
		void draw() override;

		HANDLE_EVENT_RETURN handle_event(const Event& _event) override;

		void push_event(const Event& _event) override { this->handle_event(_event); };

		/**
		 * @brief Opens a new window of size (_width, _height) with title _title
		*/
		GFXWindow(ScreenPoint::pixels_t _width, ScreenPoint::pixels_t _height, const std::string& _title);

	private:

	};






}

#endif