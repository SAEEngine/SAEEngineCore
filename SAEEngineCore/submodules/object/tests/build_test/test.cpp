/*
	Return GOOD_TEST (0) if the test was passed.
	Return anything other than GOOD_TEST (0) if the test was failed.
*/

// Common standard library headers

#include <cassert>

/**
 * @brief Return this from main if the test was passsed.
*/
constexpr static inline int GOOD_TEST = 0;

// Include the headers you need for testing here

#include <SAEEngineCore_Object.h>
#include <SAEEngineCore_Environment.h>
#include <SAEEngineCore_glObject.h>

#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include <iostream>
#include <cmath>

namespace eng = sae::engine::core;

#pragma region NASTY_EXTRA_BITS

const std::string VERTEX_SHADER = R"(
#version 330 core

layout(location = 0) in vec3 raw_pos;

uniform mat4 ProjectionMat;

void main() 
{
	gl_Position = ProjectionMat * vec4(raw_pos, 1.0);
};
)";
const std::string FRAGMENT_SHADER = R"(
#version 330 core

out vec3 color;

uniform vec3 BoxColor;

void main() 
{
	color = BoxColor;
};

)";

GLuint GenerateShader(const std::string& VertexShaderCode, const std::string& FragmentShaderCode) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		abort();
		//printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	//printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		abort(); //printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

#pragma endregion NASTY_EXTRA_BITS

constexpr eng::Event::evUser evBoxToggleON{ 1 };
constexpr eng::Event::evUser evBoxToggleOFF{ 2 };

struct BoxTest : public eng::UIToggleButton
{
private:
	bool on_toggleOn_cb(const eng::Event& _event)
	{
		std::cout << this->context()->blackboard().at(this->bb_key_).get<bool>() << '\n';
		return true;
	};
	bool on_toggleOff_cb(const eng::Event& _event)
	{
		std::cout << this->context()->blackboard().at(this->bb_key_).get<bool>() << '\n';
		return true;
	};

public:

