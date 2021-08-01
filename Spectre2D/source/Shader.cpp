#include "..\include\Spectre2D\Shader.h"
#include "..\include\Spectre2D\RenderWindow.h"
#include "..\include\Spectre2D\core.h"

#include <GL/glew.h>

#include <iostream>

namespace sp
{
	/*
	* Spectre2D's shaders by default serve the following purposes:
	* 1. Multiplying each vertex by viewprojection matrix and by their model matrix.
	* 2. Giving each pixel of a primitive an interpolated color
	* 3. Giving each pixel of a sprite respective interpolated UV coordinates
	* 4. Coloring each pixel based on its tint, interpolated color (usually white) and texture.
	* 
	* The shaders have to detect wether or not the pixel should be influenced by a texture.
	* To achieve this, a uniform bool, `sp_textureAssociated` should be present in all shader interfaces.
	* This parameter should specify wether or not the pixel is associated with a texture.
	*/
	const std::string defaultVertexShaderCode = R"DELIM(
void effect(out vec4 m_position, inout highp float m_pointSize)
{
	m_position = sp_viewProj * sp_model * vec4(sp_vertexPosition, 1.0);
	sp_interpolatedColor = sp_vertexColor;
	sp_interpolatedUV = sp_vertexUV;
}
)DELIM";

	const std::string defaultFragmentShaderCode = R"DELIM(
void effect(out vec4 m_color)
{
	if (sp_textureAssociated)
	{
		m_color = sp_tint * texture(sp_texture, sp_interpolatedUV) * sp_interpolatedColor;
	}
	else
	{
		m_color = sp_tint * sp_interpolatedColor;
	}
}
)DELIM";

	ShaderSource defaultVertexShaderSource;
	ShaderSource defaultFragmentShaderSource;

	const std::string vertexShaderMain = R"DELIM(
void main()
{
	vec4 m_position;
	highp float m_pointSize = sp_pointSize;

	effect(m_position, m_pointSize);
	gl_Position = m_position;
	gl_PointSize = m_pointSize;
}
)DELIM";

	const std::string fragmentShaderMain = R"DELIM(
