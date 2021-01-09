#include "SAEEngineCore_UI.h"

#include <cassert>
#include <array>

namespace sae::engine::core
{
	const SpecID UIPanel::VERTEX_SPEC = new_spec_id();

	Artist* UIPanel::artist() const noexcept
	{
		return this->artist_;
	};
	bool UIPanel::has_artist() const noexcept
	{
		return this->artist() != nullptr;
	};

	bool UIPanel::handle_event_type(const Event::evUser& _event)
	{
		auto _out = true;
		switch (_event)
		{
		case evOptionSet:
			switch (_event.content)
			{
			case opDraw:
				this->show();
				break;
			default:
				_out = false;
				break;
			};
			break;
		case evOptionCleared:
			switch (_event.content)
			{
			case opDraw:
				this->hide();
				break;
			default:
				_out = false;
				break;
			};
			break;
		default:
			_out = false;
			break;
		};
		return _out;
	};


	void UIPanel::set_context(GFXContext* _context)
	{
		GFXObject::set_context(_context);
		this->set_artist(this->context()->find_artist(this->VERTEX_SPEC));
		if (this->check_option(opDraw))
		{
			this->show();
		};
	};
	void UIPanel::set_artist(Artist* _artist)
	{
		if (this->has_artist())
		{
			this->artist()->remove(this);
		};
		this->artist_ = _artist;
	};

	namespace
	{
		union UIPanelVertex
		{
			struct
			{
				float_t x;
				float_t y;
				float_t z;
				uint8_t r;
				uint8_t g;
				uint8_t b;
				uint8_t a;
			};
			std::array<std::byte, (sizeof(float_t) * 3) + (sizeof(uint8_t) * 4)> arr{};
		};

		UIPanelVertex make_panel_vertex(float_t _x, float_t _y, float_t _z, UIPanel::color_type _col)
		{
			UIPanelVertex _v{};
			_v.x = _x;
			_v.y = _y;
			_v.z = _z;
			_v.r = _col.r;
			_v.g = _col.g;
			_v.b = _col.b;
			_v.a = _col.a;
			return _v;
		};

	}

	void UIPanel::show()
	{
		assert(this->has_artist());

		VertexData _vdata{ VERTEX_SPEC, 6, (sizeof(float_t) * 3) + (sizeof(uint8_t) * 4) };
		
		float_t x0 = (float_t)this->bounds().left();
		float_t y0 = (float_t)this->bounds().bottom();

		float_t x1 = (float_t)this->bounds().right();
		float_t y1 = (float_t)this->bounds().top();

		float_t z = (float_t)this->bounds().left();

		UIPanelVertex _v0 = make_panel_vertex(x0, y0, z, this->color());
		UIPanelVertex _v1 = make_panel_vertex(x1, y0, z, this->color());
		UIPanelVertex _v2 = make_panel_vertex(x0, y1, z, this->color());
		UIPanelVertex _v3 = make_panel_vertex(x1, y1, z, this->color());

		auto _vit = _vdata.begin();
		std::copy(_v0.arr.begin(), _v0.arr.end(), _vit);
		_vit += _v0.arr.size();
		std::copy(_v1.arr.begin(), _v1.arr.end(), _vit);
		_vit += _v0.arr.size();
		std::copy(_v2.arr.begin(), _v2.arr.end(), _vit);

		_vit += _v0.arr.size();
		std::copy(_v1.arr.begin(), _v1.arr.end(), _vit);
		_vit += _v0.arr.size();
		std::copy(_v2.arr.begin(), _v2.arr.end(), _vit);
		_vit += _v0.arr.size();
		std::copy(_v3.arr.begin(), _v3.arr.end(), _vit);




		this->artist()->insert(this, _vdata);
	};
	void UIPanel::hide()
	{
		assert(this->has_artist());
		this->artist()->remove(this);

	};

	UIPanel::color_type& UIPanel::color() noexcept
	{
		return this->color_;
	};
	const UIPanel::color_type& UIPanel::color() const noexcept
	{ 
		return this->color_;
	};

	void UIPanel::refresh()
	{
		if (this->check_option(opDraw))
		{
			this->hide();
			this->show();
		};
		GFXObject::refresh();
	};
	void UIPanel::handle_event(Event& _event)
	{
		GFXObject::handle_event(_event);
		using EVENT = Event::EVENT_TYPE_E;
		switch (_event.index())
		{
		case EVENT::USER_EVENT:
			if (this->handle_event_type(_event.get<EVENT::USER_EVENT>()))
			{
				_event.clear();
			};
			break;
		default:
			break;
		};
	};

	UIPanel::UIPanel(Rect _r, color_type _col) :
		GFXObject{ _r }, color_{ _col }
	{};
	UIPanel::UIPanel(color_type _col) :
		UIPanel{ Rect{}, _col }
	{};
	UIPanel::UIPanel(Rect _r) :
		UIPanel{ _r, color_type{} }
	{};
	UIPanel::UIPanel() :
		UIPanel{ Rect{}, color_type{} }
	{};

	UIPanel::~UIPanel()
	{
		if (this->has_artist() && this->check_option(opDraw))
		{
			this->hide();
		};
	};

}

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
