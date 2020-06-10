#include <MusicalKeyboard.hpp>


#define KEY_WHITE_RATIO          (6.4)                 // (height / width) of a white key
#define KEY_BLACK_RATIO          (6.6)                 // (height / width) of a black key
#define KEY_RATIO_BLACK_WHITE    (0.65)                // (width of black / width of white)
#define KEY_SPACING_RATIO        (0.001)               // Key spacing with respect to total keyboard width
#define KEY_EDGE_RATIO           (0.001)               // Key edge size with respect to total keyboard width
#define KEY_WHITE_RATIO_RADIUS   (0.15)                // Size of rounded rect radius related to key width for white keys
#define KEY_MARGIN_BOTTOM_PX     (6.0)                 // Number of pixels for bottom margin
#define RATIO_TOP_EDGE_HEIGHT    (0.2)                 // Height of top edge related to width of white key
#define BLACK_KEY_SHIFT_B        (0.23076923076923077) // Shift of black b-key to the left as factor of black key width
#define BLACK_KEY_SHIFT_DB       (0.69230769230769230) // Shift of black Db-key to the left as factor of black key width
#define BLACK_KEY_SHIFT_EB       (0.30769230769230770) // Shift of black Eb-key to the left as factor of black key width
#define BLACK_KEY_SHIFT_GB       (0.76923076923076923) // Shift of black Gb-key to the left as factor of black key width
#define BLACK_KEY_SHIFT_AB       (0.5)                 // Shift of black Ab-key to the left as factor of black key width
#define BLACK_KEY_SHADOW_WIDTH   (0.5)                 // Factor for length of shadow width related to black key width


const int Key::idxBlack[] = {  1,    4,  6,    9,   11,   13,      16,   18,      21,   23,   25,      28,   30,      33,   35,   37,      40,   42,      45,   47,   49,      52,   54,      57,   59,   61,      64,   66,      69,   71,   73,      76,   78,      81,   83,   85,     };
const int Key::idxWhite[] = {0,  2,3,  5,  7,8,  10,   12,   14,15,   17,   19,20,   22,   24,   26,27,   29,   31,32,   34,   36,   38,39,   41,   43,44,   46,   48,   50,51,   53,   55,56,   58,   60,   62,63,   65,   67,68,   70,   72,   74,75,   77,   79,80,   82,   84,   86,87};


