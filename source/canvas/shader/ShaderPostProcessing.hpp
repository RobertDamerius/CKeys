#pragma once


#include <Shader.hpp>


class ShaderPostProcessing: protected Shader {
    public:
        using Shader::Use;
        using Shader::Delete;

        /**
         *  @brief Create post processing shader.
         */
        ShaderPostProcessing();

        /**
         *  @brief Delete post processing shader.
         */
        ~ShaderPostProcessing();

        /**
         *  @brief Generate the shader.
         *  @return True if success, false otherwise.
         */
        bool Generate(void);

        /**
         *  @brief Set color scale.
         *  @param [in] color The color scale to be set, defaults to glm::vec3(1.0f).
         */
        void SetColorScale(glm::vec3 color = glm::vec3(1.0f));

    private:
        GLint locationColorScale;
};

