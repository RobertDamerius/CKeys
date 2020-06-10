#pragma once


#include <Canvas.hpp>
#include <Sequencer.hpp>


class MainWindow {
    public:
        static Canvas canvas;             ///< The canvas object that contains all the actual data.

        /**
         *  @brief Initialize the main window.
         *  @return True if success, false otherwise.
         */
        static bool Initialize(void);

        /**
         *  @brief Terminate the main window.
         */
        static void Terminate(void);

        /**
         *  @brief The main loop.
         */
        static void MainLoop(void);

        /**
         *  @brief Draw a normalized rect: [-1,-1] to [+1,+1].
         */
        static void DrawNormalizedRect(void);

    protected:
        /* Window callbacks (are forwarded to the @ref canvas) */
        static void CallbackFramebufferSize(GLFWwindow* wnd, int width, int height){ canvas.Resize(wnd, width, height); }
        static void CallbackDrop(GLFWwindow* wnd, int count, const char** paths){ canvas.DropFiles(wnd, count, paths); }
        static void CallbackKey(GLFWwindow* wnd, int key, int scancode, int action, int mods){ canvas.CallbackKey(wnd, key, scancode, action, mods); }
        static void CallbackCursorPosition(GLFWwindow* wnd, double xpos, double ypos){ canvas.CallbackCursorPosition(wnd, xpos, ypos); }
        static void CallbackMouseButton(GLFWwindow* wnd, int button, int action, int mods){ canvas.CallbackMouseButton(wnd, button, action, mods); }
        static void CallbackChar(GLFWwindow* wnd, unsigned codepoint){ canvas.CallbackChar(wnd, codepoint); }
        static void CallbackScroll(GLFWwindow* wnd, double xoffset, double yoffset){ canvas.CallbackScroll(wnd, xoffset, yoffset); }

    private:
        static GLFWwindow* glfwWindow;    ///< The GLFW window or zero if not initialized.
        static GLuint rectVAO;            ///< VAO for normalized rectangle.
        static GLuint rectVBO;            ///< VBO for normalized rectangle.
};

