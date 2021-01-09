#pragma once
#ifndef SAE_ENGINE_CORE_OBJECT_H
#define SAE_ENGINE_CORE_OBJECT_H

#include <SAEEngineCore_Event.h>
#include <SAEEngineCore_Artist.h>
#include <SAEEngineCore_Widget.h>
#include <SAEEngineCore_Environment.h>

#include <cstdint>
#include <vector>
#include <memory>
#include <algorithm>
#include <string>

struct GLFWwindow;

namespace sae::engine::core
{
	/**
	 * @brief Bit flag wrapping type for storing how an object should grow
	*/
	class GrowMode
	{
	public:
		enum GROW_BIT : uint8_t
		{
			gmLeft = 0x1,
			gmRight = 0x2,
			gmTop = 0x04,
			gmBottom = 0x08
		};
		using GROW_BIT_E = GROW_BIT;

		/**
		 * @brief Sets a grow bit to 1
		*/
		GrowMode& set(GROW_BIT_E _bit) noexcept;

		/**
		 * @brief Sets a grow bit to 0
		*/
		GrowMode& clear(GROW_BIT_E _bit) noexcept;

		/**
		 * @brief Sets the specified grow bit to the specified value
		 * @param _bit Bit to set
		 * @param _val Value to set it to
		*/
		GrowMode& set_to(GROW_BIT_E _bit, bool _val) noexcept;

		/**
		 * @brief Returns true if the _bit specified is 1
		*/
		constexpr bool is_set(GROW_BIT_E _bit) const noexcept
		{
			return ((this->bits_ & (uint8_t)_bit) != 0);
		};

		/**
		 * @brief Returns true if the _bit specified is 0
		*/
		constexpr bool is_clear(GROW_BIT_E _bit) const noexcept
		{
			return !this->is_set(_bit);
		};

		friend inline GrowMode& operator<<(GrowMode& _gm, GROW_BIT_E _bit) noexcept
		{
			_gm.set(_bit);
			return _gm;
		};

		constexpr GrowMode() noexcept = default;

	protected:
		constexpr GrowMode(uint8_t _bits) noexcept :
			bits_{ _bits }
		{};

	private:
		uint8_t bits_ = 0x00;

	};

	class GFXObject;
	class GFXGroup;
	class GFXView;
	class GFXContext;

	/**
	 * @brief Basic object type defining an interface for interacting with a single object.
	*/
	class GFXObject
	{
	public:

		/**
		 * @brief Option flag bit masks
		*/
		enum OPTION_BITS : uint8_t
		{
			opDraw = 0x01,
			opInput = 0x02
		};

		/**
		 * @brief Event thrown on option set to true. Content is set to the OPTION_BIT that was changed. Only thrown on change.
		*/
		constexpr static Event::evUser evOptionSet{ 101 };

		/**
		 * @brief Event thrown on option set to false. Content is set to the OPTION_BIT that was changed. Only thrown on change.
		*/
		constexpr static Event::evUser evOptionCleared{ 102 };

		/**
		 * @brief Sets an option bit
		 * @param _op Option bit
		 * @param _to Value to set it to
		*/
		void set_option(OPTION_BITS _op, bool _to);

		/**
		 * @brief Returns the current value of an option
		 * @param _op Option bit
		 * @return Option value
		*/
		bool check_option(OPTION_BITS _op) const noexcept;

	protected:
		friend GFXGroup;
		friend GFXView;
		friend GFXContext;

		/**
		 * @brief Sets the objects pointer pointer
		 * @param _to Parent view pointer
		*/
		void set_parent(GFXView* _to) noexcept;

		/**
		 * @brief Sets the objects context pointer
		 * @param _to Context pointer
		*/
		virtual void set_context(GFXContext* _to);

	public:

		/**
		 * @brief Returns the parent view of this object
		 * @return Parent (GFXView) pointer 
		*/
		GFXView* parent() const noexcept;
	
		/**
		 * @brief Returns true if the object has a parent pointer currently set
		*/
		bool has_parent() const noexcept;

		/**
		 * @brief Returns the context that this object belongs to
		 * @return Context pointer
		*/
		GFXContext* context() const noexcept;

		/**
		 * @brief Called when events are propogated to this object
		 * @param _event Event object
		*/
		virtual void handle_event(Event& _event);

