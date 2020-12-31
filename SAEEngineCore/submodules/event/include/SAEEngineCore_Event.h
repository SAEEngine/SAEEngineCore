#pragma once
#ifndef SAE_ENGINE_CORE_EVENT_H
#define SAE_ENGINE_CORE_EVENT_H

#include <SAEEngineCore_EventType.h>
#include <SAEEngineCore_Widget.h>

#include <SAELib_Functor.h>

#include <variant>

namespace sae::engine::core
{

	class UIObject;

	/**
	 * @brief Variant pattern type for storing any type of Event. Uses a variant to implement polymorphism
	*/
	class Event
	{
	public:
		using EVENT_TYPE = ::sae::engine::core::EVENT_TYPE;
		using EVENT_TYPE_E = EVENT_TYPE::EVENT_TYPE_E;

	public:	
		template <EVENT_TYPE_E Type>
		struct EventType;

		template <>
		struct EventType<EVENT_TYPE_E::NULL_EVENT>
		{};
		using evNull = EventType<EVENT_TYPE_E::NULL_EVENT>;

		template <>
		struct EventType<EVENT_TYPE_E::BLACKBOARD_CHANGE>
		{
			std::string key;
		};
		using evBlackboardChange = EventType<EVENT_TYPE_E::BLACKBOARD_CHANGE>;

		template <>
		struct EventType<EVENT_TYPE_E::CURSOR_WINDOW_BOUNDS>
		{
			enum CURSOR_ACTION
			{
				ENTER,
				EXIT
			};
			CURSOR_ACTION action;
		};
		using evCursorWindowBounds = EventType<EVENT_TYPE_E::CURSOR_WINDOW_BOUNDS>;

		template <>
		struct EventType<EVENT_TYPE_E::CURSOR_MOVE>
		{
			int16_t cursor_x = 0;
			int16_t cursor_y = 0;
		};
		using evCursorMove = EventType<EVENT_TYPE_E::CURSOR_MOVE>;

		template <>
		struct EventType<EVENT_TYPE_E::MOUSE_EVENT>
		{
			int button = 0;
			int action = 0;
			int mods = 0;
			int16_t cursor_x = 0;
			int16_t cursor_y = 0;
		};
		using evMouse = EventType<EVENT_TYPE_E::MOUSE_EVENT>;

		template <>
		struct EventType<EVENT_TYPE_E::KEY_EVENT>
		{
			int key = 0;
			int scancode = 0;
			int action = 0;
			int mods = 0;
		};
		using evKey = EventType<EVENT_TYPE_E::KEY_EVENT>;

		template <>
		struct EventType<EVENT_TYPE_E::SCROLL_EVENT>
		{
			double x = 0.0;
			double y = 0.0;
		};
		using evScroll = EventType<EVENT_TYPE_E::SCROLL_EVENT>;

		template <>
		struct EventType<EVENT_TYPE_E::TEXT_EVENT>
		{
			unsigned codepoint = 0;
		};
		using evText = EventType<EVENT_TYPE_E::TEXT_EVENT>;

		template <>
		struct EventType<EVENT_TYPE_E::USER_EVENT>
		{
			constexpr auto operator<=>(const EventType<EVENT_TYPE_E::USER_EVENT>& other) const noexcept
			{
				return this->ev <=> other.ev;
			};
			constexpr operator int() const noexcept { return this->ev; };
			int ev = 0;

			int content = 0;

		};
		using evUser = EventType<EVENT_TYPE_E::USER_EVENT>;

		template <>
		struct EventType<EVENT_TYPE_E::GROW_EVENT>
		{
			pixels_t dw = 0_px;
			pixels_t dh = 0_px;
		};
		using evGrow = EventType<EVENT_TYPE_E::GROW_EVENT>;

		template <>
		struct EventType<EVENT_TYPE_E::REFRESH_EVENT>
		{

		};
		using evRefresh = EventType<EVENT_TYPE_E::REFRESH_EVENT>;

