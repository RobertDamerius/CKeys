#pragma once


#include <Sequencer.hpp>
#include <nanovg/nanovg_gl.h>


class Key {
    public:
        static const int idxBlack[36]; ///< Indices to black keys.
        static const int idxWhite[52]; ///< Indices to white keys.

        double x;           ///< X position in pixels, this value is set if the keyboard is resized.
        bool pressed;       ///< True if key is pressed, false otherwise.
        glm::u8vec3 color;  ///< Color of pressed key.
};


class MusicalKeyboard {
    public:
        std::array<Key, 88> keys;      ///< Array of keys starting with A0.

        /**
         *  @brief Draw the musical keyboard.
         *  @param [in] vg Vector-graphic context.
         */
        void Draw(NVGcontext* vg);

        /**
         *  @brief Resize the keyboard.
         *  @param [in] wnd GLFW window.
         *  @param [in] lowerBound Lower bound in pixels of the widget that contains this keyboard.
         *  @param [in] upperBound Upper bound in pixels of the widget that contains this keyboard.
         *  @param [in] align Keyboard alignment: 0=bottom, 1=top. Defaults to 0.
         */
        void Resize(GLFWwindow* wnd, glm::ivec2 lowerBound, glm::ivec2 upperBound, int align = 0);

        /**
         *  @brief Get current layout attributes.
         *  @param [out] position Position of overall keyboard in pixels (y from top to bottom).
         *  @param [out] dimension Dimension of overall keyboard in pixels.
         *  @param [out] whiteKeyDimension Dimension of a white key in pixels.
         *  @param [out] blackKeyDimension Dimension of a black key in pixels.
         *  @param [out] keySpacing Spacing between keys in pixels.
         */
        inline void GetLayout(glm::vec2& position, glm::vec2& dimension, glm::vec2& whiteKeyDimension, glm::vec2& blackKeyDimension, float& keySpacing){
            position = this->position;
            dimension = this->dimension;
            whiteKeyDimension = this->whiteKeyDimension;
            blackKeyDimension = this->blackKeyDimension;
            keySpacing = this->keySpacing;
        }

    private:
        /* Layout parameters (are set during a @ref Resize call) */
        glm::vec2 position;                 ///< The position of the total keyboard in pixels (y from top to bottom).
        glm::vec2 dimension;                ///< The dimension of the total keyboard in pixels.
        float topEdgeHeight;                ///< Height of top edge of keyboard.
        float keySpacing;                   ///< Number of pixels for key spacing.
        float keyPositionY;                 ///< Y-position of keys (y from top to bottom).
        float whiteKeyRadius;               ///< Radius of white keys.
        float keyEdge;                      ///< Edge size in pixels.
        glm::vec2 whiteKeyDimension;        ///< Dimension of a white key in pixels.
        glm::vec2 blackKeyDimension;        ///< Dimension of a black key in pixels.
        glm::vec2 blackKeyShadowGradient;   ///< Shadow gradient vector in pixels for black keys.
};