	void draw_self() override
	{
		eng::UBind _uvao{ &this->vao_ };

		float_t _fvColor[3]{};
		this->set_color(this->get_palette().at((size_t)this->get_state()));

		_fvColor[0] = (float_t)this->color_.r / 255.0f;
		_fvColor[1] = (float_t)this->color_.g / 255.0f;
		_fvColor[2] = (float_t)this->color_.b / 255.0f;

		glUniform3fv(this->color_pos_, 1, _fvColor);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vbos_[1]);
		glDrawElements(GL_TRIANGLES, this->indices_.size(), GL_UNSIGNED_SHORT, 0);
		
	};

	HANDLE_EVENT_RETURN	handle_event(const eng::Event& _event) override
	{
		using EVENT = eng::Event::EVENT_TYPE;
		auto _res = UIToggleButton::handle_event(_event);
		if (_res == IGNORED)
		{
			switch (_event.index())
			{
			case EVENT::USER_EVENT:
				switch (_event.get<EVENT::USER_EVENT>().ev)
				{
				case evBoxToggleON.ev:
					_res = HANDLED;
					break;
				case evBoxToggleOFF.ev:
					_res = HANDLED;
					break;
				default:
					break;
				};
				break;
			default:
				break;
			};
		};
		return _res;
	};

	void set_color(eng::ColorRGBA_8 _col)
	{
		this->color_ = _col;
	};

	void grow(int16_t _dw, int16_t _dh) override
	{
		eng::UIToggleButton::grow(_dw, _dh);

		auto _r = this->bounds();

		this->pos_[0] = _r.left();
		this->pos_[1] = _r.top();

		this->pos_[3] = _r.right();
		this->pos_[4] = _r.top();

		this->pos_[6] = _r.right();
		this->pos_[7] = _r.bottom();

		this->pos_[9] = _r.left();
		this->pos_[10] = _r.bottom();

		eng::UBind _vb{ &this->vao_ };
		glBindBuffer(GL_ARRAY_BUFFER, this->vbos_[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->pos_), this->pos_.data());

	};

	void refresh() override
	{
		if (this->bb_key_.empty())
		{
			this->bb_key_ = this->context()->blackboard().insert_unique(false);
		};
	};

	BoxTest(eng::UIRect _r) :
		eng::UIToggleButton{ _r, {{ &BoxTest::on_toggleOn_cb, this }}, {{ &BoxTest::on_toggleOff_cb, this }} }
	{
		this->pos_[0] = _r.a.x;
		this->pos_[1] = _r.a.y;

		this->pos_[3] = _r.b.x;
		this->pos_[4] = _r.a.y;

		this->pos_[6] = _r.b.x;
		this->pos_[7] = _r.b.y;

		this->pos_[9] = _r.a.x;
		this->pos_[10] = _r.b.y;

		this->shader_ = GenerateShader(VERTEX_SHADER, FRAGMENT_SHADER);
		this->projection_pos_ = glGetUniformLocation(this->shader_, "ProjectionMat");
		this->color_pos_ = glGetUniformLocation(this->shader_, "BoxColor");

		eng::UBind _uvao{ &this->vao_ };
		glGenBuffers(2, this->vbos_);

		glBindBuffer(GL_ARRAY_BUFFER, this->vbos_[0]);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBufferData(GL_ARRAY_BUFFER, sizeof(this->pos_), this->pos_.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vbos_[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(this->indices_), this->indices_.data(), GL_STATIC_DRAW);

		((this->grow_mode() |= eng::GrowMode::BOTTOM) |= eng::GrowMode::RIGHT) |= eng::GrowMode::LEFT;

	};

	GLuint shader_ = 0;
	GLuint projection_pos_ = 0;

private:
	GLuint color_pos_ = 0;

	eng::ColorRGBA_8 color_{};

	eng::VAO vao_{};
	GLuint vbos_[2]{};

	std::array<float_t, 12> pos_ =
	{
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f
	};

	std::array<uint16_t, 6> indices_
	{
		0, 1, 2,
		0, 3, 2
	};

};

struct ElementList: public eng::UIView
{
public:
	enum POSITION_AXIS
	{
		HORIZONTAL,
		VERTICAL
	};

private:
	void resposition_elements_horizontal()
	{
		if (this->child_count() == 0)
			return;

		int16_t _x = this->bounds().left();
		int16_t _w = this->bounds().width();
		int16_t _eachWidth = (_w - (this->child_count() - 1) * this->margin_) / (this->child_count());

		for (auto& o : this->children())
		{
			auto _b = o->bounds();
			_b.left() = _x;
			_b.right() = _b.left() + _eachWidth;
			_b.top() = this->bounds().top();
			_b.bottom() = this->bounds().bottom();
			o->set_bounds(_b);
			_x += _eachWidth + this->margin_;
		};

	};
	void resposition_elements_vertical()
	{
		abort();
	};

	void reposition_elements()
	{
		switch (this->axis_)
		{
		case POSITION_AXIS::HORIZONTAL:
			this->resposition_elements_horizontal();
			break;
		case POSITION_AXIS::VERTICAL:
			this->resposition_elements_vertical();
			break;
		default:
			abort();
			break;
		};
	};

	int16_t margin_ = 5;
	POSITION_AXIS axis_ = POSITION_AXIS::HORIZONTAL;

public:
	void insert(std::shared_ptr<eng::UIObject> _ptr)
	{
		eng::UIView::insert(_ptr);
		this->reposition_elements();
	};
	void grow(int16_t _dw, int16_t _dh) override
	{
		eng::UIView::grow(_dw, _dh);
		this->reposition_elements();
	};
	void refresh() override
	{
		this->reposition_elements();
	};

	ElementList(eng::UIRect _r, POSITION_AXIS _axis, int16_t _margin) :
		eng::UIView{ _r }, axis_{ _axis }, margin_{ _margin }
	{};

};