		template <>
		struct EventType<EVENT_TYPE_E::WINDOW_CLOSE>
		{

		};
		using evWindowClose = EventType<EVENT_TYPE_E::WINDOW_CLOSE>;

	protected:
		using variant_type = std::variant<
			EventType<EVENT_TYPE_E::NULL_EVENT>,
			EventType<EVENT_TYPE_E::BLACKBOARD_CHANGE>,
			EventType<EVENT_TYPE_E::CURSOR_WINDOW_BOUNDS>,
			EventType<EVENT_TYPE_E::CURSOR_MOVE>,
			EventType<EVENT_TYPE_E::MOUSE_EVENT>,
			EventType<EVENT_TYPE_E::KEY_EVENT>,
			EventType<EVENT_TYPE_E::SCROLL_EVENT>,
			EventType<EVENT_TYPE_E::TEXT_EVENT>,
			EventType<EVENT_TYPE_E::USER_EVENT>,
			EventType<EVENT_TYPE_E::GROW_EVENT>,
			EventType<EVENT_TYPE_E::REFRESH_EVENT>,
			EventType<EVENT_TYPE_E::WINDOW_CLOSE>
		>;

		variant_type& get_variant() noexcept { return this->vt_; };
		const variant_type& get_variant() const noexcept { return this->vt_; };

	public:
		EVENT_TYPE index() const noexcept { return EVENT_TYPE{ (EVENT_TYPE_E)this->get_variant().index() }; };

		bool has_event() const noexcept
		{
			return this->index() != EVENT_TYPE{ EVENT_TYPE::NULL_EVENT };
		};
		explicit operator bool() const noexcept { return this->has_event(); };

		bool is_broadcast() const noexcept { return this->broadcast_; };

		void clear()
		{
			if (!this->is_broadcast())
			{
				this->get_variant() = evNull{};
			};
		};

		template <EVENT_TYPE_E Type>
		auto& get() 
		{
			return std::get<EventType<Type>>(this->get_variant());
		};
		template <EVENT_TYPE_E Type>
		const auto& get() const
		{
			return std::get<EventType<Type>>(this->get_variant());
		};


		Event() :
			vt_{ evNull{} }, broadcast_{ false }
		{};

		template <EVENT_TYPE_E Type>
		Event(const EventType<Type>& _evnt, bool _broadcast = false) :
			vt_{ _evnt }, broadcast_{ _broadcast }
		{};
		template <EVENT_TYPE_E Type>
		Event& operator=(const EventType<Type>& _evnt)
		{
			this->get_variant() = _evnt;
			return *this;
		};

		template <EVENT_TYPE_E Type>
		Event(EventType<Type>&& _evnt, bool _broadcast = false) :
			vt_{ std::move(_evnt) }, broadcast_{ _broadcast }
		{};
		template <EVENT_TYPE_E Type>
		Event& operator=(EventType<Type>&& _evnt)
		{
			this->get_variant() = std::move(_evnt);
			return *this;
		};

	private:
		bool broadcast_ = false;
		variant_type vt_{};


	};

	using EventSet = std::vector<Event>;



	using HandleEventCallback = functor<bool(const Event&)>;

	class EventResponse
	{
	public:
		UIObject* give_to() const noexcept { return this->give_to_; };

		Event& get() noexcept { return this->ev_; };
		const Event& get() const noexcept { return this->ev_; };

		void set_give_to(UIObject* _obj) noexcept { this->give_to_ = _obj; };

		EventResponse(const Event& _ev, UIObject* _giveTo) :
			ev_{ _ev }, give_to_{ _giveTo }
		{};
		EventResponse(Event&& _ev, UIObject* _giveTo) :
			ev_{ _ev }, give_to_{ _giveTo }
		{};

		EventResponse(const Event& _ev) :
			EventResponse{ _ev, nullptr }
		{};
		EventResponse(Event&& _ev) :
			EventResponse{ std::move(_ev), nullptr }
		{};

	private:
		UIObject* give_to_ = nullptr;
		Event ev_;

	};


}


#endif