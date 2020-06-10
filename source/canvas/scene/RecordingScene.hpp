#pragma once


#include <MusicalKeyboard.hpp>
#include <LaneManager.hpp>
#include <Recorder.hpp>
#include <nanovg/nanovg_gl.h>


class RecordingScene {
    public:
        MusicalKeyboard keyboard;      ///< Musical keyboard visualization.
        LaneManager laneManager;       ///< Lane visualization.
        Recorder recorder;             ///< The MIDI recorder.

        /**
         *  @brief Resize the recording scene.
         *  @param [in] wnd GLFW window.
         *  @param [in] width New framebuffer width in pixels.
         *  @param [in] height New framebuffer height in pixels.
         */
        void Resize(GLFWwindow* wnd, int width, int height);

        /**
         *  @brief Draw the recording scene.
         *  @param [in] vg Vector-graphic context.
         */
        void Draw(NVGcontext* vg);

        /**
         *  @brief Key callback function.
         *  @param [in] wnd GLFW window.
         *  @param [in] key The keyboard key that was pressed or released.
         *  @param [in] scancode The system-specific scancode of the key.
         *  @param [in] action GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT. Future releases may add more actions.
         *  @param [in] mods Bit field describing which modifier keys were held down.
         */
        void CallbackKey(GLFWwindow* wnd, int key, int scancode, int action, int mods);
};

