#pragma once


#include <Scene.hpp>
#include <Renderer.hpp>


class Canvas {
    public:
        Scene scene;         ///< The scene that contains all the content to be rendered.
        Renderer renderer;   ///< The renderer that renders a frame of the scene.

        /**
         *  @brief Initialize the canvas.
         *  @param [in] wnd GLFW window.
         *  @return True if success, false otherwise.
         */
        bool Initialize(GLFWwindow* wnd);

        /**
         *  @brief Terminate the canvas.
         *  @param [in] wnd GLFW window.
         */
        void Terminate(GLFWwindow* wnd);

        /**
         *  @brief Render the canvas.
         *  @param [in] wnd GLFW window.
         *  @param [in] dt Elapsed time to previous rendering event.
         */
        void Render(GLFWwindow* wnd, double dt);

        /**
         *  @brief Resize the canvas.
         *  @param [in] wnd GLFW window.
         *  @param [in] width New framebuffer width in pixels.
         *  @param [in] height New framebuffer height in pixels.
         */
        void Resize(GLFWwindow* wnd, int width, int height);

        /**
         *  @brief Drop files callback function.
         *  @param [in] wnd GLFW window.
         *  @param [in] count Number of files.
         *  @param [in] paths Array of filenames.
         */
        void DropFiles(GLFWwindow* wnd, int count, const char** paths);

        /**
         *  @brief Key callback function.
         *  @param [in] wnd GLFW window.
         *  @param [in] key The keyboard key that was pressed or released.
         *  @param [in] scancode The system-specific scancode of the key.
         *  @param [in] action GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT. Future releases may add more actions.
         *  @param [in] mods Bit field describing which modifier keys were held down.
         */
        void CallbackKey(GLFWwindow* wnd, int key, int scancode, int action, int mods);

        /**
         *  @brief Cursor position callback function.
         *  @param [in] wnd GLFW window.
         *  @param [in] xpos X position of the cursor.
         *  @param [in] ypos Y position of the cursor.
         */
        void CallbackCursorPosition(GLFWwindow* wnd, double xpos, double ypos);

        /**
         *  @brief Mouse button callback function.
         *  @param [in] wnd GLFW window.
         *  @param [in] button The mouse button that was pressed or released.
         *  @param [in] action GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT. Future releases may add more actions.
         *  @param [in] mods Bit field describing which modifier keys were held down.
         */
        void CallbackMouseButton(GLFWwindow* wnd, int button, int action, int mods);

        /**
         *  @brief Character callback function.
         *  @param [in] wnd GLFW window.
         *  @param [in] codepoint The Unicode code point of the character.
         */
        void CallbackChar(GLFWwindow* wnd, unsigned codepoint);

        /**
         *  @brief Mouse scroll callback function.
         *  @param [in] wnd GLFW window.
         *  @param [in] xoffset Scroll in x direction.
         *  @param [in] yoffset Scroll in y direction.
         */
        void CallbackScroll(GLFWwindow* wnd, double xoffset, double yoffset);
};

