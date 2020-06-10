#pragma once


#include <PerformanceScene.hpp>
#include <RecordingScene.hpp>
#include <nanovg/nanovg_gl.h>
#include <GUIMenu.hpp>


enum SceneMode {
    SCENE_MODE_PERFORMANCE,
    SCENE_MODE_RECORDING
};


class Scene {
    public:
        SceneMode sceneMode;           ///< The current scene mode.
        PerformanceScene performance;  ///< The performance scene including musical keyboard and lanes.
        RecordingScene recording;      ///< The recording scene including musical keyboard and lanes.
        GUIMenu menu;                  ///< The GUI menu.

        /**
         *  @brief Create a scene object.
         */
        Scene();

        /**
         *  @brief Initialize the scene.
         *  @param [in] wnd GLFW window.
         *  @return True if success, false otherwise.
         */
        bool Initialize(GLFWwindow* wnd);

        /**
         *  @brief Terminate the scene.
         *  @param [in] wnd GLFW window.
         */
        void Terminate(GLFWwindow* wnd);

        /**
         *  @brief Update the scene.
         *  @param [in] wnd GLFW window.
         *  @param [in] dt Elapsed time to previous rendering event.
         */
        void Update(GLFWwindow* wnd, double dt);

        /**
         *  @brief Draw the scene.
         *  @param [in] wnd GLFW window.
         */
        void Draw(GLFWwindow* wnd);

        /**
         *  @brief Resize the scene.
         *  @param [in] wnd GLFW window.
         *  @param [in] width New framebuffer width in pixels.
         *  @param [in] height New framebuffer height in pixels.
         */
        void Resize(GLFWwindow* wnd, int width, int height);

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

    private:
        NVGcontext* ctxVG;  ///< The vector-graphic context.
};

