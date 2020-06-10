#pragma once


#include <MusicalKeyboard.hpp>
#include <Sequencer.hpp>
#include <Recorder.hpp>
#include <nanovg/nanovg_gl.h>


class Lane {
    public:
        double x;          ///< X position in pixels, this value is set if the lane manager is resized.
        double w;          ///< Width of the lane in pixels, this value is set if the lane manager is resized.
        glm::u8vec3 color; ///< Background color of this lane.
};


class LaneManager {
    public:
        std::array<Lane, 88> lanes; ///< Lanes for all 88 keys starting with A0.
        double timeHorizon;         ///< Time horizon to be used (in seconds). Default value is 3.0.

        /**
         *  @brief Create a lane manager.
         */
        LaneManager();

        /**
         *  @brief Draw the lanes.
         *  @param [in] vg Vector-graphic context.
         *  @param [in] sequencer The sequencer that contains the data of the performance.
         *  @param [in] timePointer Current time point of the performance in seconds.
         *  @param [in] keyboard The keyboard that should be updated.
         */
        void Draw(NVGcontext* vg, Sequencer& sequencer, double timePointer, MusicalKeyboard& keyboard);

        /**
         *  @brief Draw the lanes.
         *  @param [in] vg Vector-graphic context.
         *  @param [in] recorder MIDI recorder.
         *  @param [in] keyboard The keyboard that should be updated.
         */
        void Draw(NVGcontext* vg, Recorder& recorder, MusicalKeyboard& keyboard);

        /**
         *  @brief Resize the lane manager.
         *  @param [in] wnd GLFW window.
         *  @param [in] lowerBound Lower bound in pixels of the widget that contains this keyboard.
         *  @param [in] upperBound Upper bound in pixels of the widget that contains this keyboard.
         *  @param [in] keyboard The musical keyboard that has already been resized and is used as reference.
         *  @param [in] align Keyboard alignment: 0=bottom, 1=top. Defaults to 0.
         */
        void Resize(GLFWwindow* wnd, glm::ivec2 lowerBound, glm::ivec2 upperBound, MusicalKeyboard& keyboard, int align = 0);

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
        double yCursorBegin;    ///< Y cursor position at begin of time scroll mode.
        double timeBegin;       ///< Time pointer at begin of time scroll mode.
        double y0;              ///< Begin of y-position during recording mode.

        /* Layout parameters (are set during a @ref Resize call) */
        glm::vec2 position;     ///< The position of the total lane field in pixels (y from top to bottom).
        glm::vec2 dimension;    ///< The dimension of the total lane field in pixels.
        float noteRadius;       ///< Radius for rounded rect of note block in pixels.
        float edgeSize;         ///< Edge size for rounded rect of note block in pixels.
};

