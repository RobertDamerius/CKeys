#pragma once


class NoteBlock {
    public:
        double velocity;   ///< Normalized velocity in range [0.0, 1.0] (this is the velocity of a note-on event).
        double on;         ///< Time in seconds when note is on.
        double off;        ///< Time in seconds when note is off. This is used for graphical rendering.
        double sustainOff; ///< Time in seconds when sustained note is off. This is used for audio rendering.

        /**
         *  @brief Create a note block.
         *  @param [in] velocity Normalized velocity in range [0.0, 1.0], defaults to 1.0.
         *  @param [in] on Time in seconds when note is on, defaults to 0.0.
         *  @param [in] off Time in seconds when note is off, defaults to infinity.
         */
        NoteBlock(double velocity = 1.0, double on = 0.0, double off = std::numeric_limits<double>::infinity()):velocity(velocity), on(on), off(off), sustainOff(off){}
};

