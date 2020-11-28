#pragma once
#ifndef SAE_ENGINE_CORE_OBJECT_H
#define SAE_ENGINE_CORE_OBJECT_H

#include <SAEEngineCore_Environment.h>

#include <glm/gtc/matrix_transform.hpp>

#include <cstdint>
#include <vector>
#include <memory>
#include <algorithm>
#include <string>

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
		 * @brief Creates a 4x4 matrix using the region as the inputs to glm::ortho
		*/
		glm::mat4 ortho() const noexcept;

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
	

	/**
	 * @brief Base type for representing drawable UIObjects
	*/
	class UIObject
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

		
		virtual void handle_event() {};


		UIObject(UIRect _r);
		virtual ~UIObject() = default;

	private:
		UIRect bounds_{};

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

	public:

		/**
		 * @brief Adds a child to the group
		 * @param _obj Shared pointer to new child
		*/
		void insert(std::shared_ptr<UIObject> _obj);
		
		/**
		 * @brief Removes a child from the group
		 * @param _obj Pointer to the object
		*/
		void remove(UIObject* _obj);
		
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


		UIView(UIRect _r) :
			UIObject{ _r }
		{};

	};







	class GFXWindow : public UIView, public Window
	{
	public:

		/**
		 * @brief Draws the window's child objects and swaps the window buffers
		*/
		void draw() override;

		/**
		 * @brief Opens a new window of size (_width, _height) with title _title
		*/
		GFXWindow(ScreenPoint::pixels_t _width, ScreenPoint::pixels_t _height, const std::string& _title);

	private:

	};






}

#endif