struct TestWindow : public eng::GFXWindow
{
private:
	bool keep_running_ = true;

	void handle_event_type(const eng::Event::evWindowResize& _event)
	{
		auto _wb = this->bounds();

		auto _dw = _event.width - _wb.width();
		auto _dh = _event.height - _wb.height();

		this->grow(_dw, _dh);

		_wb.right() = _event.width;
		_wb.bottom() = _event.height;

		this->set_bounds(_wb);

	};

public:

	GLuint shader_ = 0;
	GLuint projection_pos_ = 0;

	bool good() const noexcept { return this->keep_running_; };

	void draw() override
	{
		auto _wb = this->bounds();
		auto _wortho = glm::ortho((float_t)_wb.left(), (float_t)_wb.right(), (float_t)_wb.bottom(), (float_t)_wb.top());

		glUseProgram(this->shader_);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUniformMatrix4fv(this->projection_pos_, 1, GL_FALSE, &_wortho[0][0]);

		GFXWindow::draw();

		glfwPollEvents();
	};

	HANDLE_EVENT_RETURN handle_event(const eng::Event& _event) override
	{
		using EVENT = eng::Event::EVENT_TYPE;
		auto _out = eng::GFXWindow::handle_event(_event);
		if (_out == HANDLE_EVENT_RETURN::IGNORED)
		{
			switch (_event.index())
			{
			case EVENT::WINDOW_RESIZE:
				this->handle_event_type(_event.get<EVENT::WINDOW_RESIZE>());
				_out = HANDLED;
				break;
			case EVENT::WINDOW_CLOSE:
				this->keep_running_ = false;
				_out = HANDLED;
				break;
			default:
				break;
			};
		};
		return _out;
	};

	using eng::GFXWindow::GFXWindow;

};

int main(int argc, char* argv[], char* envp[])
{
	eng::GLFWLib _glfw{};
	TestWindow _window{ 800, 600, "haha brrr" };
	_window.make_current();

	auto _boxRect = _window.bounds();
	_boxRect.grow(-10, -10);
	_boxRect.top() = _boxRect.bottom() - 100;

	std::shared_ptr<ElementList> _elist{ new ElementList{ _boxRect, ElementList::POSITION_AXIS::HORIZONTAL, 10 } };
	_window.insert(_elist);

	_elist->grow_mode().set(eng::GrowMode::RIGHT).set(eng::GrowMode::TOP).set(eng::GrowMode::BOTTOM);



	_boxRect = eng::UIRect{ 0, 0, 32, 32 };

	std::shared_ptr<BoxTest> _box0{ new BoxTest{ _boxRect } };
	_elist->insert(_box0);
	std::shared_ptr<BoxTest> _box1{ new BoxTest{ _boxRect } };
	_elist->insert(_box1);
	std::shared_ptr<BoxTest> _box2{ new BoxTest{ _boxRect } };
	_elist->insert(_box2);
	std::shared_ptr<BoxTest> _box3{ new BoxTest{ _boxRect } };
	_elist->insert(_box3);
	std::shared_ptr<BoxTest> _box4{ new BoxTest{ _boxRect } };
	_elist->insert(_box4);
	std::shared_ptr<BoxTest> _box5{ new BoxTest{ _boxRect } };
	_elist->insert(_box5);
	std::shared_ptr<BoxTest> _box6{ new BoxTest{ _boxRect } };
	_elist->insert(_box6);
	std::shared_ptr<BoxTest> _box7{ new BoxTest{ _boxRect } };
	_elist->insert(_box7);



	_window.shader_ = _box0->shader_;
	_window.projection_pos_ = _box0->projection_pos_;

	while (_window.good())
	{
		_window.draw();
		std::this_thread::sleep_for(std::chrono::milliseconds{ 10 });
	};

	return GOOD_TEST;
};