void main()
{
	vec4 m_color;

	effect(m_color);
	sp_fragColor = m_color;
}
)DELIM";

	VDS defaultVertexInterface;
	VDS defaultVertFragInterface;
	VDS defaultUniformInterface;
	VDS defaultOutputInterface;

	ShaderSource::ShaderSource()
	{
	}

	ShaderSource::ShaderSource(const std::filesystem::path& path)
	{
		create(path);
	}

	ShaderSource::ShaderSource(std::istream& input, int32_t size)
	{
		create(input, size);
	}

	void ShaderSource::create(const std::filesystem::path& path)
	{
		std::ifstream file(path);

		if (file.is_open())
			create(file);
	}

	void ShaderSource::create(std::istream& input, int32_t size)
	{
		if (input.good())
		{
			if (size == -1)
			{
				// Add lines until stream lines end
				std::string line;
				while (std::getline(input, line))
				{
					shader += line + std::string(1, '\n');
				}
			}
			else
			{
				// Load data directly into the buffer and then into the source
				char* str = new char[size + 1];

				input.read(str, size);

				str[size] = '\0';

				shader = str;
			}
		}
	}

	void ShaderSource::createLiteral(const std::string& _shader)
	{
		shader = _shader;
	}

	const std::string& ShaderSource::getText() const
	{
		return shader;
	}

	const ShaderSource& ShaderSource::getDefaultVertexShaderSource()
	{
		if (defaultVertexShaderSource.getText().empty())
			defaultVertexShaderSource.createLiteral(defaultVertexShaderCode);

		return defaultVertexShaderSource;
	}

	const ShaderSource& ShaderSource::getDefaultFragmentShaderSource()
	{
		if (defaultFragmentShaderSource.getText().empty())
			defaultFragmentShaderSource.createLiteral(defaultFragmentShaderCode);

		return defaultFragmentShaderSource;
	}

	Shader::Shader()
	{
		programID = 0;
		owner = nullptr;
		resetInterfaces();
	}

	Shader::Shader(RenderWindow* _owner, const ShaderSource& vertexShader, const ShaderSource& fragmentShader)
	{
		programID = 0;
		owner = nullptr;
		resetInterfaces();

		create(_owner, vertexShader, fragmentShader);
	}

	Shader::Shader(RenderWindow* _owner, const std::vector<ShaderSource>& vertexShaderSources, const std::vector<ShaderSource>& fragmentShaderSources)
	{
		programID = 0;
		resetInterfaces();

		create(_owner, vertexShaderSources, fragmentShaderSources);
	}

	Shader::~Shader()
	{
		if (programID)
			glDeleteProgram(programID);

		programID = 0;
	}

	void Shader::create(RenderWindow* _owner, const ShaderSource& vertexShader, const ShaderSource& fragmentShader)
	{
		create(_owner, std::vector<ShaderSource>{ vertexShader }, { fragmentShader });
	}

	void Shader::create(RenderWindow* _owner, const std::vector<ShaderSource>& vertexShaderSources, const std::vector<ShaderSource>& fragmentShaderSources)
	{
		owner = _owner;

		clearShaders();

		GLint vertexShaderID = 0;
		GLint fragmentShaderID = 0;

		size_t vShaderSize = 0;
		size_t fShaderSize = 0;

		// Create the shader's "header" (interpret vertexInterface)
		std::string vertInter = getInterfaceString(vertexInterface, "in");
		std::string vsVertFragInter = getInterfaceString(vertFragInterface, "out", false);
		std::string fsVertFragInter = getInterfaceString(vertFragInterface, "in", false);
		std::string uniformInter = getInterfaceString(uniformInterface, "uniform", false);
		std::string outputInter = getInterfaceString(outputInterface, "out");

		// vertexshader: vertexInterface, vertFragInterface, uniformInterface
		// fragmentshader: vertFragInterface, outputInterface, uniformInterface
		const size_t additionalSources = 5;
		const size_t userSourceOffset = 4;

		// + 1 to make the compiler shut up
		const GLchar** vSources = new const GLchar* [vertexShaderSources.size() + additionalSources + 1];
		const GLchar** fSources = new const GLchar* [fragmentShaderSources.size() + additionalSources + 1];
		GLint success = 0;
		GLchar infoLog[512] = { 0 };

		// Vertex shader
		// 
		// Structure:
		// 1. vertex attributes
		// 2. output attributes
		// 3. uniforms
		// 4. void effect(out vec4 m_position);
		// 5. void main();
		// 
		// Define program-defined parts
		vSources[0] = "#version 330 core\n"; // Size is 19
		vSources[1] = vertInter.c_str(); // vertexInterface
		vSources[2] = vsVertFragInter.c_str(); // vertFragInterface
		vSources[3] = uniformInter.c_str(); // uniformInterface
		vSources[userSourceOffset + vertexShaderSources.size()] = vertexShaderMain.c_str();

		// + 19 for version macro
		vShaderSize += vertInter.size() + vsVertFragInter.size() + uniformInter.size() + vertexShaderMain.size() + 19;

		// Load sources
		for (size_t i = 0; i < vertexShaderSources.size(); i++)
		{
			vSources[userSourceOffset + i] = vertexShaderSources[i].getText().c_str();
			vShaderSize += vertexShaderSources[i].getText().size();
		}

		// Create shader
		vertexShaderID = glCreateShader(GL_VERTEX_SHADER);

		// Attach the sources
		glShaderSource(vertexShaderID, vertexShaderSources.size() + additionalSources, vSources, NULL);

		// Compile
		glCompileShader(vertexShaderID);

		// Check if loaded properly
		glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(vertexShaderID, 512, NULL, infoLog);
			throw Error(SPECTRE_INVALID_FILE, std::string("Error occured when compiling vertex shader:\n") + infoLog);
		}

		// Get the source
		char* buff = new char[vShaderSize + 10];
		glGetShaderSource(vertexShaderID, vShaderSize + 10, NULL, buff);
		vertexSource.createLiteral(buff);

		// Fragment shader
		// 
		// Structure:
		// 1. input attributes
		// 2. output attributes
		// 3. uniforms
		// 4. void effect(out vec4 m_position);
		// 5. void main();
		// 
		// Define program-defined parts
		fSources[0] = "#version 330 core\n";
		fSources[1] = fsVertFragInter.c_str(); // vertexInterface
		fSources[2] = outputInter.c_str(); // vertFragInterface
		fSources[3] = uniformInter.c_str(); // uniformInterface
		fSources[userSourceOffset + vertexShaderSources.size()] = fragmentShaderMain.c_str();

		// + 19 for version macro
		fShaderSize += fsVertFragInter.size() + outputInter.size() + uniformInter.size() + fragmentShaderMain.size() + 19;

		// Load sources
		for (size_t i = 0; i < fragmentShaderSources.size(); i++)
		{
			fSources[userSourceOffset + i] = fragmentShaderSources[i].getText().c_str();
			fShaderSize += fragmentShaderSources[i].getText().size();
		}

		// Create shader
		fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

		// Attach the sources
		glShaderSource(fragmentShaderID, fragmentShaderSources.size() + additionalSources, fSources, NULL);

		// Compile
		glCompileShader(fragmentShaderID);

		// Check if loaded properly
		glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(fragmentShaderID, 512, NULL, infoLog);
			throw Error(SPECTRE_INVALID_FILE, std::string("Error occured when compiling fragment shader:\n") + infoLog);
		}

		buff = new char[fShaderSize + 10];
		glGetShaderSource(fragmentShaderID, fShaderSize + 10, NULL, buff);
		fragmentSource.createLiteral(buff);

		// Clear source arrays and buffer
		delete[] vSources, fSources;
		delete[] buff;

		// Create program
		programID = glCreateProgram();

		// Attach shaders
		glAttachShader(programID, vertexShaderID);
		glAttachShader(programID, fragmentShaderID);

		// Link
		glLinkProgram(programID);

		// Check if linked properly
		glGetProgramiv(programID, GL_LINK_STATUS, &success);

		if (!success)
		{
			glGetProgramInfoLog(programID, 512, NULL, infoLog);
			throw Error(SPECTRE_INVALID_FILE, std::string("Error occured when linking shader program:\n") + infoLog);
		}

		// Flag the shaders for deletion
		glDeleteShader(vertexShaderID);
		glDeleteShader(fragmentShaderID);

		// Get the currently active shader and activate this
		Shader* active = owner->getActiveShader();
		glUseProgram(programID);

		// Get the uniform locations and pass them into the map
		for (const auto& unit : uniformInterface.getUnits())
		{
			if (unit.size == 1)
				uniformValues[unit.name].first = glGetUniformLocation(programID, unit.name.c_str());
			else
				for (size_t i = 0; i < unit.size; i++)
				{
					std::string name = unit.name + "[" + std::to_string(i) + "]";
					uniformValues[name].first = glGetUniformLocation(programID, name.c_str());
				}
		}

		// Pass the uniform values
		setRawUniforms();

		// Reset the active shader
		owner->setActiveShader(active);
	}

	uint32_t Shader::getOpenGLID() const
	{
		return programID;
	}

	void Shader::setVertexInterface(const VDS& inter)
	{
		if (programID)
			throw Error(SPECTRE_SETTING_AFTER_CREATION, "Attempt to set vertex interface of a shader after its creation.");

		vertexInterface = inter;
	}

	const VDS& Shader::getVertexInterface() const
	{
		return vertexInterface;
	}

	void Shader::setVertFragInterface(const VDS& inter)
	{
		if (programID)
			throw Error(SPECTRE_SETTING_AFTER_CREATION, "Attempt to set vertexshader <-> fragmentshader interface of a shader after its creation.");

		vertFragInterface = inter;
	}

	const VDS& Shader::getVertFragInterface() const
	{
		return vertFragInterface;
	}

	void Shader::setUniformInterface(const VDS& inter)
	{
		if (programID)
			throw Error(SPECTRE_SETTING_AFTER_CREATION, "Attempt to set uniform interface of a shader after its creation.");

		uniformInterface = inter;
		int32_t textureUnit = 0;

		// Note: switches even with one case are kept to make the code expandable in the future

		for (const auto& unit : uniformInterface.getUnits())
		{
			UniformValue value;
			size_t matrixRows = unit.matrixRows ? unit.matrixRows : unit.innerSize;

			switch (unit.type)
			{
			case VDT::Ignore:
			case VDT::Empty:
				break;

			default:
				for (size_t i = 0; i < unit.size; i++)
				{
					switch (unit.innerSize)
					{
					case 1:
						// This is a normal value
						switch (unit.type)
						{
						case VDT::Int:
						case VDT::Bool:
							value = 0;
							break;

						case VDT::Float:
							value = 0.0f;
							break;

						case VDT::Uint:
							value = 0u;
							break;

						case VDT::Texture2D:
							value = textureUnit;
							textureUnit++;
							break;

						default:
							throw Error(SPECTRE_INVALID_VALUE, "Given uniform type is not supported.");
						}
						break;
					case 2:
						switch (matrixRows)
						{
						case 1:
							// This is tvec2
							switch (unit.type)
							{
							case VDT::Int:
							case VDT::Bool:
								value = glm::ivec2(0);
								break;

							case VDT::Float:
								value = glm::vec2(0.0f);
								break;

							case VDT::Uint:
								value = glm::uvec2(0u);
								break;

							default:
								throw Error(SPECTRE_INVALID_VALUE, "Given uniform type is not supported.");
							}
							break;
						case 2:
							// This is mat2
							switch (unit.type)
							{
							case VDT::Float:
								value = glm::mat2(0.0f);
								break;

							default:
								throw Error(SPECTRE_INVALID_VALUE, "Given uniform type is not supported.");
							}
							break;
						case 3:
							// This is mat2x3
							switch (unit.type)
							{
							case VDT::Float:
								value = glm::mat2x3(0.0f);
								break;

							default:
								throw Error(SPECTRE_INVALID_VALUE, "Given uniform type is not supported.");
							}
							break;
						case 4:
							// This is mat2x4
							switch (unit.type)
							{
							case VDT::Float:
								value = glm::mat2x4(0.0f);
								break;

							default:
								throw Error(SPECTRE_INVALID_VALUE, "Given uniform type is not supported.");
							}
							break;
						}
						break;
					case 3:
						switch (matrixRows)
						{
						case 1:
							// This is tvec3
							switch (unit.type)
							{
							case VDT::Int:
							case VDT::Bool:
								value = glm::ivec3(0);
								break;

							case VDT::Float:
								value = glm::vec3(0.0f);
								break;

							case VDT::Uint:
								value = glm::uvec3(0u);
								break;

							default:
								throw Error(SPECTRE_INVALID_VALUE, "Given uniform type is not supported.");
							}
							break;
						case 2:
							// This is mat3x2
							switch (unit.type)
							{
							case VDT::Float:
								value = glm::mat3x2(0.0f);
								break;

							default:
								throw Error(SPECTRE_INVALID_VALUE, "Given uniform type is not supported.");
							}
							break;
						case 3:
							// This is mat3
							switch (unit.type)
							{
							case VDT::Float:
								value = glm::mat3(0.0f);
								break;

							default:
								throw Error(SPECTRE_INVALID_VALUE, "Given uniform type is not supported.");
							}
							break;
						case 4:
							// This is mat3x4
							switch (unit.type)
							{
							case VDT::Float:
								value = glm::mat3x4(0.0f);
								break;

							default:
								throw Error(SPECTRE_INVALID_VALUE, "Given uniform type is not supported.");
							}
							break;
						}
						break;
					case 4:
						switch (matrixRows)
						{
						case 1:
							// This is tvec4
							switch (unit.type)
							{
							case VDT::Int:
							case VDT::Bool:
								value = glm::ivec4(0);
								break;

							case VDT::Float:
								value = glm::vec4(0.0f);
								break;

							case VDT::Uint:
								value = glm::uvec4(0u);
								break;

							default:
								throw Error(SPECTRE_INVALID_VALUE, "Given uniform type is not supported.");
							}
							break;
						case 2:
							// This is mat4x2
							switch (unit.type)
							{
							case VDT::Float:
								value = glm::mat4x2(0.0f);
								break;

							default:
								throw Error(SPECTRE_INVALID_VALUE, "Given uniform type is not supported.");
							}
							break;
						case 3:
							// This is mat4x3
							switch (unit.type)
							{
							case VDT::Float:
								value = glm::mat4x3(0.0f);
								break;

							default:
								throw Error(SPECTRE_INVALID_VALUE, "Given uniform type is not supported.");
							}
							break;
						case 4:
							// This is mat4
							switch (unit.type)
							{
							case VDT::Float:
								value = glm::mat4(0.0f);
								break;

							default:
								throw Error(SPECTRE_INVALID_VALUE, "Given uniform type is not supported.");
							}
							break;
						}
					}

					uniformValues.insert({ unit.size == 1 ? unit.name : (unit.name + "[" + std::to_string(i) + "]"), {0, value} });
				}
			}
		}
	}

	const VDS& Shader::getUniformInterface() const
	{
		return uniformInterface;
	}

	void Shader::setOutputInterface(const VDS& inter)
	{
		if (programID)
			throw Error(SPECTRE_SETTING_AFTER_CREATION, "Attempt to set output interface of a shader after its creation.");

		outputInterface = inter;
	}

	const VDS& Shader::getOutputInterface() const
	{
		return outputInterface;
	}

	void Shader::resetInterfaces()
	{
		setVertexInterface(getDefaultVertexInterface());
		setVertFragInterface(getDefaultVertFragInterface());
		setUniformInterface(getDefaultUniformInterface());
		setOutputInterface(getDefaultOutputInterface());
	}

	int32_t Shader::getUniformLocation(const std::string& uniform) const
	{
		auto it = uniformValues.find(uniform);

		if (it != uniformValues.end())
			return it->second.first;
		else
			return glGetUniformLocation(programID, uniform.c_str());
	}

	void Shader::setRawUniform(const std::string& uniform, const UniformValue& value) const
	{
		// Find location
		int32_t location = getUniformLocation(uniform);

		setRawUniform(location, value);
	}

	void Shader::setRawUniform(int32_t location, const UniformValue& value) const
	{
		/*
		* typedef std::variant<
		* float_t,
		* glm::vec2,
		* glm::vec3,
		* glm::vec4,
		* int32_t,
		* glm::ivec2,
		* glm::ivec3,
		* glm::ivec4,
		* uint32_t,
		* glm::uvec2,
		* glm::uvec3,
		* glm::uvec4,
		* glm::mat2,
		* glm::mat3,
		* glm::mat4,
		* glm::mat2x3,
		* glm::mat3x2,
		* glm::mat2x4,
		* glm::mat4x2,
		* glm::mat3x4,
		* glm::mat4x3
		* > UniformValue;
		*/

		switch (value.index())
		{
		case 0:
			// float
			glUniform1f(location, std::get<float_t>(value));
			break;

		case 1:
			// vec2
			glUniform2fv(location, 1, glm::value_ptr(std::get<glm::vec2>(value)));
			break;

		case 2:
			// vec3
			glUniform3fv(location, 1, glm::value_ptr(std::get<glm::vec3>(value)));
			break;

		case 3:
			// vec4
			glUniform4fv(location, 1, glm::value_ptr(std::get<glm::vec4>(value)));
			break;

		case 4:
			// int
			glUniform1i(location, std::get<int32_t>(value));
			break;

		case 5:
			// ivec2
			glUniform2iv(location, 1, glm::value_ptr(std::get<glm::ivec2>(value)));
			break;

		case 6:
			// ivec3
			glUniform3iv(location, 1, glm::value_ptr(std::get<glm::ivec3>(value)));
			break;

		case 7:
			// ivec4
			glUniform4iv(location, 1, glm::value_ptr(std::get<glm::ivec4>(value)));
			break;

		case 8:
			// uint
			glUniform1ui(location, std::get<uint32_t>(value));
			break;

		case 9:
			// uvec2
			glUniform2uiv(location, 1, glm::value_ptr(std::get<glm::uvec2>(value)));
			break;

		case 10:
			// uvec3
			glUniform3uiv(location, 1, glm::value_ptr(std::get<glm::uvec3>(value)));
			break;

		case 11:
			// uvec4
			glUniform4uiv(location, 1, glm::value_ptr(std::get<glm::uvec4>(value)));
			break;

		case 12:
			// mat2
			glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(std::get<glm::mat2>(value)));
			break;

		case 13:
			// mat3
			glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(std::get<glm::mat3>(value)));
			break;

		case 14:
			// mat4
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(std::get<glm::mat4>(value)));
			break;

		case 15:
			// mat2x3
			glUniformMatrix2x3fv(location, 1, GL_FALSE, glm::value_ptr(std::get<glm::mat2x3>(value)));
			break;

		case 16:
			// mat3x2
			glUniformMatrix3x2fv(location, 1, GL_FALSE, glm::value_ptr(std::get<glm::mat3x2>(value)));
			break;

		case 17:
			// mat2x4
			glUniformMatrix2x4fv(location, 1, GL_FALSE, glm::value_ptr(std::get<glm::mat2x4>(value)));
			break;

		case 18:
			// mat4x2
			glUniformMatrix4x2fv(location, 1, GL_FALSE, glm::value_ptr(std::get<glm::mat4x2>(value)));
			break;

		case 19:
			// mat3x4
			glUniformMatrix3x4fv(location, 1, GL_FALSE, glm::value_ptr(std::get<glm::mat3x2>(value)));
			break;

		case 20:
			// mat4x3
			glUniformMatrix4x3fv(location, 1, GL_FALSE, glm::value_ptr(std::get<glm::mat4x3>(value)));
			break;
		}
	}

	void Shader::setRawUniforms() const
	{
		for (const auto& [key, value] : uniformValues)
		{
			setRawUniform(value.first, value.second);
		}
	}

	void Shader::setUniform(const std::string& uniform, const UniformValue& value)
	{
		auto it = uniformValues.find(uniform);

		if (it == uniformValues.end())
			throw Error(SPECTRE_INVALID_VALUE, "Attempt to set nonexistent uniform value. If the uniform is not registered in interface, use Shader::setRawUniform().");

		if (it->second.second.index() != value.index())
			throw Error(SPECTRE_INVALID_VALUE, "Attempt to set uniform value to a value of invalid type.");

		it->second.second = value;

		if (owner->getActiveShader() == this)
			setRawUniform(uniform, value);
	}

	const ShaderSource& Shader::getVertexSource() const
	{
		return vertexSource;
	}

	const ShaderSource& Shader::getFragmentSource() const
	{
		return fragmentSource;
	}

	std::string Shader::getInterfaceString(const VDS& inter, const std::string& prefix, bool useLayout)
	{
		std::string strInter;
		GLuint attribIndex = 0;

		for (const auto& unit : inter.getUnits())
		{
			// For each unit
			std::string type;
			size_t matrixRows = unit.matrixRows ? unit.matrixRows : unit.innerSize;

			// Specify the unit's glsl type name
			if (unit.matrixRows == 1 && unit.innerSize == 1)
			{
				switch (unit.type)
				{
				case VDT::Ignore:
					// Skip the ignored attributes
					attribIndex += unit.size;

					[[fallthrough]];
				case VDT::Empty:
					continue;
					break;

				case VDT::Bool:
					type = "bool";
					break;

				case VDT::Double:
					type = "double";
					break;

				case VDT::Float:
					type = "highp float";
					break;

				case VDT::Int:
					type = "int";
					break;

				case VDT::Uint:
					type = "uint";
					break;

				case VDT::Texture2D:
					type = "sampler2D";
				}
			}
			else
			{
				switch (unit.type)
				{
				case VDT::Ignore:
					// Skip the ignored attributes
					attribIndex += unit.size;

					[[fallthrough]];
				case VDT::Empty:
					continue;
					break;

				case VDT::Bool:
					type = "b";
					break;

				case VDT::Double:
					type = "d";
					break;

				case VDT::Float:
					type = "";
					break;

				case VDT::Int:
					type = "i";
					break;

				case VDT::Uint:
					type = "u";
					break;

				default:
					throw Error(SPECTRE_INVALID_ENUM, "Incorrect type for a vector/matrix specified in the shader.");
				}
			}

			if (matrixRows == 1)
			{
				if (useLayout)
					strInter += "layout(location=" + std::to_string(attribIndex) + ") ";

				strInter += prefix + " " + type;

				if (unit.innerSize == 1)
				{
					// Normal value (type)
					if (unit.size == 1)
						strInter += " " + unit.name + ";\n";
					else
						strInter += " " + unit.name + "[" + std::to_string(unit.size) + "];\n";
				}
				else
				{
					// Vector (tvecn)
					if (unit.size == 1)
						strInter += "vec" + std::to_string(unit.innerSize) + " " + unit.name + ";\n";
					else
						strInter += "vec" + std::to_string(unit.innerSize) + " " + unit.name + "[" + std::to_string(unit.size) + "];\n";

				}

				// Increase location
				attribIndex += unit.locationIncrease() * unit.size;
			}
			else
			{
				if (useLayout)
					strInter += "layout(location=" + std::to_string(attribIndex) + ") ";

				strInter += prefix + " " + type;

				if (matrixRows == unit.innerSize)
				{
					// Uniform size matrix (tmatn)
					if (unit.size == 1)
						strInter += "mat" + std::to_string(unit.innerSize) + " " + unit.name + ";\n";
					else
						strInter += "mat" + std::to_string(unit.innerSize) + " " + unit.name + "[" + std::to_string(unit.size) + "];\n";
				}
				else
				{
					// Matrix (tmatnxm)
					if (unit.size == 1)
						strInter += "mat" + std::to_string(unit.innerSize) + "x" + std::to_string(matrixRows) + " " + unit.name + ";\n";
					else
						strInter += "mat" + std::to_string(unit.innerSize) + "x" + std::to_string(matrixRows) + " " + unit.name
						+ "[" + std::to_string(unit.size) + "];\n";
				}

				// Increase location
				attribIndex = unit.locationIncrease() * unit.size;
			}
		}

		return strInter;
	}

	const VDS& Shader::getDefaultVertexInterface()
	{
		if (defaultVertexInterface.getUnits().empty())
		{
			defaultVertexInterface << VDS::Vec("sp_vertexPosition", 3) << VDS::Vec("sp_vertexColor", 4) << VDS::Vec("sp_vertexUV", 2);
		}

		return defaultVertexInterface;
	}

	const VDS& Shader::getDefaultVertFragInterface()
	{
		if (defaultVertFragInterface.getUnits().empty())
		{
			defaultVertFragInterface << VDS::Vec("sp_interpolatedColor", 4) << VDS::Vec("sp_interpolatedUV", 2);
		}

		return defaultVertFragInterface;
	}

	const VDS& Shader::getDefaultUniformInterface()
	{
		if (defaultUniformInterface.getUnits().empty())
		{
			defaultUniformInterface
				<< VDS::Bool("sp_textureAssociated") << VDS::Tex2D("sp_texture")
				<< VDS::Mat("sp_viewProj", 4) << VDS::Mat("sp_model", 4)
				<< VDS::Vec("sp_tint", 4) << VDS::Float("sp_pointSize");
		}

		return defaultUniformInterface;
	}

	const VDS& Shader::getDefaultOutputInterface()
	{
		if (defaultOutputInterface.getUnits().empty())
		{
			defaultOutputInterface << VDS::Vec("sp_fragColor", 4);
		}

		return defaultOutputInterface;
	}

	void Shader::clearShaders()
	{
		if (programID)
			glDeleteProgram(programID);

		programID = 0;
	}

}