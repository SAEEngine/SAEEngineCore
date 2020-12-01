#pragma once
#include <SAELib_DualMap.h>
#include <optional>
#include <string>

namespace sae::engine::core
{

class EVENT_TYPE 
{
public: 
	enum EVENT_TYPE_E
	{
		BLACKBOARD_CHANGE,
		CURSOR_WINDOW_BOUNDS,
		CURSOR_MOVE,
		MOUSE_EVENT,
		KEY_EVENT,
		SCROLL_EVENT,
		TEXT_EVENT,
		USER_EVENT,
		WINDOW_RESIZE,
		WINDOW_CLOSE,
	};

private:
	const static inline ::sae::unordered_dualmap<std::string, EVENT_TYPE_E> STR_ENUM_DMAP
	{
		{ "BLACKBOARD_CHANGE" , EVENT_TYPE_E::BLACKBOARD_CHANGE },
		{ "CURSOR_WINDOW_BOUNDS" , EVENT_TYPE_E::CURSOR_WINDOW_BOUNDS },
		{ "CURSOR_MOVE" , EVENT_TYPE_E::CURSOR_MOVE },
		{ "MOUSE_EVENT" , EVENT_TYPE_E::MOUSE_EVENT },
		{ "KEY_EVENT" , EVENT_TYPE_E::KEY_EVENT },
		{ "SCROLL_EVENT" , EVENT_TYPE_E::SCROLL_EVENT },
		{ "TEXT_EVENT" , EVENT_TYPE_E::TEXT_EVENT },
		{ "USER_EVENT" , EVENT_TYPE_E::USER_EVENT },
		{ "WINDOW_RESIZE" , EVENT_TYPE_E::WINDOW_RESIZE },
		{ "WINDOW_CLOSE" , EVENT_TYPE_E::WINDOW_CLOSE }
	};
	EVENT_TYPE_E val_;

public:
	friend constexpr inline bool operator==(const EVENT_TYPE& _lhs, const EVENT_TYPE& _rhs) noexcept = default;
	friend constexpr inline bool operator!=(const EVENT_TYPE& _lhs, const EVENT_TYPE& _rhs) noexcept = default;
	explicit operator bool() noexcept = delete;

	static inline std::optional<EVENT_TYPE_E> from_string(const std::string& _str)
	{
		std::optional<EVENT_TYPE_E> _out{ std::nullopt };
		if(STR_ENUM_DMAP.contains_left(_str))
		{
			_out = STR_ENUM_DMAP.ltor(_str);
		};
		return _out;
	};
	const std::string& to_string() const
	{
		return STR_ENUM_DMAP.rtol(this->val_);
	};

	constexpr inline operator EVENT_TYPE_E() const noexcept { return this->val_; };
	constexpr EVENT_TYPE(EVENT_TYPE_E _val) noexcept : 
		val_{ _val }
	{};
	EVENT_TYPE& operator=(EVENT_TYPE_E _val) noexcept {
		this->val_ = _val;
		return *this;
	};
};


};
