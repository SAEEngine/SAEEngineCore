#include "SAEEngineCore_Object.h"

#include <cassert>

namespace sae::engine::core
{
	/**
	 * @brief Sets a grow bit to 1
	*/
	GrowMode& GrowMode::set(GROW_BIT_E _bit) noexcept
	{
		this->bits_ |= (uint8_t)_bit;
		return *this;
	};

	/**
	 * @brief Sets a grow bit to 0
	*/
	GrowMode& GrowMode::clear(GROW_BIT_E _bit) noexcept
	{
		this->bits_ &= ~((uint8_t)_bit);
		return *this;
	};

	/**
	 * @brief Sets the specified grow bit to the specified value
	 * @param _bit Bit to set
	 * @param _val Value to set it to
	*/
	GrowMode& GrowMode::set_to(GROW_BIT_E _bit, bool _val) noexcept
	{
		if (_val)
		{
			return this->set(_bit);
		}
		else
		{
			return this->clear(_bit);
		};
	};

}

namespace sae::engine::core
{

	void GFXObject::on_state_change(STATE_BITS _b, bool _to) {};

	void GFXObject::set_state_bit(STATE_BITS _bit) noexcept
	{
		this->state_ |= _bit;
		this->on_state_change(_bit, true);
	};
	void GFXObject::clear_state_bit(STATE_BITS _bit) noexcept
	{
		this->state_ &= ~_bit;
		this->on_state_change(_bit, false);
	};
	bool GFXObject::check_state_bit(STATE_BITS _bit) const noexcept
	{
		return (this->state_ & _bit) != 0;
	};

	void GFXObject::set_parent(GFXView* _to) noexcept
	{
		this->parent_ = _to;
	};
	void GFXObject::set_context(GFXContext* _to)
	{
		//assert(!this->context());
		this->context_ = _to;
	};

	GFXView* GFXObject::parent() const noexcept
	{
		return this->parent_;
	};
	bool GFXObject::has_parent() const noexcept
	{
		return this->parent() != nullptr;
	};

	GFXContext* GFXObject::context() const noexcept
	{
		return this->context_;
	};

	void GFXObject::handle_event_type(const Event::evGrow& _event)
	{
		this->grow(_event.dw, _event.dh);
	};

	Rect& GFXObject::bounds() noexcept
	{
		return this->bounds_;
	};
	const Rect& GFXObject::bounds() const noexcept
	{
		return this->bounds_;
	};

	ZLayer& GFXObject::zlayer() noexcept
	{
		return this->z_;
	};
	const ZLayer& GFXObject::zlayer() const noexcept
	{
		return this->z_;
	};

	GrowMode& GFXObject::grow_mode() noexcept
	{
		return this->grow_mode_;
	};
	const GrowMode& GFXObject::grow_mode() const noexcept
	{
		return this->grow_mode_;
	};

	void GFXObject::handle_event(Event& _event) {};

	void GFXObject::refresh() {};
	void GFXObject::grow(pixels_t _dw, pixels_t _dh)
	{
		using GBIT = GrowMode::GROW_BIT_E;

		const auto& _gmode = this->grow_mode();
		auto& _b = this->bounds();

		if (_gmode.is_set(GBIT::gmLeft))
		{
			_b.left() += _dw;
		};
		if (_gmode.is_set(GBIT::gmRight))
		{
			_b.right() += _dw;
		};
		if (_gmode.is_set(GBIT::gmTop))
		{
			_b.top() += _dh;
		};
		if (_gmode.is_set(GBIT::gmBottom))
		{
			_b.bottom() += _dh;
		};

	};

	GFXObject::GFXObject(Rect _r) :
		bounds_{ _r }
	{};

	GFXObject::GFXObject() :
		GFXObject{ Rect{} }
	{};
	GFXObject::~GFXObject() {};

}

namespace sae::engine::core
{
	void GFXGroup::insert_child(value_type _obj)
	{
		this->children().push_back(std::move(_obj));
	};
	void GFXGroup::remove_child(GFXObject* _obj)
	{
		this->children().erase(std::find_if(this->children().begin(), this->children().end(), [_obj](const auto& o) {
			return o.get() == _obj;
			}));
	};

	void GFXGroup::handle_event(Event& _event)
	{
		GFXObject::handle_event(_event);
		if (_event)
		{
			for (auto& o : this->children())
			{
				o->handle_event(_event);
				if (!_event)
				{
					break;
				};
			};
		};
	};

