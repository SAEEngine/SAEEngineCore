#include "SAEEngineCore_Object.h"

#include <SAEEngineCore_Logging.h>
#include <SAEEngineCore_Input.h>

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

	GFXContext* GFXBase::context() const noexcept
	{
		return this->context_;
	};

	void GFXBase::set_context(GFXContext* _context) noexcept
	{
		this->context_ = _context;
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

	Palette& UIObject::get_palette() const
	{
		return this->EMPTY_PALLETE;
	};

	UIObject::HANDLE_EVENT_RETURN UIObject::handle_event(const Event& _event)
	{
		return IGNORED;
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

	UIView::HANDLE_EVENT_RETURN UIView::handle_event(const Event& _ev)
	{
		auto _out = UIObject::handle_event(_ev);
		if (_out == IGNORED)
		{
			for (auto& o : this->children())
			{
				if (o->handle_event(_ev) == HANDLE_EVENT_RETURN::HANDLED)
				{
					_out = HANDLE_EVENT_RETURN::HANDLED;
					break;
				};
			};
		};
		return _out;
	};

	void UIView::insert(std::shared_ptr<UIObject> _obj)
	{
		this->add_to_graphics_context(this->context(), _obj.get());
		UIGroup::insert(_obj);
	};

	void UIView::remove(UIObject* _obj)
	{
		UIGroup::remove(_obj);
	};

}

namespace sae::engine::core
{

	void UIButton::handle_response(const EventResponse& _ev, const Event& _fromEvent)
	{
		switch (_ev.type())
		{
		case EventResponse::DEFERRED:
			this->context()->push_event(_ev.get<EventResponse::DEFERRED>());
			break;
		case EventResponse::IMMEDIATE:
			_ev.get<EventResponse::IMMEDIATE>().invoke(_fromEvent);
			break;
		default:
			abort();
		};
	};

	UIButton::HANDLE_EVENT_RETURN UIButton::handle_mouse_event(const Event::evMouse& _evmouse)
	{
		HANDLE_EVENT_RETURN _out = IGNORED;
		
		ScreenPoint _cpos{ _evmouse.cursor_x, _evmouse.cursor_y };
		if (!this->bounds().intersects(_cpos))
		{
			return IGNORED;
		};

		switch (_evmouse.action)
		{
		case GLFW_PRESS:
			switch (_evmouse.button)
			{
			case GLFW_MOUSE_BUTTON_1:
				this->is_down_ = true;
				this->down_button_ = GLFW_MOUSE_BUTTON_1;
				_out = HANDLED;
				break;
			case GLFW_MOUSE_BUTTON_2:
				this->is_down_ = true;
				this->down_button_ = GLFW_MOUSE_BUTTON_2;
				_out = HANDLED;
				break;
			default:
				break;
			};
			break;
		case GLFW_RELEASE:
			switch (_evmouse.button)
			{
			case GLFW_MOUSE_BUTTON_1:
				if (this->is_down_ && this->down_button_ == GLFW_MOUSE_BUTTON_1)
				{
					this->handle_response(this->on_mb1_, _evmouse);
					this->is_down_ = false;
				};
				break;
			case GLFW_MOUSE_BUTTON_2:
				if (this->is_down_ && this->down_button_ == GLFW_MOUSE_BUTTON_2)
				{
					this->handle_response(this->on_mb2_, _evmouse);
					this->is_down_ = false;
				};
				break;
			default:
				break;
			};
			break;
		default:
			break;
		};
		
		return _out;
	};

	UIButton::HANDLE_EVENT_RETURN UIButton::handle_event(const Event& _event)
	{
		using EVENT = Event::EVENT_TYPE;
		auto _out = UIView::handle_event(_event);
		if (_out == IGNORED)
		{
			switch (_event.index())
			{
			case Event::EVENT_TYPE_E::MOUSE_EVENT:
				_out = this->handle_mouse_event(_event.get<Event::EVENT_TYPE_E::MOUSE_EVENT>());
				break;
			default:
				break;
			};
		};
		return _out;
	};

	UIButton::UIButton(UIRect _r, const EventResponse& _leftMouse, const EventResponse& _rightMouse) :
		UIView{ _r }, on_mb1_{ _leftMouse }, on_mb2_{ _rightMouse }
	{};

}

namespace sae::engine::core
{

	void UIPushButton::handle_response(const EventResponse& _ev, const Event& _fromEvent)
	{
		switch (_ev.type())
		{
		case EventResponse::DEFERRED:
			this->context()->push_event(_ev.get<EventResponse::DEFERRED>());
			break;
		case EventResponse::IMMEDIATE:
			_ev.get<EventResponse::IMMEDIATE>().invoke(_fromEvent);
			break;
		default:
			abort();
		};
	};

	UIPushButton::HANDLE_EVENT_RETURN UIPushButton::handle_mouse_event(const Event::evMouse& _evmouse)
	{
		HANDLE_EVENT_RETURN _out = IGNORED;

		ScreenPoint _cpos{ _evmouse.cursor_x, _evmouse.cursor_y };
		if (!this->bounds().intersects(_cpos))
		{
			return IGNORED;
		};

		switch (_evmouse.action)
		{
		case GLFW_PRESS:
			if (!this->is_down_)
			{
				this->is_down_ = true;
				this->down_button_ = _evmouse.button;
				this->handle_response(this->on_push_, _evmouse);
				_out = HANDLED;
			};
			break;
		case GLFW_RELEASE:
			if (this->is_down_ && _evmouse.button == this->down_button_)
			{
				this->handle_response(this->on_release_, _evmouse);
				this->is_down_ = false;
				_out = HANDLED;
			}
			break;
		default:
			break;
		};

		return _out;
	};
	UIPushButton::HANDLE_EVENT_RETURN UIPushButton::handle_cursor_event(const Event::evCursorMove& _event)
	{
		if (!this->is_hovered_ && !this->is_down_ && this->bounds().intersects({ _event.cursor_x, _event.cursor_y }))
		{
			this->is_hovered_ = true;
			this->handle_response(this->on_mouse_enter_, _event);
			return HANDLED;
		}
		else if (!this->bounds().intersects({ _event.cursor_x, _event.cursor_y }))
		{
			this->is_hovered_ = false;
			this->is_down_ = false;
			this->handle_response(this->on_mouse_leave_, _event);
			return HANDLED;
		};
		return IGNORED;
	};

	UIPushButton::HANDLE_EVENT_RETURN UIPushButton::handle_event(const Event& _event)
	{
		using EVENT = Event::EVENT_TYPE;
		auto _out = UIView::handle_event(_event);
		if (_out == IGNORED)
		{
			switch (_event.index())
			{
			case Event::EVENT_TYPE_E::MOUSE_EVENT:
				_out = this->handle_mouse_event(_event.get<Event::EVENT_TYPE_E::MOUSE_EVENT>());
				break;
			case Event::EVENT_TYPE_E::CURSOR_MOVE:
				_out = this->handle_cursor_event(_event.get<Event::EVENT_TYPE_E::CURSOR_MOVE>());
				break;
			default:
				break;
			};
		};
		return _out;
	};

	UIPushButton::UIPushButton(	UIRect _r, const EventResponse& _onPush, const EventResponse& _onRelease,
								const EventResponse& _onMouseEnter, const EventResponse& _onMouseLeave) :
		UIView{ _r }, on_push_{ _onPush }, on_release_{ _onRelease }, on_mouse_enter_{ _onMouseEnter }, on_mouse_leave_{ _onMouseLeave }
	{};

}

namespace sae::engine::core
{
	void UIToggleButton::handle_response(const EventResponse& _ev, const Event& _fromEvent)
	{

	};

	UIToggleButton::HANDLE_EVENT_RETURN UIToggleButton::handle_mouse_event(const Event::evMouse& _evmouse)
	{

	};
	UIToggleButton::HANDLE_EVENT_RETURN UIToggleButton::handle_cursor_event(const Event::evCursorMove& _event)
	{

	};

	UIToggleButton::HANDLE_EVENT_RETURN UIToggleButton::handle_event(const Event& _event)
	{

	};

}


namespace sae::engine::core
{
	void GFXWindow::glfw_mouse_button_callback(GLFWwindow* _window, int _button, int _action, int _mods)
	{
		auto _ptr = (GFXWindow*)glfwGetWindowUserPointer(_window);
		if (_ptr)
		{
			Event::evMouse _evm{};
			_evm.action = _action;
			_evm.button = _button;
			_evm.mods = _mods;

			auto _cursorPos = Cursor::get_position(_window);
			_evm.cursor_x = _cursorPos.x;
			_evm.cursor_y = _cursorPos.y;

			_ptr->handle_event(_evm);
		};
	};

	void GFXWindow::glfw_cursor_move_callback(GLFWwindow* _window, double _x, double _y)
	{
		auto _ptr = (GFXWindow*)glfwGetWindowUserPointer(_window);
		if (_ptr)
		{
			Event::evCursorMove _evm{};
			_evm.cursor_x = (int16_t)_x;
			_evm.cursor_y = (int16_t)_y;
			_ptr->handle_event(_evm);
		};
	};





	void GFXWindow::draw()
	{
		this->make_current();
		UIView::draw();
		this->swap_buffers();
	};

	GFXWindow::HANDLE_EVENT_RETURN GFXWindow::handle_event(const Event& _event)
	{
		using EVENT = Event::EVENT_TYPE;
		auto _out = UIView::handle_event(_event);
		if (_out == IGNORED)
		{
			
		};
		return _out;
	};

	GFXWindow::GFXWindow(ScreenPoint::pixels_t _width, ScreenPoint::pixels_t _height, const std::string& _title) : 
		Window{ glfwCreateWindow(_width, _height, _title.c_str(), NULL, NULL) }, 
		UIView{ UIRect{ { 0, 0 }, { _width, _height } } }
	{
		this->add_to_context(this);
		glfwSetWindowUserPointer(this->get(), this);
		glfwSetMouseButtonCallback(this->get(), &GFXWindow::glfw_mouse_button_callback );
		glfwSetCursorPosCallback(this->get(), &GFXWindow::glfw_cursor_move_callback );
	};

}