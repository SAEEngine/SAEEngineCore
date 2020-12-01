#pragma once
#ifndef SAE_ENGINE_CORE_EVENT_H
#define SAE_ENGINE_CORE_EVENT_H

#include "SAEEngineCore_EventType.h"

#include <SAELib_Functor.h>

#include <variant>

namespace sae::engine::core
{

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

		};
		using evScroll = EventType<EVENT_TYPE_E::SCROLL_EVENT>;

		template <>
		struct EventType<EVENT_TYPE_E::USER_EVENT>
		{
			int ev = 0;
		};
		using evUser = EventType<EVENT_TYPE_E::USER_EVENT>;

		template <>
		struct EventType<EVENT_TYPE_E::WINDOW_RESIZE>
		{
			int16_t width = 0;
			int16_t height = 0;
		};
		using evWindowResize = EventType<EVENT_TYPE_E::WINDOW_RESIZE>;

		template <>
		struct EventType<EVENT_TYPE_E::WINDOW_CLOSE>
		{

		};
		using evWindowClose = EventType<EVENT_TYPE_E::WINDOW_CLOSE>;


	protected:

		using variant_type = std::variant<
			EventType<EVENT_TYPE_E::BLACKBOARD_CHANGE>,
			EventType<EVENT_TYPE_E::CURSOR_WINDOW_BOUNDS>,
			EventType<EVENT_TYPE_E::CURSOR_MOVE>,
			EventType<EVENT_TYPE_E::MOUSE_EVENT>,
			EventType<EVENT_TYPE_E::KEY_EVENT>,
			EventType<EVENT_TYPE_E::SCROLL_EVENT>,
			EventType<EVENT_TYPE_E::USER_EVENT>,
			EventType<EVENT_TYPE_E::WINDOW_RESIZE>,
			EventType<EVENT_TYPE_E::WINDOW_CLOSE>
		>;

		variant_type& get_variant() noexcept { return this->vt_; };
		const variant_type& get_variant() const noexcept { return this->vt_; };

	public:
		EVENT_TYPE index() const noexcept { return EVENT_TYPE{ (EVENT_TYPE_E)this->get_variant().index() }; };

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

		template <EVENT_TYPE_E Type>
		Event(const EventType<Type>& _evnt) : 
			vt_{ _evnt }
		{};
		template <EVENT_TYPE_E Type>
		Event& operator=(const EventType<Type>& _evnt)
		{
			this->get_variant() = _evnt;
			return *this;
		};

		template <EVENT_TYPE_E Type>
		Event(EventType<Type>&& _evnt) :
			vt_{ std::move(_evnt) }
		{};
		template <EVENT_TYPE_E Type>
		Event& operator=(EventType<Type>&& _evnt)
		{
			this->get_variant() = std::move(_evnt);
			return *this;
		};

	private:
		variant_type vt_{};

	};



	using HandleEventCallback = functor<bool(const Event&)>;

	class EventResponse
	{
	public:
		using deferred_type = Event::evUser;
		using immediate_type = HandleEventCallback;

	private:
		using variant_type = std::variant<deferred_type, immediate_type>;
		variant_type vt_;

		using index_type = decltype(vt_.index());

	public:
		enum RESPONSE_TYPE : index_type
		{
			DEFERRED = 0,
			IMMEDIATE = 1
		};

		RESPONSE_TYPE type() const noexcept 
		{ 
			return (RESPONSE_TYPE)this->vt_.index();
		};

		template <RESPONSE_TYPE Type>
		auto& get() 
		{ 
			return std::get<(index_type)Type>(this->vt_);
		};
		template <RESPONSE_TYPE Type>
		const auto& get() const 
		{ 
			return std::get<(index_type)Type>(this->vt_);
		};

		EventResponse(deferred_type _df) :
			vt_{ _df }
		{};
		EventResponse(const immediate_type& _im) :
			vt_{ _im }
		{};

		EventResponse& operator=(deferred_type _df)
		{
			this->vt_ = _df;
			return *this;
		};
		EventResponse& operator=(const immediate_type& _im)
		{
			this->vt_ = _im;
			return *this;
		};

	};
	


}


#endif