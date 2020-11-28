#include "SAEEngineCore_Object.h"


namespace sae::engine::core
{
	glm::mat4 UIRect::ortho() const noexcept
	{
		auto _out = glm::ortho((float_t)this->a.x, (float_t)this->b.x, (float_t)this->b.y, (float_t)this->a.y);
		return _out;
	};

	UIRect& UIRect::shift(pixels_t _dx, pixels_t _dy) noexcept
	{
		this->a.x += _dx;
		this->a.y += _dy;

		this->b.x += _dx;
		this->b.y += _dy;

		return *this;
	};

	UIRect& UIRect::grow(pixels_t _dw, pixels_t _dh) noexcept
	{
		this->a.x += _dw;
		this->a.y += _dh;
		
		this->b.x -= _dw;
		this->b.y -= _dh;

		return *this;
	};

}


namespace sae::engine::core
{

	void UIObject::set_bounds(UIRect _r) noexcept
	{
		this->bounds_ = _r;
	};

	const UIRect& UIObject::bounds() const noexcept
	{
		return this->bounds_;
	};

	void UIObject::draw_self()
	{};

	void UIObject::draw()
	{
		this->draw_self();
	};

	UIObject::UIObject(UIRect _r) :
		bounds_{ _r }
	{};

}

namespace sae::engine::core
{

	UIGroup::ContainerT& UIGroup::get_container() noexcept
	{
		return this->objs_;
	};
	const UIGroup::ContainerT& UIGroup::get_container() const noexcept
	{
		return this->objs_;
	};

	void UIGroup::insert(std::shared_ptr<UIObject> _obj)
	{
		this->get_container().push_back(std::move(_obj));
	};
	void UIGroup::remove(UIObject* _obj)
	{
		this->get_container().erase(std::find_if(this->get_container().begin(), this->get_container().end(),
			[_obj](const auto& _o)
			{
				return (_o.get() == _obj);
			}));
	};

	size_t UIGroup::child_count() const noexcept
	{
		return this->get_container().size();
	};

}

namespace sae::engine::core
{
	void GFXWindow::draw()
	{
		this->make_current();
		UIView::draw();
		this->swap_buffers();
	};

	GFXWindow::GFXWindow(ScreenPoint::pixels_t _width, ScreenPoint::pixels_t _height, const std::string& _title) : 
		Window{ glfwCreateWindow(_width, _height, _title.c_str(), NULL, NULL) }, UIView{ UIRect{ { 0, 0 }, { _width, _height } } }
	{

	};

}