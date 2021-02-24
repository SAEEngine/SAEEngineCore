#include "SAEEngineCore_UI.h"

#include <cassert>

namespace sae::engine::core
{

	void UIList::reposition_horizontal(pixels_t _eachWidth)
	{
		auto _x = this->bounds().left();
		if (this->direction() == DIRECTION::NEGATIVE)
		{
			_x = this->bounds().right() - _eachWidth;
		};

		if (this->is_fixed_size())
		{
			assert(this->fixed_size().has_value());
			_eachWidth = this->fixed_size()->width();
		};

		auto _w = this->bounds().width();
		auto _inc = _eachWidth + this->margin();
		if (this->direction() == DIRECTION::NEGATIVE)
		{
			_inc = -_inc;
		};

		for (auto& o : this->children())
		{
			auto& _b = o->bounds();
			_b.left() = _x;
			_b.right() = _b.left() + _eachWidth;
			_b.top() = this->bounds().top();
			_b.bottom() = this->bounds().bottom();
			o->zlayer() = this->zlayer();
			_x += _inc;
		};
	};
	void UIList::reposition_horizontal()
	{
		if (this->child_count() != 0)
		{
			auto _eachWidth = (this->bounds().width() - ((this->child_count() - 1) * this->margin_)) / (this->child_count());
			this->reposition_horizontal(_eachWidth);
		};
	};

	void UIList::reposition_vertical(pixels_t _eachHeight)
	{
		auto _y = this->bounds().top();
		if (this->direction() == DIRECTION::NEGATIVE)
		{
			_y = this->bounds().bottom() - _eachHeight;
		};

		if (this->is_fixed_size())
		{
			assert(this->fixed_size().has_value());
			_eachHeight = this->fixed_size()->height();
		};

		auto _h = this->bounds().height();
		auto _inc = _eachHeight + this->margin();
		if (this->direction() == DIRECTION::NEGATIVE)
		{
			_inc = -_inc;
		};

		for (auto& o : this->children())
		{
			auto& _b = o->bounds();
			_b.left() = this->bounds().left();
			_b.right() = this->bounds().right();
			_b.top() = _y;
			_b.bottom() = _y + _eachHeight;
			o->zlayer() = this->zlayer();
			_y += _inc;
		};
	};
	void UIList::reposition_vertical()
	{
		if (this->child_count() != 0)
		{
			auto _eachHeight = (this->bounds().height() - ((this->child_count() - 1) * this->margin_)) / (this->child_count());
			this->reposition_vertical(_eachHeight);
		};
	};

	pixels_t& UIList::margin() noexcept
	{
		return this->margin_;
	};
	const pixels_t& UIList::margin() const noexcept
	{
		return this->margin_;
	};

	void  UIList::set_axis(AXIS _axis) noexcept
	{
		this->axis_ = _axis;
	};
	UIList::AXIS UIList::axis() const noexcept
	{
		return this->axis_;
	};

	void UIList::set_direction(DIRECTION _dir) noexcept
	{
		this->dir_ = _dir;
	};
	UIList::DIRECTION UIList::direction() const noexcept
	{
		return this->dir_;
	};

	void UIList::refresh()
	{
		switch (this->axis())
		{
		case AXIS::HORIZONTAL:
			this->reposition_horizontal();
			break;
		case AXIS::VERTICAL:
			this->reposition_vertical();
			break;
		default:
			abort();
		};
		GFXView::refresh();
	}; 

	bool UIList::is_fixed_size() const noexcept
	{
		return (bool)this->fixed_size();
	};
	void UIList::use_fixed_size(Rect _r) noexcept
	{
		this->fixed_size_ = _r;
	};
	std::optional<Rect> UIList::fixed_size() const noexcept
	{
		return this->fixed_size_;
	};

	UIList::UIList(GFXContext* _context, Rect _r, AXIS _axis, DIRECTION _dir, pixels_t _margin) :
		GFXView{ _context, _r }, margin_{ _margin }, axis_{ _axis }, dir_{ _dir }
	{};

}
