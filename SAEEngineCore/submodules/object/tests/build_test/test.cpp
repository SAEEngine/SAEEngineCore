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
	void draw_self() override
	{
		eng::UBind _uvao{ &this->vao_ };

		float_t _fvColor[3]{};
		this->set_color(
			std::get<1>(this->get_palette().at((size_t)this->get_state()))
		);

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




	BoxTest(eng::UIRect _r) :
		eng::UIToggleButton{ _r, evBoxToggleON, evBoxToggleOFF }
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

struct TestWindow : public eng::GFXWindow
{

	HANDLE_EVENT_RETURN handle_event(const eng::Event& _event) override
	{
		using EVENT = eng::Event::EVENT_TYPE;
		auto _out = eng::GFXWindow::handle_event(_event);
		if (_out == HANDLE_EVENT_RETURN::IGNORED)
		{
			
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
	_boxRect.a.x = 50;
	_boxRect.b.x = 150;
	_boxRect.a.y = 50;
	_boxRect.b.y = 150;

	std::shared_ptr<BoxTest> _box{ new BoxTest{ _boxRect } };
	_window.insert(_box);

	_boxRect.shift(200, 0);
	std::shared_ptr<BoxTest> _box2{ new BoxTest{ _boxRect } };
	_window.insert(_box2);

	auto _wortho = _window.bounds().ortho();
	uint64_t _tcounter = 0;

	while (true)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(_box->shader_);
		glUniformMatrix4fv(_box->projection_pos_, 1, GL_FALSE, &_wortho[0][0]);

		_window.draw();
		_glfw.poll_events();
		
		std::this_thread::sleep_for(std::chrono::milliseconds{ 10 });
	};

	return GOOD_TEST;
};