void MusicalKeyboard::Draw(NVGcontext* vg){
    // Background
    nvgBeginPath(vg);
    nvgRect(vg, position.x, position.y, dimension.x, dimension.y);
    nvgFillColor(vg, nvgRGBA(10,10,10,255));
    nvgFill(vg);

    // White keys
    float sw = (whiteKeyDimension.x - keyEdge)*0.4f;
    for(int i = 0; i < 52; i++){
        // Calculate final key color
        NVGcolor clr1 = nvgRGBA(255,255,255,255);
        NVGcolor clr2 = nvgRGBA(248,244,240,255);
        if(keys[Key::idxWhite[i]].pressed){
            clr1 = nvgRGBA(keys[Key::idxWhite[i]].color.r, keys[Key::idxWhite[i]].color.g, keys[Key::idxWhite[i]].color.b, 255);
            clr2 = nvgRGBA((uint8_t)(double(keys[Key::idxWhite[i]].color.r) / 255.0 * 248.0), (uint8_t)(double(keys[Key::idxWhite[i]].color.g) / 255.0 * 244.0), (uint8_t)(double(keys[Key::idxWhite[i]].color.b) / 255.0 * 240), 255);
        }

        // white border
        nvgBeginPath(vg);
        nvgRoundedRectVarying(vg, keys[Key::idxWhite[i]].x, keyPositionY, whiteKeyDimension.x, whiteKeyDimension.y, 0.0f, 0.0f, whiteKeyRadius, whiteKeyRadius);
        nvgFillColor(vg, clr1);
        nvgFill(vg);

        // nearly white main color
        nvgBeginPath(vg);
        nvgRoundedRectVarying(vg, keys[Key::idxWhite[i]].x, keyPositionY, whiteKeyDimension.x - keyEdge, whiteKeyDimension.y - keyEdge, 0.0f, 0.0f, whiteKeyRadius, whiteKeyRadius - keyEdge);
        nvgFillColor(vg, clr2);
        nvgFill(vg);

        // Shadow of neighbor white key
        int n = i - 1;
        if(keys[Key::idxWhite[i]].pressed && !((n >= 0) && keys[Key::idxWhite[n]].pressed)){
            nvgBeginPath(vg);
            nvgRoundedRectVarying(vg, keys[Key::idxWhite[i]].x, keyPositionY, sw, whiteKeyDimension.y, 0.0f, 0.0f, whiteKeyRadius, whiteKeyRadius);
            NVGpaint gradient = nvgLinearGradient(vg, keys[Key::idxWhite[i]].x, keyPositionY, keys[Key::idxWhite[i]].x + sw, keyPositionY, nvgRGBA(0,0,0,80), nvgRGBA(0,0,0,0));
            nvgFillPaint(vg, gradient);
            nvgFill(vg);
        }
    }

    // Black keys
    float ks2 = keySpacing + keySpacing;
    float ke2 = keyEdge + keyEdge;
    float r = 0.25f * blackKeyDimension.x;
    sw = (float)BLACK_KEY_SHADOW_WIDTH * blackKeyDimension.x;
    for(int i = 0; i < 36; i++){
        // Calculate final key color
        NVGcolor clr1 = nvgRGBA(45,45,45,255);
        NVGcolor clr2 = nvgRGBA(15,15,15,255);
        float f = blackKeyDimension.x;
        if(keys[Key::idxBlack[i]].pressed){
            f *= 0.7f;
            clr1 = nvgRGBA((uint8_t)(double(keys[Key::idxBlack[i]].color.r)), (uint8_t)(double(keys[Key::idxBlack[i]].color.g)), (uint8_t)(double(keys[Key::idxBlack[i]].color.b)), 255);
            clr2 = nvgRGBA((uint8_t)(0.9 * double(keys[Key::idxBlack[i]].color.r)), (uint8_t)(0.9 * double(keys[Key::idxBlack[i]].color.g)), (uint8_t)(0.9 * double(keys[Key::idxBlack[i]].color.b)), 255);
        }

        // Background for black keys
        nvgBeginPath(vg);
        nvgRect(vg, keys[Key::idxBlack[i]].x, keyPositionY, blackKeyDimension.x, blackKeyDimension.y);
        nvgFillColor(vg, nvgRGBA(10,10,10,255));
        nvgFill(vg);

        // grey border
        nvgBeginPath(vg);
        nvgRect(vg, keys[Key::idxBlack[i]].x + keySpacing, keyPositionY, blackKeyDimension.x - ks2, blackKeyDimension.y - keySpacing);
        nvgFillColor(vg, clr1);
        nvgFill(vg);

        // nearly black main color
        nvgBeginPath(vg);
        nvgRect(vg, keys[Key::idxBlack[i]].x + keySpacing + keyEdge, keyPositionY, blackKeyDimension.x - ks2 - ke2, blackKeyDimension.y - keySpacing - keyEdge);
        nvgFillColor(vg, clr2);
        nvgFill(vg);

        // highlight for bottom of key
        nvgBeginPath(vg);
        nvgRoundedRectVarying(vg, keys[Key::idxBlack[i]].x + keySpacing + keyEdge - 0.6f, keyPositionY + blackKeyDimension.y - keySpacing - f, blackKeyDimension.x - ks2 - ke2 + 1.2f, f, r, r, 0.0f, 0.0f);
        nvgFillColor(vg, clr1);
        nvgFill(vg);

        // Shadow
        uint8_t shadowAlpha = keys[Key::idxBlack[i]].pressed ? 60 : 120;
        nvgBeginPath(vg);
        nvgRect(vg, keys[Key::idxBlack[i]].x + blackKeyDimension.x, keyPositionY, sw, blackKeyDimension.y);
        NVGpaint gradient = nvgLinearGradient(vg, keys[Key::idxBlack[i]].x + blackKeyDimension.x, keyPositionY, keys[Key::idxBlack[i]].x + blackKeyDimension.x + blackKeyShadowGradient.x, keyPositionY + blackKeyShadowGradient.y, nvgRGBA(0,0,0,shadowAlpha), nvgRGBA(0,0,0,0));
        nvgFillPaint(vg, gradient);
        nvgFill(vg);
    }

    // Top edge
    nvgBeginPath(vg);
    nvgRect(vg, position.x, position.y, dimension.x, topEdgeHeight);
    NVGpaint gradient = nvgLinearGradient(vg, position.x, position.y, position.x, position.y + topEdgeHeight, nvgRGBA(90,90,90,255), nvgRGBA(40,40,40,255));
    nvgFillPaint(vg, gradient);
    nvgFill(vg);
    nvgBeginPath(vg);
    nvgRect(vg, position.x, position.y + topEdgeHeight - topEdgeHeight*0.2f, dimension.x, topEdgeHeight*0.4f);
    nvgFillColor(vg, nvgRGBA(100,10,10,100));
    nvgFill(vg);
}

