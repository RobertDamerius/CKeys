#pragma once


#include <MusicalKeyboard.hpp>
#include <LaneManager.hpp>
#include <Sequencer.hpp>
#include <ProgressBar.hpp>
#include <nanovg/nanovg_gl.h>


class PerformanceScene {
    public:
        MusicalKeyboard keyboard;      ///< Musical keyboard visualization.
        LaneManager laneManager;       ///< Lane visualization.
        Sequencer sequencer;           ///< The sequencer which contains the data of the whole performance.
        ProgressBar progressBar;       ///< The progress bar.

        /**
         *  @brief Resize the performance scene.
         *  @param [in] wnd GLFW window.
         *  @param [in] width New framebuffer width in pixels.
         *  @param [in] height New framebuffer height in pixels.
         */
        void Resize(GLFWwindow* wnd, int width, int height);

        /**
         *  @brief Draw the performance scene.
         *  @param [in] vg Vector-graphic context.
         */
        void Draw(NVGcontext* vg);

        /**
         *  @brief Load the performance from a MIDI file.
         *  @param [in] wnd GLFW window.
         *  @param [in] filename Name of the MIDI file.
         */
        void Load(GLFWwindow* wnd, std::string filename);

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
};

