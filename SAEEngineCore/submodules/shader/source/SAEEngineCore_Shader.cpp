#include "SAEEngineCore_Shader.h"

#include <vector>


namespace sae::engine::core
{

	bool ShaderProgram::good() const noexcept
	{
		return this->id() == 0;
	};

	GLuint ShaderProgram::id() const noexcept
	{
		return this->id_;
	};
	void ShaderProgram::destroy()
	{
		glDeleteProgram(this->id());
		this->id_ = 0;
	};

	void ShaderProgram::bind()
	{
		glUseProgram(this->id());
	};
	void ShaderProgram::unbind()
	{
		glUseProgram(0);
	};



	ShaderProgram::ShaderProgram(GLuint _id) :
		id_{ _id }
	{};

	ShaderProgram::ShaderProgram(ShaderProgram&& other) : 
		id_{ std::exchange(other.id_, 0) }
	{};
	ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other)
	{
		this->destroy();
		this->id_ = std::exchange(other.id_, 0);
		return *this;
	};

	std::optional<ShaderProgram> HACK_generate_shader(std::istream& _vertex, std::istream& _fragment)
	{
		char _buff[256]{};
		std::string _vertexSource = "";
		while (!_vertex.eof())
		{
			_vertex.read(_buff, sizeof(_buff) / sizeof(char));
			_vertexSource.append(_buff, _vertex.gcount());
		};
		
		std::string _fragmentSource = "";
		while (!_fragment.eof())
		{
			_fragment.read(_buff, sizeof(_buff) / sizeof(char));
			_fragmentSource.append(_buff, _fragment.gcount());
		};


		// Create the shaders
		GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

		GLint Result = GL_FALSE;
		int InfoLogLength;

		// Compile Vertex Shader
		char const* VertexSourcePointer = _vertexSource.c_str();
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
		char const* FragmentSourcePointer = _fragmentSource.c_str();
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

		return ShaderProgram{ ProgramID };

	};






}
