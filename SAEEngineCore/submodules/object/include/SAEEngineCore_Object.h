#pragma once
#ifndef SAE_ENGINE_CORE_OBJECT_H
#define SAE_ENGINE_CORE_OBJECT_H

#include <SAEEngineCore_Event.h>
#include <SAEEngineCore_Artist.h>

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

	template <typename T>
	concept cx_arithmetic = std::is_arithmetic_v<T>;

	/**
	 * @brief Type to wrap up a Z position. Higher value is always closer to the front.
	*/
	class ZLayer
	{
	public:
		using value_type = uint16_t;

	protected:
		template <std::floating_point T>
		constexpr static value_type fpoint_to_zlayer(T _v) noexcept
		{
			return (value_type)(std::clamp(_v, (T)0, std::numeric_limits<T>::max()) * (T)std::numeric_limits<value_type>::max());
		};
		template <std::floating_point T>
		constexpr static T zlayer_to_fpoint(value_type _v) noexcept
		{
			return (T)_v / (T)std::numeric_limits<value_type>::max();
		};

	public:
		/**
		 * @brief Returns the actual internal z layer value
		*/
		constexpr value_type layer() const noexcept { return this->z_; };
		constexpr explicit operator value_type() const noexcept { return this->layer(); };

		/**
		 * @brief Converts the z value to a floating point number. This is always >= 0.0f
		 * @tparam T floating point type
		 * @return The converted floating point value
		*/
		template <std::floating_point T = float_t>
		constexpr T to_fpoint() const noexcept { return zlayer_to_fpoint<T>(this->layer()); };
		template <std::floating_point T = float_t>
		explicit constexpr operator T() const noexcept { return this->to_fpoint<T>(); };

		constexpr auto operator<=>(const ZLayer&) const noexcept = default;

		template <cx_arithmetic T>
		constexpr bool operator<=>(T _rhs) const noexcept
		{
			return this->layer() <=> (value_type)_rhs;
		};
		template <std::floating_point T>
		constexpr bool operator<=>(T _rhs) const noexcept
		{
			return this->layer() <=> this->fpoint_to_zlayer(_rhs);
		};

		constexpr ZLayer operator+(value_type _i) const noexcept { return ZLayer{ (value_type)(this->layer() + _i) }; };
		ZLayer& operator+=(value_type _i) noexcept
		{
			return this->operator=(this->layer() + _i);
		};

		constexpr ZLayer operator-(value_type _i) const noexcept { return ZLayer{ (value_type)(this->layer() - _i) }; };
		ZLayer& operator-=(value_type _i) noexcept
		{
			return this->operator=(this->layer() - _i);
		};

		constexpr ZLayer() = default;

		constexpr ZLayer(value_type _z) noexcept :
			z_{ _z }
		{};
		template <std::floating_point T>
		constexpr ZLayer(T _zfloat) noexcept :
			z_{ fpoint_to_zlayer(_zfloat) }
		{};

		ZLayer& operator=(value_type _z) noexcept
		{
			this->z_ = _z;
			return *this;
		};
		template <std::floating_point T>
		ZLayer& operator=(T _z) noexcept
		{
			this->z_ = fpoint_to_zlayer(_z);
			return *this;
		};

	private:
		value_type z_ = 0;

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
		enum STATE_BITS : uint8_t
		{
			stActive = 0x01,
			stSelected = 0x02,
			stDisplayed = 0x04
		};

	protected:
		friend GFXGroup;
		friend GFXView;
		friend GFXContext;

		virtual void on_state_change(STATE_BITS _b, bool _to);

	private:
		void set_state_bit(STATE_BITS _bit) noexcept;

		void clear_state_bit(STATE_BITS _bit) noexcept;

		bool check_state_bit(STATE_BITS _bit) const noexcept;

		void handle_event_type(const Event::evGrow& _event);

	protected:
		void set_parent(GFXView* _to) noexcept;
		virtual void set_context(GFXContext* _to);

	public:
		GFXView* parent() const noexcept;
		bool has_parent() const noexcept;

		GFXContext* context() const noexcept;

		virtual void handle_event(Event& _event);

		Rect& bounds() noexcept;
		const Rect& bounds() const noexcept;

		ZLayer& zlayer() noexcept;
		const ZLayer& zlayer() const noexcept;

		GrowMode& grow_mode() noexcept;
		const GrowMode& grow_mode() const noexcept;

		virtual void refresh();
		virtual void grow(pixels_t _dw, pixels_t _dh);

		GFXObject(Rect _r);
		
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
		uint8_t state_ = 0x00;
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
		void emplace(GFXObject* _obj);

		void remove(GFXObject* _obj);

		GFXView(GFXContext* _context, Rect _r);

	};

	/**
	 * @brief Represents a graphics context - usually a window. This prevents the need for global state.
	*/
	class GFXContext : public GFXView
	{
	public:

		void handle_event(Event& _event) override;
		virtual void draw();

		void register_artist(const std::string& _name, std::unique_ptr<IArtist> _artist);
		IArtist* find_artist(const std::string& _name);

		GLFWwindow* window() const noexcept;

		GFXContext(GLFWwindow* _window, Rect _r);
		GFXContext(GLFWwindow* _window);

		~GFXContext();

	private:
		GLFWwindow* window_ = nullptr;
		std::vector<std::unique_ptr<IArtist>> artists_{};
		std::unordered_map<std::string, IArtist*> artist_names_{};

	};

}

#endif