void MusicalKeyboard::Resize(GLFWwindow* wnd, glm::ivec2 lowerBound, glm::ivec2 upperBound, int align){
    align = std::clamp(align, 0, 1);
    int width = upperBound.x - lowerBound.x;
    int height = upperBound.y - lowerBound.y;
    keySpacing = (float)(KEY_SPACING_RATIO * (double)width);
    keyEdge = (float)(KEY_EDGE_RATIO * (double)width);

    // White keys
    double d = (double)width - 53.0 * keySpacing;
    double whiteKeyWidth = ((d < 0.0) ? 0.0 : d) / 52.0;
    double whiteKeyHeight = KEY_WHITE_RATIO * whiteKeyWidth;
    double whiteKeyStride = whiteKeyWidth + keySpacing;
    topEdgeHeight = (double)(RATIO_TOP_EDGE_HEIGHT * whiteKeyWidth);
    keyPositionY = !align ? (double(height + lowerBound.y) - whiteKeyHeight - KEY_MARGIN_BOTTOM_PX) : (lowerBound.y + topEdgeHeight);
    whiteKeyRadius = (float)(whiteKeyWidth * KEY_WHITE_RATIO_RADIUS);
    for(int i = 0; i < 52; i++){
        keys[Key::idxWhite[i]].x = (float)lowerBound.x + (float)(keySpacing + double(i) * whiteKeyStride);
    }
    whiteKeyDimension.x = (GLfloat)whiteKeyWidth;
    whiteKeyDimension.y = (GLfloat)whiteKeyHeight;

    // Black keys
    double blackKeyWidth = KEY_RATIO_BLACK_WHITE * whiteKeyWidth;
    double blackKeyHeight = KEY_BLACK_RATIO * blackKeyWidth;
    blackKeyDimension.x = (GLfloat)blackKeyWidth;
    blackKeyDimension.y = (GLfloat)blackKeyHeight;
    keys[Key::idxBlack[0]].x = (float)lowerBound.x + (float)(0.5*keySpacing + whiteKeyStride - BLACK_KEY_SHIFT_B*blackKeyWidth);
    keys[Key::idxBlack[1]].x = (float)lowerBound.x + (float)(0.5*keySpacing + 3.0 * whiteKeyStride - BLACK_KEY_SHIFT_DB*blackKeyWidth);
    keys[Key::idxBlack[2]].x = (float)lowerBound.x + (float)(0.5*keySpacing + 4.0 * whiteKeyStride - BLACK_KEY_SHIFT_EB*blackKeyWidth);
    keys[Key::idxBlack[3]].x = (float)lowerBound.x + (float)(0.5*keySpacing + 6.0 * whiteKeyStride - BLACK_KEY_SHIFT_GB*blackKeyWidth);
    keys[Key::idxBlack[4]].x = (float)lowerBound.x + (float)(0.5*keySpacing + 7.0 * whiteKeyStride - BLACK_KEY_SHIFT_AB*blackKeyWidth);
    keys[Key::idxBlack[5]].x = (float)lowerBound.x + (float)(0.5*keySpacing + 8.0 * whiteKeyStride - BLACK_KEY_SHIFT_B*blackKeyWidth);
    double keyStrideOctave = 7.0 * whiteKeyStride;
    for(int i = 1; i < 7; i++){
        double di = double(i);
        int j = i * 5;
        keys[Key::idxBlack[j+1]].x = keys[Key::idxBlack[1]].x + di * keyStrideOctave;
        keys[Key::idxBlack[j+2]].x = keys[Key::idxBlack[2]].x + di * keyStrideOctave;
        keys[Key::idxBlack[j+3]].x = keys[Key::idxBlack[3]].x + di * keyStrideOctave;
        keys[Key::idxBlack[j+4]].x = keys[Key::idxBlack[4]].x + di * keyStrideOctave;
        keys[Key::idxBlack[j+5]].x = keys[Key::idxBlack[5]].x + di * keyStrideOctave;
    }
    double sw = BLACK_KEY_SHADOW_WIDTH * blackKeyWidth;
    glm::vec2 v(blackKeyHeight, sw);
    double a = atan2(sw,blackKeyHeight);
    blackKeyShadowGradient = glm::normalize(v);
    blackKeyShadowGradient.x *= sw * cos(a);
    blackKeyShadowGradient.y *= sw * cos(a);

    // Total position/dimension of keyboard
    position.x = (float)lowerBound.x;
    position.y = keyPositionY - topEdgeHeight;
    dimension.x = (GLfloat)width;
    dimension.y = !align ? (GLfloat)(topEdgeHeight + whiteKeyHeight + KEY_MARGIN_BOTTOM_PX) : (GLfloat)(topEdgeHeight + whiteKeyHeight + keyEdge);
    (void)wnd;
}

