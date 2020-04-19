#include "rmpch.h"
#include "OpenGLShader.h"

#include <fstream>

#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"


namespace RoMan
{

	OpenGLShader::OpenGLShader(const std::string& filepath)
		:m_AssetPath(filepath)
	{
		//Get filename from path
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		m_Name = filepath.substr(lastSlash, count);

		Reload();
	}

	Ref<OpenGLShader> OpenGLShader::CreateFromString(const std::string& source)
	{
		Ref<OpenGLShader> shader = std::make_shared<OpenGLShader>();
		shader->Load(source);
		return shader;
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RendererID);
	}

	void OpenGLShader::Reload()
	{
		std::string source = ReadShaderFromFile(m_AssetPath);
		Load(source);
	}

	void OpenGLShader::AddShaderReloadedCallback(const ShaderReloadedCallback& callback)
	{
		m_ShaderReloadedCallbacks.push_back(callback);
	}

	void OpenGLShader::Load(const std::string& source)
	{
		m_ShaderSource = PreProcess(source);
		Parse();

		RM_RENDER_S({
			if (self->m_RendererID)
				glDeleteShader(self->m_RendererID);

			self->CompileAndUploadShader();
			self->ResolveUniforms();
			self->ValidateUniforms();

			if (self->m_Loaded)
			{
				for (auto& callback : self->m_ShaderReloadedCallbacks)
					callback();
			}
			self->m_Loaded = true;
		});
	}

	std::string OpenGLShader::ReadShaderFromFile(const std::string& filepath)
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else
		{
			RM_CORE_ERROR("Couldn't open file path {0}", filepath);
		}

		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		std::unordered_map < GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			RM_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			RM_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}


	GLenum OpenGLShader::ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		else if (type == "fragment")
			return GL_FRAGMENT_SHADER;

		RM_CORE_ASSERT(false, "Unknown ShaderType!");

		return 0;
	}

	void OpenGLShader::ResolveAndSetUniforms(const Scope<OpenGLShaderUniformBufferDeclaration>& decl, Buffer buffer)
	{
		const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
		for (size_t i = 0; i < uniforms.size(); i++)
		{
			OpenGLShaderUniformDeclaration* uniform = (OpenGLShaderUniformDeclaration*)uniforms[i];
			if (uniform->IsArray())
				ResolveAndSetUniformArray(uniform, buffer);
			else
				ResolveAndSetUniform(uniform, buffer);
		}
	}

	void OpenGLShader::ResolveAndSetUniform(OpenGLShaderUniformDeclaration* uniform, Buffer buffer)
	{
		if (uniform->GetLocation() == -1)
			return;

		RM_CORE_ASSERT(uniform->GetLocation() != -1, "Invalid Uniform Location");

		uint32_t offset = uniform->GetOffset();
		switch (uniform->GetType())
		{
			case OpenGLShaderUniformDeclaration::Type::INT32:
				UploadUniformInt(uniform->GetName(), *(int32_t*)&buffer.Data[offset]);
				return;
			case OpenGLShaderUniformDeclaration::Type::FLOAT32:
				UploadUniformFloat(uniform->GetName(), *(float*)&buffer.Data[offset]);
				return;
			case OpenGLShaderUniformDeclaration::Type::VEC2:
				UploadUniformFloat2(uniform->GetName(), *(glm::vec2*)&buffer.Data[offset]);
				return;
			case OpenGLShaderUniformDeclaration::Type::VEC3:
				UploadUniformFloat3(uniform->GetName(), *(glm::vec3*) & buffer.Data[offset]);
				return;
			case OpenGLShaderUniformDeclaration::Type::VEC4:
				UploadUniformFloat4(uniform->GetName(), *(glm::vec4*) & buffer.Data[offset]);
				return;
			case OpenGLShaderUniformDeclaration::Type::MAT3:
				UploadUniformMatrix3(uniform->GetName(), *(glm::mat3*) & buffer.Data[offset]);
				return;
			case OpenGLShaderUniformDeclaration::Type::MAT4:
				UploadUniformMatrix4(uniform->GetName(), *(glm::mat4*) & buffer.Data[offset]);
				return;
			case OpenGLShaderUniformDeclaration::Type::STRUCT:
				UploadUniformStruct(uniform, buffer.Data, offset);
				return;
			default:
				RM_CORE_ASSERT(false, "Unknown Uniform Type!");
		}
	}

	void OpenGLShader::ResolveAndSetUniformArray(OpenGLShaderUniformDeclaration* uniform, Buffer buffer)
	{
		if (uniform->GetLocation() == -1)
			return;

		RM_CORE_ASSERT(uniform->GetLocation() != -1, "Invalid Uniform Location");

		uint32_t offset = uniform->GetOffset();
		switch (uniform->GetType())
		{
			case OpenGLShaderUniformDeclaration::Type::INT32:
				UploadUniformInt(uniform->GetName(), *(int32_t*)&buffer.Data[offset]);
				return;
			case OpenGLShaderUniformDeclaration::Type::FLOAT32:
				UploadUniformFloat(uniform->GetName(), *(float*)&buffer.Data[offset]);
				return;
			case OpenGLShaderUniformDeclaration::Type::VEC2:
				UploadUniformFloat2(uniform->GetName(), *(glm::vec2*) & buffer.Data[offset]);
				return;
			case OpenGLShaderUniformDeclaration::Type::VEC3:
				UploadUniformFloat3(uniform->GetName(), *(glm::vec3*) & buffer.Data[offset]);
				return;
			case OpenGLShaderUniformDeclaration::Type::VEC4:
				UploadUniformFloat4(uniform->GetName(), *(glm::vec4*) & buffer.Data[offset]);
				return;
			case OpenGLShaderUniformDeclaration::Type::MAT3:
				UploadUniformMatrix3(uniform->GetName(), *(glm::mat3*) & buffer.Data[offset]);
				return;
			case OpenGLShaderUniformDeclaration::Type::MAT4:
				UploadUniformMatrix4Array(uniform->GetLocation(), *(glm::mat4*) & buffer.Data[offset], uniform->GetCount());
				return;
			case OpenGLShaderUniformDeclaration::Type::STRUCT:
				UploadUniformStruct(uniform, buffer.Data, offset);
				return;
			default:
				RM_CORE_ASSERT(false, "Unknown Uniform Type!");
		}
	}

	void OpenGLShader::ResolveAndSetUniformField(const OpenGLShaderUniformDeclaration& field, byte* data, int32_t offset)
	{
		switch (field.GetType())
		{
			case OpenGLShaderUniformDeclaration::Type::FLOAT32:
				UploadUniformFloat(field.GetLocation(), *(float*)&data[offset]);
				break;
			case OpenGLShaderUniformDeclaration::Type::INT32:
				UploadUniformInt(field.GetLocation(), *(int32_t*)&data[offset]);
				break;
			case OpenGLShaderUniformDeclaration::Type::VEC2:
				UploadUniformFloat2(field.GetLocation(), *(glm::vec2*) & data[offset]);
				break;
			case OpenGLShaderUniformDeclaration::Type::VEC3:
				UploadUniformFloat3(field.GetLocation(), *(glm::vec3*) & data[offset]);
				break;
			case OpenGLShaderUniformDeclaration::Type::VEC4:
				UploadUniformFloat4(field.GetLocation(), *(glm::vec4*) & data[offset]);
				break;
			case OpenGLShaderUniformDeclaration::Type::MAT3:
				UploadUniformMatrix3(field.GetLocation(), *(glm::mat3*) & data[offset]);
				break;
			case OpenGLShaderUniformDeclaration::Type::MAT4:
				UploadUniformMatrix4(field.GetLocation(), *(glm::mat4*) & data[offset]);
				break;
			default:
				RM_CORE_ASSERT(false, "Unknown uniform type!");
		}
	}




	// Parsing helper functions
	const char* FindToken(const char* str, const std::string& token)
	{
		const char* t = str;
		while (t = strstr(t, token.c_str()))
		{
			bool left = str == t || isspace(t[-1]);
			bool right = !t[token.size()] || isspace(t[token.size()]);
			if (left && right)
				return t;

			t += token.size();
		}
		return nullptr;
	}

	const char* FindToken(const std::string& string, const std::string& token)
	{
		return FindToken(string.c_str(), token);
	}

	std::vector<std::string> SplitString(const std::string& string, const std::string& delimiters)
	{
		size_t start = 0;
		size_t end = string.find_first_of(delimiters);

		std::vector<std::string> result;

		while (end <= std::string::npos)
		{
			std::string token = string.substr(start, end - start);
			if (!token.empty())
				result.push_back(token);

			if (end == std::string::npos)
				break;

			start = end + 1;
			end = string.find_first_of(delimiters, start);
		}

		return result;
	}

	std::vector<std::string> SplitString(const std::string& string, const char delimiter)
	{
		return SplitString(string, std::string(1, delimiter));
	}

	std::vector<std::string> Tokenize(const std::string& string)
	{
		return SplitString(string, " \t\n");
	}

	std::vector<std::string> GetLines(const std::string& string)
	{
		return SplitString(string, "\n");
	}

	std::string GetBlock(const char* str, const char** outPosition)
	{
		const char* end = strstr(str, "}");
		if (!end)
			return str;

		if (outPosition)
			*outPosition = end;
		uint32_t length = end - str + 1;
		return std::string(str, length);
	}

	std::string GetStatement(const char* str, const char** outPosition)
	{
		const char* end = strstr(str, ";");
		if (!end)
			return str;

		if (outPosition)
			*outPosition = end;
		uint32_t length = end - str + 1;
		return std::string(str, length);
	}

	bool StartsWith(const std::string& string, const std::string& start)
	{
		return string.find(start) == 0;
	}


	void OpenGLShader::Parse()
	{
		const char* token;
		const char* vstr;
		const char* fstr;

		m_Resources.clear();
		m_Structs.clear();
		m_VSMaterialUniformBuffer.reset();
		m_FSMaterialUniformBuffer.reset();

		auto& vertexSource = m_ShaderSource[GL_VERTEX_SHADER];
		auto& fragmentSource = m_ShaderSource[GL_FRAGMENT_SHADER];

		//Vertex Shader
		vstr = vertexSource.c_str();
		while (token = FindToken(vstr, "struct"))
			ParseUniformStruct(GetBlock(token, &vstr), ShaderDomain::Vertex);

		vstr = vertexSource.c_str();
		while (token = FindToken(vstr, "uniform"))
			ParseUniform(GetStatement(token, &vstr), ShaderDomain::Vertex);

		//Fragment Shader
		fstr = fragmentSource.c_str();
		while (token = FindToken(fstr, "struct"))
			ParseUniformStruct(GetBlock(token, &fstr), ShaderDomain::Fragment);

		fstr = fragmentSource.c_str();
		while (token = FindToken(fstr, "uniform"))
			ParseUniform(GetStatement(token, &fstr), ShaderDomain::Fragment);

	}

	static bool IsTypeStringResource(const std::string& type)
	{
		if (type == "sampler2D")		return true;
		if (type == "samplerCube")		return true;
		if (type == "sampler2DShadow")	return true;
		return false;
	}



	void OpenGLShader::ParseUniform(const std::string& statement, ShaderDomain domain)
	{
		std::vector<std::string> tokens = Tokenize(statement);
		uint32_t index = 0;

		index++; // "uniform"
		std::string typeString = tokens[index++];
		std::string name = tokens[index++];
		// Strip ; from name if present
		if (const char* s = strstr(name.c_str(), ";"))
			name = std::string(name.c_str(), s - name.c_str());

		std::string n(name);
		int32_t count = 1;
		const char* namestr = n.c_str();
		if (const char* s = strstr(namestr, "["))
		{
			name = std::string(namestr, s - namestr);

			const char* end = strstr(namestr, "]");
			std::string c(s + 1, end - s);
			count = atoi(c.c_str());
		}

		if (IsTypeStringResource(typeString))
		{
			ShaderResourceDeclaration* declaration = new OpenGLShaderResourceDeclaration(OpenGLShaderResourceDeclaration::StringToType(typeString), name, count);
			m_Resources.push_back(declaration);
		}
		else
		{
			OpenGLShaderUniformDeclaration::Type t = OpenGLShaderUniformDeclaration::StringToType(typeString);
			OpenGLShaderUniformDeclaration* declaration = nullptr;

			if (t == OpenGLShaderUniformDeclaration::Type::NONE)
			{
				// Find struct
				ShaderStruct* s = FindStruct(typeString);
				RM_CORE_ASSERT(s, "");
				declaration = new OpenGLShaderUniformDeclaration(domain, s, name, count);
			}
			else
			{
				declaration = new OpenGLShaderUniformDeclaration(domain, t, name, count);
			}

			if (StartsWith(name, "r_"))
			{
				if (domain == ShaderDomain::Vertex)
					((OpenGLShaderUniformBufferDeclaration*)m_VSRendererUniformBuffers.front())->PushUniform(declaration);
				else if (domain == ShaderDomain::Fragment)
					((OpenGLShaderUniformBufferDeclaration*)m_FSRendererUniformBuffers.front())->PushUniform(declaration);
			}
			else
			{
				if (domain == ShaderDomain::Vertex)
				{
					if (!m_VSMaterialUniformBuffer)
						m_VSMaterialUniformBuffer.reset(new OpenGLShaderUniformBufferDeclaration("", domain));

					m_VSMaterialUniformBuffer->PushUniform(declaration);
				}
				else if (domain == ShaderDomain::Fragment)
				{
					if (!m_FSMaterialUniformBuffer)
						m_FSMaterialUniformBuffer.reset(new OpenGLShaderUniformBufferDeclaration("", domain));

					m_FSMaterialUniformBuffer->PushUniform(declaration);
				}
			}
		}
	}
	void OpenGLShader::ParseUniformStruct(const std::string& block, ShaderDomain domain)
	{
		std::vector<std::string> tokens = Tokenize(block);

		uint32_t index = 0;
		index++; // struct
		std::string name = tokens[index++];
		ShaderStruct* uniformStruct = new ShaderStruct(name);
		index++; // {
		while (index < tokens.size())
		{
			if (tokens[index] == "}")
				break;

			std::string type = tokens[index++];
			std::string name = tokens[index++];

			// Strip ; from name if present
			if (const char* s = strstr(name.c_str(), ";"))
				name = std::string(name.c_str(), s - name.c_str());

			uint32_t count = 1;
			const char* namestr = name.c_str();
			if (const char* s = strstr(namestr, "["))
			{
				name = std::string(namestr, s - namestr);

				const char* end = strstr(namestr, "]");
				std::string c(s + 1, end - s);
				count = atoi(c.c_str());
			}
			ShaderUniformDeclaration* field = new OpenGLShaderUniformDeclaration(domain, OpenGLShaderUniformDeclaration::StringToType(type), name, count);
			uniformStruct->AddField(field);
		}
		m_Structs.push_back(uniformStruct);
	}


	ShaderStruct* OpenGLShader::FindStruct(const std::string& name)
	{
		for (ShaderStruct* s : m_Structs)
		{
			if (s->GetName() == name)
				return s;
		}
		return nullptr;
	}

	void OpenGLShader::Bind() const
	{
		RM_RENDER_S({
			glUseProgram(self->m_RendererID);
		});
	}

	void OpenGLShader::UnBind() const
	{
		RM_RENDER_S({
			glUseProgram(0);
		});
	}

	void OpenGLShader::UploadUniformBuffer(const UniformBufferBase& uniformBuffer)
	{
		for (unsigned int i = 0; i < uniformBuffer.GetUniformCount(); i++)
		{
			const UniformDecl& decl = uniformBuffer.GetUniforms()[i];
			switch (decl.Type)
			{
				case UniformType::Float:
				{
					const std::string& name = decl.Name;
					float value = *(float*)(uniformBuffer.GetBuffer() + decl.Offset);
					RM_RENDER_S2(name, value, {
							self->UploadUniformFloat(name, value);
						});
				}

				case UniformType::Float2:
				{
					const std::string& name = decl.Name;
					glm::vec2 value = *(glm::vec2*)(uniformBuffer.GetBuffer() + decl.Offset);
					RM_RENDER_S2(name, value, {
							self->UploadUniformFloat2(name, value);
						});
				}

				case UniformType::Float3:
				{
					const std::string& name = decl.Name;
					glm::vec3 value = *(glm::vec3*)(uniformBuffer.GetBuffer() + decl.Offset);
					RM_RENDER_S2(name, value, {
							self->UploadUniformFloat3(name, value);
						});
				}

				case UniformType::Float4:
				{
					const std::string& name = decl.Name;
					glm::vec4 value = *(glm::vec4*)(uniformBuffer.GetBuffer() + decl.Offset);
					RM_RENDER_S2(name, value, {
							self->UploadUniformFloat4(name, value);
						});
				}


				case UniformType::Matrix3x3:
				{
					const std::string& name = decl.Name;
					glm::mat3 value = *(glm::mat3*)(uniformBuffer.GetBuffer() + decl.Offset);
					RM_RENDER_S2(name, value, {
							self->UploadUniformMatrix3(name, value);
						});
				}

				case UniformType::Matrix4x4:
				{
					const std::string& name = decl.Name;
					glm::mat4 value = *(glm::mat4*)(uniformBuffer.GetBuffer() + decl.Offset);
					RM_RENDER_S2(name, value, {
							self->UploadUniformMatrix4(name, value);
						});
				}
			}
		}
	}


	void OpenGLShader::CompileAndUploadShader()
	{
		std::vector<GLuint> shaderRendererIDs;

		GLuint program = glCreateProgram();
		for (auto& kv : m_ShaderSource)
		{
			GLenum type = kv.first;
			std::string& source = kv.second;

			GLuint shaderRendererID = glCreateShader(type);
			const GLchar* sourceCstr = (const GLchar*)source.c_str();
			glShaderSource(shaderRendererID, 1, &sourceCstr, 0);

			glCompileShader(shaderRendererID);

			GLint isCompiled = 0;
			glGetShaderiv(shaderRendererID, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shaderRendererID, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shaderRendererID, maxLength, &maxLength, &infoLog[0]);

				RM_CORE_ERROR("Shader compilation failed:\n{0}", &infoLog[0]);

				// We don't need the shader anymore.
				glDeleteShader(shaderRendererID);

				RM_CORE_ASSERT(false, "Failed");
			}

			shaderRendererIDs.push_back(shaderRendererID);
			glAttachShader(program, shaderRendererID);
		}

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
			RM_CORE_ERROR("Shader compilation failed:\n{0}", &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);
			// Don't leak shaders either.
			for (auto id : shaderRendererIDs)
				glDeleteShader(id);
		}

		// Always detach shaders after a successful link.
		for (auto id : shaderRendererIDs)
			glDetachShader(program, id);

		m_RendererID = program;
	}

	void OpenGLShader::SetFloat(const std::string& name, float value)
	{
		RM_RENDER_S2(name, value, {
				self->UploadUniformFloat(name, value);
			});
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		RM_RENDER_S2(name, value, {
				self->UploadUniformMatrix4(name, value);
			});
	}

	void OpenGLShader::SetMat4FromRenderThread(const std::string& name, const glm::mat4& value)
	{
		UploadUniformMatrix4(name, value);
	}

	void OpenGLShader::SetVSMaterialUniformBuffer(Buffer buffer)
	{
		RM_RENDER_S1(buffer, {
			glUseProgram(self->m_RendererID);
			self->ResolveAndSetUniforms(self->m_VSMaterialUniformBuffer, buffer);
		});
	}

	void OpenGLShader::SetFSMaterialUniformBuffer(Buffer buffer)
	{
		RM_RENDER_S1(buffer, {
			glUseProgram(self->m_RendererID);
			self->ResolveAndSetUniforms(self->m_FSMaterialUniformBuffer, buffer);
		});
	}

	void OpenGLShader::UploadUniformInt(const std::string& name, int32_t value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformIntArray(const std::string& name, int32_t* values, int32_t count)
	{
		int32_t location = GetUniformLocation(name);
		glUniform1iv(location, count, values);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2f(location, value.x, value.y);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::UploadUniformMatrix3(const std::string& name, const glm::mat3& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMatrix4(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformInt(uint32_t location, int32_t value)
	{
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformIntArray(uint32_t location, int32_t* values, int32_t count)
	{
		glUniform1iv(location, count, values);
	}

	void OpenGLShader::UploadUniformFloat(uint32_t location, float value)
	{
		glUniform1f(location, value);
	}

	void OpenGLShader::UploadUniformFloat2(uint32_t location, glm::vec2& value)
	{
		glUniform2f(location, value.x, value.y);
	}

	void OpenGLShader::UploadUniformFloat3(uint32_t location, glm::vec3& value)
	{
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLShader::UploadUniformFloat4(uint32_t location, glm::vec4& value)
	{
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::UploadUniformMatrix3(uint32_t location, const glm::mat3& matrix)
	{
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMatrix4(uint32_t location, const glm::mat4& matrix)
	{
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMatrix4Array(uint32_t location, const glm::mat4& matrix, uint32_t count)
	{
		glUniformMatrix4fv(location, count, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformStruct(OpenGLShaderUniformDeclaration* uniform, byte* buffer, uint32_t offset)
	{
		const ShaderStruct& s = uniform->GetShaderUniformStruct();
		const auto& fields = s.GetFields();
		for (size_t k = 0; k < fields.size(); k++)
		{
			OpenGLShaderUniformDeclaration* field = (OpenGLShaderUniformDeclaration*)fields[k];
			ResolveAndSetUniformField(*field, buffer, offset);
			offset += field->GetSize();
		}
	}

	int32_t OpenGLShader::GetUniformLocation(const std::string& name) const
	{
		int32_t result = glGetUniformLocation(m_RendererID, name.c_str());
		if (result == -1)
			RM_CORE_WARN("Could not find uniform {0} in shader", name);

		return result;
	}

	void OpenGLShader::ResolveUniforms()
	{
		glUseProgram(m_RendererID);

		for (size_t i = 0; i < m_VSRendererUniformBuffers.size(); i++)
		{
			OpenGLShaderUniformBufferDeclaration* decl = (OpenGLShaderUniformBufferDeclaration*)m_VSRendererUniformBuffers[i];
			const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
			for (size_t j = 0; j < uniforms.size(); j++)
			{
				OpenGLShaderUniformDeclaration* uniform = (OpenGLShaderUniformDeclaration*)uniforms[j];
				if (uniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
				{
					const ShaderStruct& s = uniform->GetShaderUniformStruct();
					const auto& fields = s.GetFields();
					for (size_t k = 0; k < fields.size(); k++)
					{
						OpenGLShaderUniformDeclaration* field = (OpenGLShaderUniformDeclaration*)fields[k];
						field->m_Location = GetUniformLocation(uniform->GetName() + "." + field->GetName());
					}
				}
				else
				{
					uniform->m_Location = GetUniformLocation(uniform->GetName());
				}
			}
		}

		for (size_t i = 0; i < m_FSRendererUniformBuffers.size(); i++)
		{
			OpenGLShaderUniformBufferDeclaration* decl = (OpenGLShaderUniformBufferDeclaration*)m_FSRendererUniformBuffers[i];
			const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
			for (size_t j = 0; j < uniforms.size(); j++)
			{
				OpenGLShaderUniformDeclaration* uniform = (OpenGLShaderUniformDeclaration*)uniforms[j];
				if (uniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
				{
					const ShaderStruct& s = uniform->GetShaderUniformStruct();
					const auto& fields = s.GetFields();
					for (size_t k = 0; k < fields.size(); k++)
					{
						OpenGLShaderUniformDeclaration* field = (OpenGLShaderUniformDeclaration*)fields[k];
						field->m_Location = GetUniformLocation(uniform->GetName() + "." + field->GetName());
					}
				}
				else
				{
					uniform->m_Location = GetUniformLocation(uniform->GetName());
				}
			}
		}

		{
			const auto& decl = m_VSMaterialUniformBuffer;
			if (decl)
			{
				const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
				for (size_t j = 0; j < uniforms.size(); j++)
				{
					OpenGLShaderUniformDeclaration* uniform = (OpenGLShaderUniformDeclaration*)uniforms[j];
					if (uniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
					{
						const ShaderStruct& s = uniform->GetShaderUniformStruct();
						const auto& fields = s.GetFields();
						for (size_t k = 0; k < fields.size(); k++)
						{
							OpenGLShaderUniformDeclaration* field = (OpenGLShaderUniformDeclaration*)fields[k];
							field->m_Location = GetUniformLocation(uniform->GetName() + "." + field->GetName());
						}
					}
					else
					{
						uniform->m_Location = GetUniformLocation(uniform->GetName());
					}
				}
			}
		}

		{
			const auto& decl = m_FSMaterialUniformBuffer;
			if (decl)
			{
				const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
				for (size_t j = 0; j < uniforms.size(); j++)
				{
					OpenGLShaderUniformDeclaration* uniform = (OpenGLShaderUniformDeclaration*)uniforms[j];
					if (uniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
					{
						const ShaderStruct& s = uniform->GetShaderUniformStruct();
						const auto& fields = s.GetFields();
						for (size_t k = 0; k < fields.size(); k++)
						{
							OpenGLShaderUniformDeclaration* field = (OpenGLShaderUniformDeclaration*)fields[k];
							field->m_Location = GetUniformLocation(uniform->GetName() + "." + field->GetName());
						}
					}
					else
					{
						uniform->m_Location = GetUniformLocation(uniform->GetName());
					}
				}
			}
		}

		uint32_t sampler = 0;
		for (size_t i = 0; i < m_Resources.size(); i++)
		{
			OpenGLShaderResourceDeclaration* resource = (OpenGLShaderResourceDeclaration*)m_Resources[i];
			int32_t location = GetUniformLocation(resource->GetName());

			if (resource->GetCount() == 1)
			{
				resource->m_Register = sampler;
				if (location != -1)
					UploadUniformInt(location, sampler);

				sampler++;
			}
			else if (resource->GetCount() > 1)
			{
				resource->m_Register = 0;
				uint32_t count = resource->GetCount();
				int* samplers = new int[count];
				for (uint32_t s = 0; s < count; s++)
				{
					samplers[s] = 0;
				}
				UploadUniformIntArray(resource->GetName(), samplers, count);
				delete[] samplers;
			}
		}

	}

	void OpenGLShader::ValidateUniforms()
	{
	}



}