	void GFXGroup::set_context(GFXContext* _to)
	{
		GFXObject::set_context(_to);
		for (auto& o : this->children())
		{
			o->set_context(_to);
		};
	};

	GFXGroup::container_type& GFXGroup::children() noexcept
	{
		return this->children_;
	};
	const GFXGroup::container_type& GFXGroup::children() const noexcept
	{
		return this->children_;
	};

	void GFXGroup::refresh()
	{
		GFXObject::refresh();
		for (auto& o : this->children())
		{
			o->refresh();
		};
	};
	void GFXGroup::grow(pixels_t _dw, pixels_t _dh)
	{
		GFXObject::grow(_dw, _dh);
		for (auto& o : this->children())
		{
			o->grow(_dw, _dh);
		};
	};

	GFXGroup::GFXGroup(Rect _r) :
		GFXObject{ _r }
	{};

}

namespace sae::engine::core
{

	GFXView::iterator GFXView::begin() noexcept
	{
		return this->children().begin();
	};
	GFXView::const_iterator GFXView::begin() const noexcept
	{
		return this->children().cbegin();
	};
	GFXView::const_iterator GFXView::cbegin() const noexcept
	{
		return this->children().cbegin();
	};

	GFXView::iterator GFXView::end() noexcept
	{
		return this->children().end();
	};
	GFXView::const_iterator GFXView::end() const noexcept
	{
		return this->children().cend();
	};
	GFXView::const_iterator GFXView::cend() const noexcept
	{
		return this->children().cend();
	};

	GFXView::size_type GFXView::child_count() const noexcept
	{
		return this->children().size();
	};

	GFXView::reference GFXView::first_child() noexcept
	{
		return this->children().front();
	};
	GFXView::const_reference GFXView::first_child() const noexcept
	{
		return this->children().front();
	};

	GFXView::reference GFXView::last_child() noexcept
	{
		return this->children().back();
	};
	GFXView::const_reference GFXView::last_child() const noexcept
	{
		return this->children().back();
	};

	GFXView::reference GFXView::child(size_t i)
	{
		return this->children().at(i);
	};
	GFXView::const_reference GFXView::child(size_t i) const
	{
		return this->children().at(i);
	};
	
	void GFXView::clear() noexcept
	{
		this->children().clear();
	};

	void GFXView::insert(value_type _obj)
	{
		if (_obj->has_parent())
		{
			assert(_obj->parent() != this);
			_obj->parent()->remove(_obj.get());
		};
		assert(!_obj->has_parent());
		_obj->set_parent(this);
		if (_obj->context() == nullptr)
		{
			assert(this->context() != nullptr);
			_obj->set_context(this->context());
		};
		GFXGroup::insert_child(_obj);
	};
	void GFXView::emplace(GFXObject* _obj)
	{
		this->insert(value_type{ _obj });
	};

	void GFXView::remove(GFXObject* _obj)
	{
		GFXGroup::remove_child(_obj);
	};

	GFXView::GFXView(GFXContext* _context, Rect _r) :
		GFXGroup{ _r }
	{
		this->set_context(_context);
	};

}

namespace sae::engine::core
{
	void GFXContext::draw()
	{
		for (auto& o : this->artists_)
		{
			o->draw();
		};
	};

	void GFXContext::handle_event(Event& _event)
	{
		for (auto& a : this->artists_)
		{
			a->handle_event(_event);
		};
		GFXView::handle_event(_event);
	};

	void GFXContext::register_artist(const std::string& _name, std::unique_ptr<IArtist> _artist)
	{
		this->artist_names_.insert({ _name, _artist.get() });
		this->artists_.push_back(std::move(_artist));
	};
	IArtist* GFXContext::find_artist(const std::string& _name)
	{
		return this->artist_names_.at(_name);
	};

	GLFWwindow* GFXContext::window() const noexcept
	{
		return this->window_;
	};

	GFXContext::GFXContext(GLFWwindow* _window, Rect _r) :
		GFXView{ this, _r }, window_{ _window }
	{};
	GFXContext::GFXContext(GLFWwindow* _window) :
		GFXContext{ _window, {} }
	{};

	GFXContext::~GFXContext()
	{
		this->clear();
	};

}
