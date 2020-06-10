#pragma once


#include <FrameBufferGUI.hpp>
#include <ShaderPostProcessing.hpp>
#include <Scene.hpp>


class Renderer {
    public:
        /**
         *  @brief Initialize the renderer.
         *  @param [in] wnd GLFW window.
         *  @return True if success, false otherwise.
         */
        bool Initialize(GLFWwindow* wnd);

        /**
         *  @brief Terminate the renderer.
         *  @param [in] wnd GLFW window.
         */
        void Terminate(GLFWwindow* wnd);

        /**
         *  @brief Render one frame.
         *  @param [in] wnd GLFW window.
         *  @param [in] scene Reference to the scene that should be rendered.
         */
        void RenderFrame(GLFWwindow* wnd, Scene& scene);

        /**
         *  @brief Resize the scene.
         *  @param [in] wnd GLFW window.
         *  @param [in] width New framebuffer width in pixels.
         *  @param [in] height New framebuffer height in pixels.
         */
        void Resize(GLFWwindow* wnd, int width, int height);

        /**
         *  @brief Set color scale for post processing.
         *  @param [in] color The color scale value to be set.
         */
        inline void SetPostProcessingColorScale(glm::vec3 color){
            shaderPostProcessing.Use();
            shaderPostProcessing.SetColorScale(color);
        }

    private:
        /* Framebuffers */
        FrameBufferGUI fbGUI;     ///< The GUI framebuffer.

        /* Shaders */
        ShaderPostProcessing shaderPostProcessing;          ///< The post processing shader.

        /**
         *  @brief Generate all framebuffers.
         *  @param [in] width Framebuffer width in pixels.
         *  @param [in] height Framebuffer height in pixels.
         *  @return True if success, false otherwise.
         */
        bool GenerateFrameBuffers(int width, int height);

        /**
         *  @brief Delete all framebuffers.
         */
        void DeleteFrameBuffers(void);

        /**
         *  @brief Generate shaders.
         *  @return True if success, false otherwise.
         */
        bool GenerateShaders(void);

        /**
         *  @brief Delete all shaders.
         */
        void DeleteShaders(void);
};

