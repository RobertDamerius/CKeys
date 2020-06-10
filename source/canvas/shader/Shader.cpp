#include <Shader.hpp>


Shader::Shader(){
    this->id = 0;
}

Shader::~Shader(){}

std::string Shader::GetShadingLanguageVersion(void){
    // Get string that looks like "4.40 - Build 21.20.16.4727"
    std::string strGLSL((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

    // Only use substring to first " "
    std::size_t pos = strGLSL.find_first_of(" ");
    if(std::string::npos != pos){
        strGLSL = strGLSL.substr(0, pos);
    }

    // Only use numbers
    std::string result;
    for(auto c : strGLSL){
        if((c >= '0') && (c <= '9'))
            result.push_back(c);
    }
    return result;
}

bool Shader::Generate(std::string fileName){
    return Generate(fileName, GetShadingLanguageVersion());
}

bool Shader::Generate(std::string fileName, std::string version){
    std::vector<std::pair<std::string, std::string>> emptyReplacement;
    return Generate(fileName, version, emptyReplacement);
}

bool Shader::Generate(std::string fileName, std::string version, std::vector<std::pair<std::string, std::string>>& replacement){
    std::ifstream file(fileName, std::ios::binary);
    file.unsetf(std::ios::skipws);
    if(!file.is_open())
        return false;
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<uint8_t> fileData;
    fileData.reserve(fileSize);
    fileData.insert(fileData.begin(), std::istream_iterator<uint8_t>(file), std::istream_iterator<uint8_t>());
    return Generate(fileData, version, replacement);
}

bool Shader::Generate(std::vector<uint8_t>& fileData, std::string version){
    std::vector<std::pair<std::string, std::string>> emptyReplacement;
    return Generate(fileData, version, emptyReplacement);
}

bool Shader::Generate(std::vector<uint8_t>& fileData, std::string version, std::vector<std::pair<std::string, std::string>>& replacement){
    GLuint idShaderVS, idShaderGS = 0, idShaderFS;
    GLint success, len;
    GLchar *src;
    std::string strSource;

    // Replace text and add zero terminator
    std::string shaderSource(fileData.begin(), fileData.end());
    for(std::pair<std::string, std::string> fromToPair: replacement){
        size_t start_pos = 0;
        while((start_pos = shaderSource.find(fromToPair.first, start_pos)) != std::string::npos){
            shaderSource.replace(start_pos, fromToPair.first.length(), fromToPair.second);
            start_pos += fromToPair.second.length();
        }
    }
    shaderSource.push_back(0x00);

    // Prefix management
    std::string prefixVersion = std::string("#version ") + version + std::string("\n");
    std::string prefixVS = prefixVersion + std::string("#define VERTEX_SHADER\n");
    std::string prefixGS = prefixVersion + std::string("#define GEOMETRY_SHADER\n");
    std::string prefixFS = prefixVersion + std::string("#define FRAGMENT_SHADER\n");

    // Create vertex shader
    strSource = prefixVS + shaderSource;
    idShaderVS = glCreateShader(GL_VERTEX_SHADER);
    src = (GLchar*)strSource.c_str();
    len = (GLint)strSource.length();
    DEBUG_GLCHECK( glShaderSource(idShaderVS, 1, &src, &len); );
    DEBUG_GLCHECK( glCompileShader(idShaderVS); );
    DEBUG_GLCHECK( glGetShaderiv(idShaderVS, GL_COMPILE_STATUS, &success); );
    if(!success){
        GLint logSize = 0;
        DEBUG_GLCHECK( glGetShaderiv(idShaderVS, GL_INFO_LOG_LENGTH, &logSize); );
        if(logSize){
            GLchar *infoLog = new GLchar[logSize];
            DEBUG_GLCHECK( glGetShaderInfoLog(idShaderVS, logSize, nullptr, infoLog); );
            LogError("\nVERTEX SHADER ERROR:\n%s\n", (const char*)infoLog);
            delete[] infoLog;
        }
        else{
            LogError("Could not get log size for vertex shader information!\n");
        }
        return false;
    }

    // Create geometry shader
    bool useGeometryShader = (std::string::npos != shaderSource.find("GEOMETRY_SHADER"));
    if(useGeometryShader){
        strSource = prefixGS + shaderSource;
        idShaderGS = glCreateShader(GL_GEOMETRY_SHADER);
        src = (GLchar*)strSource.c_str();
        len = (GLint)strSource.length();
        DEBUG_GLCHECK( glShaderSource(idShaderGS, 1, &src, &len); );
        DEBUG_GLCHECK( glCompileShader(idShaderGS); );
        DEBUG_GLCHECK( glGetShaderiv(idShaderGS, GL_COMPILE_STATUS, &success); );
        if(!success){
            GLint logSize = 0;
            DEBUG_GLCHECK( glGetShaderiv(idShaderGS, GL_INFO_LOG_LENGTH, &logSize); );
            if(logSize){
                GLchar *infoLog = new GLchar[logSize];
                DEBUG_GLCHECK( glGetShaderInfoLog(idShaderGS, logSize, nullptr, infoLog); );
                LogError("\nGEOMETRY SHADER ERROR:\n%s\n", (const char*)infoLog);
                delete[] infoLog;
            }
            else{
                LogError("Could not get log size for geometry shader information!\n");
            }
            return false;
        }
    }

    // Create fragment shader
    strSource = prefixFS + shaderSource;
    idShaderFS = glCreateShader(GL_FRAGMENT_SHADER);
    src = (GLchar*)strSource.c_str();
    len = (GLint)strSource.length();
    DEBUG_GLCHECK( glShaderSource(idShaderFS, 1, &src, &len); );
    DEBUG_GLCHECK( glCompileShader(idShaderFS); );
    DEBUG_GLCHECK( glGetShaderiv(idShaderFS, GL_COMPILE_STATUS, &success); );
    if(!success){
        GLint logSize = 0;
        DEBUG_GLCHECK( glGetShaderiv(idShaderFS, GL_INFO_LOG_LENGTH, &logSize); );
        if(logSize){
            GLchar *infoLog = new GLchar[logSize];
            DEBUG_GLCHECK( glGetShaderInfoLog(idShaderFS, logSize, nullptr, infoLog); );
            LogError("\nFRAGMENT SHADER ERROR:\n%s\n", (const char*)infoLog);
            delete[] infoLog;
        }
        else{
            LogError("Could not get log size for fragment shader information!\n");
        }
        glDeleteShader(idShaderVS);
        return false;
    }

    // Create shader program
    this->id = glCreateProgram();
    DEBUG_GLCHECK( glAttachShader(this->id, idShaderVS); );
    if(useGeometryShader){
        DEBUG_GLCHECK( glAttachShader(this->id, idShaderGS); );
    }
    DEBUG_GLCHECK( glAttachShader(this->id, idShaderFS); );
    DEBUG_GLCHECK( glLinkProgram(this->id); );
    DEBUG_GLCHECK( glGetProgramiv(this->id, GL_LINK_STATUS, &success); );
    if(!success){
        GLint logSize = 0;
        DEBUG_GLCHECK( glGetProgramiv(this->id, GL_INFO_LOG_LENGTH, &logSize); );
        if(logSize){
            GLchar *infoLog = new GLchar[logSize];
            DEBUG_GLCHECK( glGetShaderInfoLog(this->id, logSize, nullptr, infoLog); );
            LogError("\nSHADER LINK ERROR:\n%s\n", (const char*)infoLog);
            delete[] infoLog;
        }
        else{
            LogError("Could not get log size for shader program information!\n");
        }
        glDeleteShader(idShaderVS);
        if(useGeometryShader){
            glDeleteShader(idShaderGS);
        }
        glDeleteShader(idShaderFS);
        return false;
    }

    // Flag attached shaders for deletion
    glDeleteShader(idShaderVS);
    if(useGeometryShader){
        glDeleteShader(idShaderGS);
    }
    glDeleteShader(idShaderFS);

    // Success
    return true;
}

void Shader::Delete(void){
    if(id){
        DEBUG_GLCHECK( glDeleteProgram(id); );
        id = 0;
    }
}

void Shader::Use(void){
    DEBUG_GLCHECK( glUseProgram(id); );
}

GLint Shader::GetUniformLocation(const GLchar* name){
    return glGetUniformLocation(this->id, name);
}

void Shader::UniformMatrix4fv(const GLchar* name, GLboolean transpose, glm::mat4& matrix){
    DEBUG_GLCHECK( glUniformMatrix4fv(glGetUniformLocation(this->id, name), 1, transpose, glm::value_ptr(matrix)); );
}

void Shader::UniformMatrix4fv(GLint location, GLboolean transpose, glm::mat4& matrix){
    DEBUG_GLCHECK( glUniformMatrix4fv(location, 1, transpose, glm::value_ptr(matrix)); );
}

void Shader::UniformMatrix4fv(const GLchar* name, GLboolean transpose, GLfloat* matrix) {
    DEBUG_GLCHECK( glUniformMatrix4fv(glGetUniformLocation(this->id, name), 1, transpose, matrix); );
}

void Shader::UniformMatrix4fv(GLint location, GLboolean transpose, GLfloat* matrix) {
    DEBUG_GLCHECK( glUniformMatrix4fv(location, 1, transpose, matrix); );
}

void Shader::UniformMatrix3fv(const GLchar* name, GLboolean transpose, glm::mat3& matrix){
    DEBUG_GLCHECK( glUniformMatrix3fv(glGetUniformLocation(this->id, name), 1, transpose, glm::value_ptr(matrix)); );
}

void Shader::UniformMatrix3fv(GLint location, GLboolean transpose, glm::mat3& matrix){
    DEBUG_GLCHECK( glUniformMatrix3fv(location, 1, transpose, glm::value_ptr(matrix)); );
}

void Shader::UniformMatrix3fv(const GLchar* name, GLboolean transpose, GLfloat* matrix) {
    DEBUG_GLCHECK( glUniformMatrix3fv(glGetUniformLocation(this->id, name), 1, transpose, matrix); );
}

void Shader::UniformMatrix3fv(GLint location, GLboolean transpose, GLfloat* matrix) {
    DEBUG_GLCHECK( glUniformMatrix3fv(location, 1, transpose, matrix); );
}

void Shader::UniformMatrix2fv(const GLchar* name, GLboolean transpose, glm::mat2& matrix){
    DEBUG_GLCHECK( glUniformMatrix2fv(glGetUniformLocation(this->id, name), 1, transpose, glm::value_ptr(matrix)); );
}

void Shader::UniformMatrix2fv(GLint location, GLboolean transpose, glm::mat2& matrix){
    DEBUG_GLCHECK( glUniformMatrix2fv(location, 1, transpose, glm::value_ptr(matrix)); );
}

void Shader::UniformMatrix2fv(const GLchar* name, GLboolean transpose, GLfloat* matrix) {
    DEBUG_GLCHECK( glUniformMatrix2fv(glGetUniformLocation(this->id, name), 1, transpose, matrix); );
}

void Shader::UniformMatrix2fv(GLint location, GLboolean transpose, GLfloat* matrix) {
    DEBUG_GLCHECK( glUniformMatrix2fv(location, 1, transpose, matrix); );
}

void Shader::Uniform4f(const GLchar* name, glm::vec4& value){
    DEBUG_GLCHECK( glUniform4f(glGetUniformLocation(this->id, name), value.x, value.y, value.z, value.w); );
}

void Shader::Uniform4f(GLint location, glm::vec4& value){
    DEBUG_GLCHECK( glUniform4f(location, value.x, value.y, value.z, value.w); );
}

void Shader::Uniform4fv(const GLchar* name, GLsizei count, const GLfloat* value){
    DEBUG_GLCHECK( glUniform4fv(glGetUniformLocation(this->id, name), count, value); );
}

void Shader::Uniform4fv(GLint location, GLsizei count, const GLfloat* value){
    DEBUG_GLCHECK( glUniform4fv(location, count, value); );
}

void Shader::Uniform3f(const GLchar* name, glm::vec3& value){
    DEBUG_GLCHECK( glUniform3f(glGetUniformLocation(this->id, name), value.x, value.y, value.z); );
}

void Shader::Uniform3f(GLint location, glm::vec3& value){
    DEBUG_GLCHECK( glUniform3f(location, value.x, value.y, value.z); );
}

void Shader::Uniform3fv(const GLchar* name, GLsizei count, const GLfloat* value){
    DEBUG_GLCHECK( glUniform3fv(glGetUniformLocation(this->id, name), count, value); );
}

void Shader::Uniform3fv(GLint location, GLsizei count, const GLfloat* value){
    DEBUG_GLCHECK( glUniform3fv(location, count, value); );
}

void Shader::Uniform2f(const GLchar* name, glm::vec2& value){
    DEBUG_GLCHECK( glUniform2f(glGetUniformLocation(this->id, name), value.x, value.y); );
}

void Shader::Uniform2f(GLint location, glm::vec2& value){
    DEBUG_GLCHECK( glUniform2f(location, value.x, value.y); );
}

void Shader::Uniform2fv(const GLchar* name, GLsizei count, const GLfloat* value){
    DEBUG_GLCHECK( glUniform2fv(glGetUniformLocation(this->id, name), count, value); );
}

void Shader::Uniform2fv(GLint location, GLsizei count, const GLfloat* value){
    DEBUG_GLCHECK( glUniform2fv(location, count, value); );
}

void Shader::Uniform1f(const GLchar* name, GLfloat value){
    DEBUG_GLCHECK( glUniform1f(glGetUniformLocation(this->id, name), value); );
}

void Shader::Uniform1f(GLint location, GLfloat value){
    DEBUG_GLCHECK( glUniform1f(location, value); );
}

void Shader::Uniform1fv(const GLchar* name, GLsizei count, const GLfloat* value){
    DEBUG_GLCHECK( glUniform1fv(glGetUniformLocation(this->id, name), count, value); );
}

void Shader::Uniform1fv(GLint location, GLsizei count, const GLfloat* value){
    DEBUG_GLCHECK( glUniform1fv(location, count, value); );
}

void Shader::Uniform4i(const GLchar* name, GLint value0, GLint value1, GLint value2, GLint value3){
    DEBUG_GLCHECK( glUniform4i(glGetUniformLocation(this->id, name), value0, value1, value2, value3); );
}

void Shader::Uniform4i(GLint location, GLint value0, GLint value1, GLint value2, GLint value3){
    DEBUG_GLCHECK( glUniform4i(location, value0, value1, value2, value3); );
}

void Shader::Uniform4iv(const GLchar* name, GLsizei count, const GLint* values){
    DEBUG_GLCHECK( glUniform4iv(glGetUniformLocation(this->id, name), count, values); );
}

void Shader::Uniform4iv(GLint location, GLsizei count, const GLint* values){
    DEBUG_GLCHECK( glUniform4iv(location, count, values); );
}

void Shader::Uniform3i(const GLchar* name, GLint value0, GLint value1, GLint value2){
    DEBUG_GLCHECK( glUniform3i(glGetUniformLocation(this->id, name), value0, value1, value2); );
}

void Shader::Uniform3i(GLint location, GLint value0, GLint value1, GLint value2){
    DEBUG_GLCHECK( glUniform3i(location, value0, value1, value2); );
}

void Shader::Uniform3iv(const GLchar* name, GLsizei count, const GLint* values){
    DEBUG_GLCHECK( glUniform3iv(glGetUniformLocation(this->id, name), count, values); );
}

void Shader::Uniform3iv(GLint location, GLsizei count, const GLint* values){
    DEBUG_GLCHECK( glUniform3iv(location, count, values); );
}

void Shader::Uniform2i(const GLchar* name, GLint value0, GLint value1){
    DEBUG_GLCHECK( glUniform2i(glGetUniformLocation(this->id, name), value0, value1); );
}

void Shader::Uniform2i(GLint location, GLint value0, GLint value1){
    DEBUG_GLCHECK( glUniform2i(location, value0, value1); );
}

void Shader::Uniform2iv(const GLchar* name, GLsizei count, const GLint* values){
    DEBUG_GLCHECK( glUniform2iv(glGetUniformLocation(this->id, name), count, values); );
}

void Shader::Uniform2iv(GLint location, GLsizei count, const GLint* values){
    DEBUG_GLCHECK( glUniform2iv(location, count, values); );
}

void Shader::Uniform1i(const GLchar* name, GLint value){
    DEBUG_GLCHECK( glUniform1i(glGetUniformLocation(this->id, name), value); );
}

void Shader::Uniform1i(GLint location, GLint value){
    DEBUG_GLCHECK( glUniform1i(location, value); );
}

void Shader::Uniform1iv(const GLchar* name, GLsizei count, const GLint* values){
    DEBUG_GLCHECK( glUniform1iv(glGetUniformLocation(this->id, name), count, values); );
}

void Shader::Uniform1iv(GLint location, GLsizei count, const GLint* values){
    DEBUG_GLCHECK( glUniform1iv(location, count, values); );
}

void Shader::UniformBlockBinding(const GLchar* name, GLuint value){
    DEBUG_GLCHECK( glUniformBlockBinding(this->id, glGetUniformBlockIndex(this->id, name), value); );
}

void Shader::UniformBlockBinding(GLint location, GLuint value){
    DEBUG_GLCHECK( glUniformBlockBinding(this->id, location, value); );
}

