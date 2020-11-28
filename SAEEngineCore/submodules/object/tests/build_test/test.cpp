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

#include <glad/glad.h>
#include <SAEEngineCore_Environment.h>

#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include <iostream>

namespace eng = sae::engine::core;

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


void main() 
{
	color = vec3(1.0, 1.0, 1.0);
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



struct BoxTest : public eng::UIView
{
	void draw_self() override
	{

	};



	BoxTest(eng::UIRect _r) :
		eng::UIView{ _r }
	{
		this->pos_[0] = _r.a.x;
		this->pos_[1] = _r.a.y;

		this->pos_[3] = _r.b.x;
		this->pos_[4] = _r.a.y;

		this->pos_[6] = _r.b.x;
		this->pos_[7] = _r.b.y;

		this->pos_[9] = _r.a.x;
		this->pos_[10] = _r.b.y;

	};

private:

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

struct SceneView : public eng::UIView
{
	std::vector<std::shared_ptr<BoxTest>> boxes_{};
	std::shared_ptr<TriDrawer> drawer_;
};

int main(int argc, char* argv[], char* envp[])
{
	eng::GLFWLib _glfw{};
	eng::GFXWindow _window{ 800, 600, "haha brrr" };

	_window.make_current();
	std::shared_ptr<SceneView> _sv{ new SceneView{ _window.bounds() } };

	auto _brect = _window.bounds();

	_brect.b.x = 100;
	_brect.b.y = 100;
	_sv->new_box(_brect);

	_brect.shift(200, 0);
	_sv->new_box(_brect);
	_window.insert(_sv);

	while (true)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		_window.draw();
		_glfw.poll_events();
		
		std::this_thread::sleep_for(std::chrono::milliseconds{ 10 });
	};

	return GOOD_TEST;
};