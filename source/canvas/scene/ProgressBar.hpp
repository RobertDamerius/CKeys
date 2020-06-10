#pragma once


#include <nanovg/nanovg_gl.h>


class ProgressBar {
    public:
        glm::vec2 position;   ///< Position of the progress bar in pixels (y from top to bottom).
        glm::vec2 dimension;  ///< Dimension of the progress bar in pixels.

        /**
         *  @brief Create a progress bar.
         */
        ProgressBar();

        /**
         *  @brief Draw the progress bar.
         *  @param [in] vg Vector-graphic context.
         */
        void Draw(NVGcontext* vg);

        /**
         *  @brief Resize the progress bar.
         *  @param [in] wnd GLFW window.
         *  @param [in] lowerBound Lower bound in pixels of the widget that contains this keyboard.
         *  @param [in] upperBound Upper bound in pixels of the widget that contains this keyboard.
         */
        void Resize(GLFWwindow* wnd, glm::ivec2 lowerBound, glm::ivec2 upperBound);

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
         *  @brief Get the time scroll mode.
         *  @return True if time scroll mode is enabled.
         */
        inline bool GetTimeScrollMode(void){ return timeScrollMode; }

    private:
        /* Mouse button attributes */
        bool mouseHover;        ///< True if mouse cursor is over progress bar.
        bool timeScrollMode;    ///< True if time scroll mode is enabled.
        bool playState;         ///< True if stream was playing during left mouse button action.

        /* Resize attributes */
        float padding;          ///< Padding around progress bar in pixels.
        float edge;             ///< Edge dimension in pixels.
        float radius;           ///< Radius of rounded rect in pixels.
};