		Rect& bounds() noexcept;
		const Rect& bounds() const noexcept;

		ZLayer& zlayer() noexcept;
		const ZLayer& zlayer() const noexcept;

		GrowMode& grow_mode() noexcept;
		const GrowMode& grow_mode() const noexcept;

		/**
		 * @brief Any changes made since the last call to refresh() should be present or otherwise viewable upon a new call to refresh()
		*/
		virtual void refresh();

		/**
		 * @brief Applies a size change to the object
		 * @param _dw Change in parent width
		 * @param _dh Change in parent height
		*/
		virtual void grow(pixels_t _dw, pixels_t _dh);

		/**
		 * @brief Constructs a GFXObject with the provided bounds
		 * @param _r Bounds
		*/
		GFXObject(Rect _r);
		
		/**
		 * @brief Constructs a GFXObject with default bounds
		*/
		GFXObject();
		
		GFXObject(const GFXObject& other) = delete;
		GFXObject& operator=(const GFXObject& other) = delete;
		GFXObject(GFXObject&& other) noexcept = delete;
		GFXObject& operator=(GFXObject&& other) noexcept = delete;

		virtual ~GFXObject();

	private:
		GFXContext* context_ = nullptr;
		GFXView* parent_ = nullptr;
		GrowMode grow_mode_{};
		uint8_t options_ = opDraw | opInput;
		Rect bounds_{};
		ZLayer z_{};
	};

	/**
	 * @brief Base type for handling a group of objects that act as only one GFXObject. Provides protected container functionality.
	*/
	class GFXGroup : public GFXObject
	{
	protected:
		using value_type = std::shared_ptr<GFXObject>;
		using container_type = std::vector<value_type>;

		void insert_child(value_type _obj);
		void remove_child(GFXObject* _obj);

		container_type& children() noexcept;
		const container_type& children() const noexcept;

		void set_context(GFXContext* _to) override;

	public:
		void refresh() override;
		void grow(pixels_t _dw, pixels_t _dh) override;

		void handle_event(Event& _event) override;

		GFXGroup(Rect _r);

	private:
		container_type children_{};

	};

	/**
	 * @brief Base container type for GFXObjects. Implements public container functionality derived from GFXGroup
	*/
	class GFXView : public GFXGroup
	{
	public:
		using value_type = GFXGroup::value_type;
		using pointer = value_type*;
		using reference = value_type&;
		using const_pointer = const value_type*;
		using const_reference = const value_type&;

		using size_type = typename GFXGroup::container_type::size_type;
		using allocator_type = typename GFXGroup::container_type::allocator_type;

		using iterator = typename GFXGroup::container_type::iterator;
		using const_iterator = typename GFXGroup::container_type::const_iterator;

		iterator begin() noexcept;
		const_iterator begin() const noexcept;
		const_iterator cbegin() const noexcept;

		iterator end() noexcept;
		const_iterator end() const noexcept;
		const_iterator cend() const noexcept;

		size_type child_count() const noexcept;

		reference first_child() noexcept;
		const_reference first_child() const noexcept;

		reference last_child() noexcept;
		const_reference last_child() const noexcept;

		reference child(size_t i);
		const_reference child(size_t i) const;

		void clear() noexcept;

		void insert(value_type _obj);

		/**
		 * @brief Convience function that calls insert() by creating a shared_ptr from the provided pointer. See insert() for info.
		 * @param _obj Owning object pointer
		*/
		void emplace(GFXObject* _obj);

		void remove(GFXObject* _obj);

		
		GFXView(GFXContext* _context, Rect _r);
		GFXView(GFXContext* _context);


	};

	/**
	 * @brief Represents a graphics context - usually a window. This prevents the need for global state.
	*/
	class GFXContext : public GFXView
	{
	public:
		void handle_event(Event& _event) override;
		
		virtual void draw();
		virtual void idle();

		void update();

		void register_artist(std::unique_ptr<Artist> _artist);
		Artist* find_artist(const SpecID _id);



		Window& window() noexcept;
		const Window& window() const noexcept;

		GFXContext(Window _window, Rect _r);
		GFXContext(Window _window);

		~GFXContext();

	private:
		Window window_;
		std::unordered_map<SpecID, std::unique_ptr<Artist>> artists_{};
		std::vector<Artist*> artist_vec_{};

	};


}

#endif