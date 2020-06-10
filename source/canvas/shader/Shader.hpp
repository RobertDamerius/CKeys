#pragma once


/**
 *  @brief Class: Shader
 *  @details Handles vertex + geometry (optional) + fragment shader.
 */
class Shader {
    public:
        /**
         *  @brief Create a shader object.
         */
        Shader();

        /**
         *  @brief Delete the shader object.
         *  @details The destructor does NOT call the @ref Delete member function!
         */
        virtual ~Shader();

        /**
         *  @brief Get the GLSL-version string to be used to generate shader.
         *  @return The version string, e.g. "450".
         *  @details Make sure that the OpenGL context is initialized to obtain correct version information.
         */
        static std::string GetShadingLanguageVersion(void);

        /**
         *  @brief Generate the shader.
         *  @param [in] fileName The filename of the GLSL shader file.
         *  @return True if success, false otherwise.
         *  @details A zero terminator will be added to the shader source. The GLSL version will be assigned automatically using the @ref GetShadingLanguageVersion function.
         */
        bool Generate(std::string fileName);

        /**
         *  @brief Generate the shader.
         *  @param [in] fileName The filename of the GLSL shader file.
         *  @param [in] version The version string number, e.g. "450".
         *  @return True if success, false otherwise.
         *  @details A zero terminator will be added to the shader source.
         */
        bool Generate(std::string fileName, std::string version);

        /**
         *  @brief Generate the shader.
         *  @param [in] fileName The filename of the GLSL shader file.
         *  @param [in] version The version string number, e.g. "450".
         *  @param [in] replacement Text replacement data.
         *  @return True if success, false otherwise.
         *  @details A zero terminator will be added to the shader source.
         */
        bool Generate(std::string fileName, std::string version, std::vector<std::pair<std::string, std::string>>& replacement);

        /**
         *  @brief Generate the shader.
         *  @param [in] fileData Binary file data.
         *  @param [in] version The version string number, e.g. "450".
         *  @return True if success, false otherwise.
         *  @details A zero terminator will be added to the shader source.
         */
        bool Generate(std::vector<uint8_t>& fileData, std::string version);

        /**
         *  @brief Generate the shader.
         *  @param [in] fileData Binary file data.
         *  @param [in] version The version string number, e.g. "450".
         *  @param [in] replacement Text replacement data.
         *  @return True if success, false otherwise.
         *  @details A zero terminator will be added to the shader source.
         */
        bool Generate(std::vector<uint8_t>& fileData, std::string version, std::vector<std::pair<std::string, std::string>>& replacement);

        /**
         *  @brief Delete the shader program.
         */
        void Delete(void);

        /**
         *  @brief Use the shader.
         */
        void Use(void);

        /* The uniforms */
        void UniformMatrix4fv(const GLchar* name, GLboolean transpose, glm::mat4& matrix);
        void UniformMatrix4fv(GLint location, GLboolean transpose, glm::mat4& matrix);
        void UniformMatrix4fv(const GLchar* name, GLboolean transpose, GLfloat* matrix);
        void UniformMatrix4fv(GLint location, GLboolean transpose, GLfloat* matrix);

        void UniformMatrix3fv(const GLchar* name, GLboolean transpose, glm::mat3& matrix);
        void UniformMatrix3fv(GLint location, GLboolean transpose, glm::mat3& matrix);
        void UniformMatrix3fv(const GLchar* name, GLboolean transpose, GLfloat* matrix);
        void UniformMatrix3fv(GLint location, GLboolean transpose, GLfloat* matrix);

        void UniformMatrix2fv(const GLchar* name, GLboolean transpose, glm::mat2& matrix);
        void UniformMatrix2fv(GLint location, GLboolean transpose, glm::mat2& matrix);
        void UniformMatrix2fv(const GLchar* name, GLboolean transpose, GLfloat* matrix);
        void UniformMatrix2fv(GLint location, GLboolean transpose, GLfloat* matrix);

        void Uniform4f(const GLchar* name, glm::vec4& value);
        void Uniform4f(GLint location, glm::vec4& value);
        void Uniform4fv(const GLchar* name, GLsizei count, const GLfloat* value);
        void Uniform4fv(GLint location, GLsizei count, const GLfloat* value);

        void Uniform3f(const GLchar* name, glm::vec3& value);
        void Uniform3f(GLint location, glm::vec3& value);
        void Uniform3fv(const GLchar* name, GLsizei count, const GLfloat* value);
        void Uniform3fv(GLint location, GLsizei count, const GLfloat* value);

        void Uniform2f(const GLchar* name, glm::vec2& value);
        void Uniform2f(GLint location, glm::vec2& value);
        void Uniform2fv(const GLchar* name, GLsizei count, const GLfloat* value);
        void Uniform2fv(GLint location, GLsizei count, const GLfloat* value);

        void Uniform1f(const GLchar* name, GLfloat value);
        void Uniform1f(GLint location, GLfloat value);
        void Uniform1fv(const GLchar* name, GLsizei count, const GLfloat* value);
        void Uniform1fv(GLint location, GLsizei count, const GLfloat* value);

        void Uniform4i(const GLchar* name, GLint value0, GLint value1, GLint value2, GLint value3);
        void Uniform4i(GLint location, GLint value0, GLint value1, GLint value2, GLint value3);
        void Uniform4iv(const GLchar* name, GLsizei count, const GLint* values);
        void Uniform4iv(GLint location, GLsizei count, const GLint* values);

        void Uniform3i(const GLchar* name, GLint value0, GLint value1, GLint value2);
        void Uniform3i(GLint location, GLint value0, GLint value1, GLint value2);
        void Uniform3iv(const GLchar* name, GLsizei count, const GLint* values);
        void Uniform3iv(GLint location, GLsizei count, const GLint* values);

        void Uniform2i(const GLchar* name, GLint value0, GLint value1);
        void Uniform2i(GLint location, GLint value0, GLint value1);
        void Uniform2iv(const GLchar* name, GLsizei count, const GLint* values);
        void Uniform2iv(GLint location, GLsizei count, const GLint* values);

        void Uniform1i(const GLchar* name, GLint value);
        void Uniform1i(GLint location, GLint value);
        void Uniform1iv(const GLchar* name, GLsizei count, const GLint* values);
        void Uniform1iv(GLint location, GLsizei count, const GLint* values);

        void UniformBlockBinding(const GLchar* name, GLuint value);
        void UniformBlockBinding(GLint location, GLuint value);

        /* Additional member functions */
        GLint GetUniformLocation(const GLchar* name);

    private:
        GLuint id; ///< The program ID.

};

