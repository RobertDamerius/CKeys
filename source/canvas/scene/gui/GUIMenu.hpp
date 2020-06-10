#pragma once


#include <nanogui.h>
#include <WidgetPerformanceInfo.hpp>


class GUIMenu {
    public:
        /**
         *  @brief Create a GUI menu.
         */
        GUIMenu();

        /**
         *  @brief Initialize the GUI menu.
         *  @param wnd The GLFW window.
         *  @return True if success, false otherwise.
         */
        bool Initialize(GLFWwindow* wnd);

        /**
         *  @brief Terminate the GUI menu.
         */
        void Terminate(void);

        /**
         *  @brief Render callback function.
         */
        void Render(void);

        /**
         *  @brief Check if mouse cursor is over GUI menu.
         *  @param [in] cursor Mouse cursor position.
         *  @return True if mouse cursor is over GUI menu, false otherwise.
         */
        bool CursorOverGUI(glm::dvec2 cursor);

        /**
         *  @brief Check if any GUI element is focused.
         *  @return True if at least one GUI element is focused.
         */
        bool IsFocused(void);

        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Callback functions
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        /**
         *  @brief Framebuffer resize callback.
         *  @param [in] wnd The GLFW window.
         *  @param [in] size Framebuffer size in pixels.
         */
        void CallbackFramebufferResize(GLFWwindow* wnd, int width, int height);

        /**
         *  @brief The key input callback function.
         *  @param [in] wnd The GLFW window.
         *  @param [in] key The keyboard key that was pressed or released.
         *  @param [in] scancode The system-specific scancode of the key.
         *  @param [in] action GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT.
         *  @param [in] mods Bit field describing which modifier keys were held down.
         */
        void CallbackKey(GLFWwindow* wnd, int key, int scancode, int action, int mods);

        /**
         *  @brief The char input callback function.
         *  @param [in] wnd The GLFW window.
         *  @param [in] codepoint The Unicode code point of the character.
         */
        void CallbackChar(GLFWwindow* wnd, unsigned codepoint);

        /**
         *  @brief Mouse button callback function.
         *  @param [in] wnd The GLFW window.
         *  @param [in] button The GLFW mouse button that was pressed or released.
         *  @param [in] action GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT.
         *  @param [in] mods Bit field describing which modifier keys were held down.
         */
        void CallbackMouseButton(GLFWwindow* wnd, int button, int action, int mods);

        /**
         *  @brief Mouse cursor callback function.
         *  @param [in] wnd The GLFW window.
         *  @param [in] xpos X position of the cursor.
         *  @param [in] ypos Y position of the cursor.
         */
        void CallbackCursorPosition(GLFWwindow* wnd, double xpos, double ypos);

        /**double xpos, double ypos
         *  @brief Mouse scroll callback function.
         *  @param [in] wnd The GLFW window.
         *  @param [in] xoffset Scroll in x direction.
         *  @param [in] yoffset Scroll in y direction.
         */
        void CallbackScroll(GLFWwindow* wnd, double xoffset, double yoffset);

    private:
        nanogui::Screen* screen;
        WidgetPerformanceInfo* performanceInfo;
};

