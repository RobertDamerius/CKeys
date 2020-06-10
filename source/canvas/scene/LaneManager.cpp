#include <LaneManager.hpp>
#include <AudioEngine.hpp>
#include <MainWindow.hpp>


#define NOTE_BLOCK_RADIUS                 (0.004)  // Radius of note block with respect to width of lane manager.
#define NOTE_BLOCK_EDGE                   (0.001)  // Edge size of note block with respect to width of lane manager.
#define NOTE_BLOCK_COLOR_SCALE_EDGE       (0.25)   // Color scale for edge of note block.
#define NOTE_BLOCK_COLOR_SCALE_GRADIENT   (1.4)    // Color scale for upper gradient color of note block color.


LaneManager::LaneManager(){
    mouseHover = false;
    timeScrollMode = false;
    playState = false;
    timeHorizon = 3.0;
    noteRadius = 0.0;
    edgeSize = 0.0;
    yCursorBegin = 0.0;
    timeBegin = 0.0;
    y0 = 0.0;
};

void LaneManager::Draw(NVGcontext* vg, Sequencer& sequencer, double timePointer, MusicalKeyboard& keyboard){
    // Reset keys of keyboard
    for(int i = 0; i < 88; i++){
        keyboard.keys[i].pressed = false;
    }

    // Background
    nvgBeginPath(vg);
    nvgRect(vg, position.x, position.y, dimension.x, dimension.y);
    nvgFillColor(vg, nvgRGBA(60,60,60,255));
    nvgFill(vg);
    for(int i = 0; i < 52; i++){
        nvgBeginPath(vg);
        nvgRect(vg, lanes[Key::idxWhite[i]].x, position.y, lanes[Key::idxWhite[i]].w, dimension.y);
        nvgFillColor(vg, nvgRGBA(lanes[Key::idxWhite[i]].color.x, lanes[Key::idxWhite[i]].color.y, lanes[Key::idxWhite[i]].color.z, 255));
        nvgFill(vg);
    }
    for(int i = 0; i < 36; i++){
        nvgBeginPath(vg);
        nvgRect(vg, lanes[Key::idxBlack[i]].x, position.y, lanes[Key::idxBlack[i]].w, dimension.y);
        nvgFillColor(vg, nvgRGBA(lanes[Key::idxBlack[i]].color.x, lanes[Key::idxBlack[i]].color.y, lanes[Key::idxBlack[i]].color.z, 180));
        nvgFill(vg);
    }

    // Render sequencer data of selected tracks
    nvgScissor(vg, position.x, position.y, dimension.x, dimension.y);
    double pixelsPerSecond = this->dimension.y / this->timeHorizon;
    float edgeSize2 = edgeSize + edgeSize;
    for(int idxTrack = 0; idxTrack < (int)sequencer.tracks.size(); idxTrack++){
        // White keys
        for(int i = 0; i < 52; i++){
            int k = Key::idxWhite[i];
            for(auto&& noteBlock : sequencer.tracks[idxTrack].lanes[k]){
                double y = (noteBlock.on - timePointer) * pixelsPerSecond;
                double h = (noteBlock.off - noteBlock.on) * pixelsPerSecond;
                y = position.y + dimension.y - y - h;

                // Ignore note blocks out of lane area
                if(((y < position.y) && ((y+h) < position.y)) || ((y > (position.y + dimension.y)) && ((y + h) > (position.y + dimension.y)))){
                    continue;
                }

                // Draw the note block
                nvgBeginPath(vg);
                nvgRoundedRect(vg, this->lanes[k].x, (float)y, this->lanes[k].w, (float)h, noteRadius);
                nvgFillColor(vg, nvgRGBA(uint8_t(NOTE_BLOCK_COLOR_SCALE_EDGE * double(sequencer.tracks[idxTrack].colorWhiteKey.r)), uint8_t(NOTE_BLOCK_COLOR_SCALE_EDGE * double(sequencer.tracks[idxTrack].colorWhiteKey.g)), uint8_t(NOTE_BLOCK_COLOR_SCALE_EDGE * double(sequencer.tracks[idxTrack].colorWhiteKey.b)), 255));
                nvgFill(vg);
                nvgBeginPath(vg);
                nvgRoundedRect(vg, this->lanes[k].x + edgeSize, (float)y + edgeSize, this->lanes[k].w - edgeSize2, (float)h - edgeSize2, noteRadius - edgeSize);
                NVGcolor colorGradientBegin = nvgRGBA(uint8_t(std::min(NOTE_BLOCK_COLOR_SCALE_GRADIENT * double(sequencer.tracks[idxTrack].colorWhiteKey.r), 255.0)), uint8_t(std::min(NOTE_BLOCK_COLOR_SCALE_GRADIENT * double(sequencer.tracks[idxTrack].colorWhiteKey.g), 255.0)), uint8_t(std::min(NOTE_BLOCK_COLOR_SCALE_GRADIENT * double(sequencer.tracks[idxTrack].colorWhiteKey.b),255.0)), 255);
                NVGcolor colorGradientEnd = nvgRGBA(sequencer.tracks[idxTrack].colorWhiteKey.r, sequencer.tracks[idxTrack].colorWhiteKey.g, sequencer.tracks[idxTrack].colorWhiteKey.b, 255);
                NVGpaint gradient = nvgLinearGradient(vg, this->lanes[k].x + edgeSize, 0.0f, this->lanes[k].x + this->lanes[k].w - edgeSize2, 0.0f, colorGradientBegin, colorGradientEnd);
                nvgFillPaint(vg, gradient);
                nvgFill(vg);

                // Check if key is pressed
                bool pressed = ((y + h) >= (double)(position.y + dimension.y));
                keyboard.keys[k].pressed |= pressed;
                if(pressed){
                    keyboard.keys[k].color = sequencer.tracks[idxTrack].colorWhiteKey;
                }
            }
        }

        // Black keys
        for(int i = 0; i < 36; i++){
            int k = Key::idxBlack[i];
            for(auto&& noteBlock : sequencer.tracks[idxTrack].lanes[k]){
                double y = (noteBlock.on - timePointer) * pixelsPerSecond;
                double h = (noteBlock.off - noteBlock.on) * pixelsPerSecond;
                y = position.y + dimension.y - y - h;

                // Ignore note blocks out of lane area
                if(((y < position.y) && ((y+h) < position.y)) || ((y > (position.y + dimension.y)) && ((y + h) > (position.y + dimension.y)))){
                    continue;
                }

                // Draw the note block
                nvgBeginPath(vg);
                nvgRoundedRect(vg, this->lanes[k].x, (float)y, this->lanes[k].w, (float)h, noteRadius);
                nvgFillColor(vg, nvgRGBA(uint8_t(NOTE_BLOCK_COLOR_SCALE_EDGE * double(sequencer.tracks[idxTrack].colorBlackKey.r)), uint8_t(NOTE_BLOCK_COLOR_SCALE_EDGE * double(sequencer.tracks[idxTrack].colorBlackKey.g)), uint8_t(NOTE_BLOCK_COLOR_SCALE_EDGE * double(sequencer.tracks[idxTrack].colorBlackKey.b)), 255));
                nvgFill(vg);
                nvgBeginPath(vg);
                nvgRoundedRect(vg, this->lanes[k].x + edgeSize, (float)y + edgeSize, this->lanes[k].w - edgeSize2, (float)h - edgeSize2, noteRadius - edgeSize);
                NVGcolor colorGradientBegin = nvgRGBA(uint8_t(std::min(NOTE_BLOCK_COLOR_SCALE_GRADIENT * double(sequencer.tracks[idxTrack].colorBlackKey.r), 255.0)), uint8_t(std::min(NOTE_BLOCK_COLOR_SCALE_GRADIENT * double(sequencer.tracks[idxTrack].colorBlackKey.g), 255.0)), uint8_t(std::min(NOTE_BLOCK_COLOR_SCALE_GRADIENT * double(sequencer.tracks[idxTrack].colorBlackKey.b),255.0)), 255);
                NVGcolor colorGradientEnd = nvgRGBA(sequencer.tracks[idxTrack].colorBlackKey.r, sequencer.tracks[idxTrack].colorBlackKey.g, sequencer.tracks[idxTrack].colorBlackKey.b, 255);
                NVGpaint gradient = nvgLinearGradient(vg, this->lanes[k].x + edgeSize, 0.0f, this->lanes[k].x + this->lanes[k].w - edgeSize2, 0.0f, colorGradientBegin, colorGradientEnd);
                nvgFillPaint(vg, gradient);
                nvgFill(vg);

                // Check if key is pressed
                bool pressed = ((y + h) >= (double)(position.y + dimension.y));
                keyboard.keys[k].pressed |= pressed;
                if(pressed){
                    keyboard.keys[k].color = sequencer.tracks[idxTrack].colorBlackKey;
                }
            }
        }
    }
    nvgResetScissor(vg);
}

void LaneManager::Draw(NVGcontext* vg, Recorder& recorder, MusicalKeyboard& keyboard){
    // Reset keys of keyboard
    for(int i = 0; i < 88; i++){
        keyboard.keys[i].pressed = false;
    }

    // Background
    nvgBeginPath(vg);
    nvgRect(vg, position.x, position.y, dimension.x, dimension.y);
    nvgFillColor(vg, nvgRGBA(60,60,60,255));
    nvgFill(vg);
    for(int i = 0; i < 52; i++){
        nvgBeginPath(vg);
        nvgRect(vg, lanes[Key::idxWhite[i]].x, position.y, lanes[Key::idxWhite[i]].w, dimension.y);
        nvgFillColor(vg, nvgRGBA(lanes[Key::idxWhite[i]].color.x, lanes[Key::idxWhite[i]].color.y, lanes[Key::idxWhite[i]].color.z, 255));
        nvgFill(vg);
    }
    for(int i = 0; i < 36; i++){
        nvgBeginPath(vg);
        nvgRect(vg, lanes[Key::idxBlack[i]].x, position.y, lanes[Key::idxBlack[i]].w, dimension.y);
        nvgFillColor(vg, nvgRGBA(lanes[Key::idxBlack[i]].color.x, lanes[Key::idxBlack[i]].color.y, lanes[Key::idxBlack[i]].color.z, 180));
        nvgFill(vg);
    }

    // Render recorder data
    nvgScissor(vg, position.x, position.y, dimension.x, dimension.y);
    double pixelsPerSecond = this->dimension.y / this->timeHorizon;
    float edgeSize2 = edgeSize + edgeSize;
    double timePointer = recorder.GetTimePointer();

    // White keys
    recorder.mtxTrack.lock();
    for(int i = 0; i < 52; i++){
        int k = Key::idxWhite[i];
        for(auto&& noteBlock : recorder.track.lanes[k]){
            double yon = y0 + (timePointer - noteBlock.on) * pixelsPerSecond;
            double yoff = y0 + (timePointer - noteBlock.off) * pixelsPerSecond;
            if(noteBlock.off > timePointer){
                yoff = -noteRadius - edgeSize;
            }
            double y = yoff;
            double h = yon - yoff;

            // Ignore note blocks out of lane area
            if(((y < position.y) && ((y+h) < position.y)) || ((y > (position.y + dimension.y)) && ((y + h) > (position.y + dimension.y)))){
                continue;
            }

            // Draw the note block
            nvgBeginPath(vg);
            nvgRoundedRect(vg, this->lanes[k].x, (float)y, this->lanes[k].w, (float)h, noteRadius);
            nvgFillColor(vg, nvgRGBA(uint8_t(NOTE_BLOCK_COLOR_SCALE_EDGE * double(recorder.track.colorWhiteKey.r)), uint8_t(NOTE_BLOCK_COLOR_SCALE_EDGE * double(recorder.track.colorWhiteKey.g)), uint8_t(NOTE_BLOCK_COLOR_SCALE_EDGE * double(recorder.track.colorWhiteKey.b)), 255));
            nvgFill(vg);
            nvgBeginPath(vg);
            nvgRoundedRect(vg, this->lanes[k].x + edgeSize, (float)y + edgeSize, this->lanes[k].w - edgeSize2, (float)h - edgeSize2, noteRadius - edgeSize);
            NVGcolor colorGradientBegin = nvgRGBA(uint8_t(std::min(NOTE_BLOCK_COLOR_SCALE_GRADIENT * double(recorder.track.colorWhiteKey.r), 255.0)), uint8_t(std::min(NOTE_BLOCK_COLOR_SCALE_GRADIENT * double(recorder.track.colorWhiteKey.g), 255.0)), uint8_t(std::min(NOTE_BLOCK_COLOR_SCALE_GRADIENT * double(recorder.track.colorWhiteKey.b),255.0)), 255);
            NVGcolor colorGradientEnd = nvgRGBA(recorder.track.colorWhiteKey.r, recorder.track.colorWhiteKey.g, recorder.track.colorWhiteKey.b, 255);
            NVGpaint gradient = nvgLinearGradient(vg, this->lanes[k].x + edgeSize, 0.0f, this->lanes[k].x + this->lanes[k].w - edgeSize2, 0.0f, colorGradientBegin, colorGradientEnd);
            nvgFillPaint(vg, gradient);
            nvgFill(vg);

            // Check if key is pressed
            bool pressed = yoff < 0.0;
            keyboard.keys[k].pressed |= pressed;
            if(pressed){
                keyboard.keys[k].color = recorder.track.colorWhiteKey;
            }
        }
    }

    // Black keys
    for(int i = 0; i < 36; i++){
        int k = Key::idxBlack[i];
        for(auto&& noteBlock : recorder.track.lanes[k]){
            double yon = y0 + (timePointer - noteBlock.on) * pixelsPerSecond;
            double yoff = y0 + (timePointer - noteBlock.off) * pixelsPerSecond;
            if(noteBlock.off > timePointer){
                yoff = -noteRadius - edgeSize;
            }
            double y = yoff;
            double h = yon - yoff;

            // Ignore note blocks out of lane area
            if(((y < position.y) && ((y+h) < position.y)) || ((y > (position.y + dimension.y)) && ((y + h) > (position.y + dimension.y)))){
                continue;
            }

            // Draw the note block
            nvgBeginPath(vg);
            nvgRoundedRect(vg, this->lanes[k].x, (float)y, this->lanes[k].w, (float)h, noteRadius);
            nvgFillColor(vg, nvgRGBA(uint8_t(NOTE_BLOCK_COLOR_SCALE_EDGE * double(recorder.track.colorBlackKey.r)), uint8_t(NOTE_BLOCK_COLOR_SCALE_EDGE * double(recorder.track.colorBlackKey.g)), uint8_t(NOTE_BLOCK_COLOR_SCALE_EDGE * double(recorder.track.colorBlackKey.b)), 255));
            nvgFill(vg);
            nvgBeginPath(vg);
            nvgRoundedRect(vg, this->lanes[k].x + edgeSize, (float)y + edgeSize, this->lanes[k].w - edgeSize2, (float)h - edgeSize2, noteRadius - edgeSize);
            NVGcolor colorGradientBegin = nvgRGBA(uint8_t(std::min(NOTE_BLOCK_COLOR_SCALE_GRADIENT * double(recorder.track.colorBlackKey.r), 255.0)), uint8_t(std::min(NOTE_BLOCK_COLOR_SCALE_GRADIENT * double(recorder.track.colorBlackKey.g), 255.0)), uint8_t(std::min(NOTE_BLOCK_COLOR_SCALE_GRADIENT * double(recorder.track.colorBlackKey.b),255.0)), 255);
            NVGcolor colorGradientEnd = nvgRGBA(recorder.track.colorBlackKey.r, recorder.track.colorBlackKey.g, recorder.track.colorBlackKey.b, 255);
            NVGpaint gradient = nvgLinearGradient(vg, this->lanes[k].x + edgeSize, 0.0f, this->lanes[k].x + this->lanes[k].w - edgeSize2, 0.0f, colorGradientBegin, colorGradientEnd);
            nvgFillPaint(vg, gradient);
            nvgFill(vg);

            // Check if key is pressed
            bool pressed = yoff < 0.0;
            keyboard.keys[k].pressed |= pressed;
            if(pressed){
                keyboard.keys[k].color = recorder.track.colorBlackKey;
            }
        }
    }
    recorder.mtxTrack.unlock();
    nvgResetScissor(vg);
}

void LaneManager::Resize(GLFWwindow* wnd, glm::ivec2 lowerBound, glm::ivec2 upperBound, MusicalKeyboard& keyboard, int align){
    // Get keyboard layout
    glm::vec2 keyboardPosition, keyboardDimension, whiteKeyDimension, blackKeyDimension;
    float keySpacing;
    keyboard.GetLayout(keyboardPosition, keyboardDimension, whiteKeyDimension, blackKeyDimension, keySpacing);

    // Set area for lane manager
    this->position = keyboardPosition;
    this->dimension = keyboardDimension;
    this->position.y = !align ? lowerBound.y : keyboardPosition.y;
    this->dimension.y = !align ? (keyboardPosition.y - this->position.y) : (upperBound.y - this->position.y);
    this->y0 = !align ? 0.0 : (keyboardPosition.y + keyboardDimension.y);

    // Set all lane values
    for(int i = 0; i < 52; i++){
        lanes[Key::idxWhite[i]].x = keyboard.keys[Key::idxWhite[i]].x;
        lanes[Key::idxWhite[i]].w = whiteKeyDimension.x;
        lanes[Key::idxWhite[i]].color = glm::u8vec3(45);
    }
    for(int i = 0; i < 36; i++){
        lanes[Key::idxBlack[i]].x = keyboard.keys[Key::idxBlack[i]].x;
        lanes[Key::idxBlack[i]].w = blackKeyDimension.x;
        lanes[Key::idxBlack[i]].color = glm::u8vec3(40);
    }
    noteRadius = this->dimension.x * (float)NOTE_BLOCK_RADIUS;
    edgeSize = this->dimension.x * (float)NOTE_BLOCK_EDGE;
    (void)wnd;
    (void)upperBound;
}

void LaneManager::CallbackCursorPosition(GLFWwindow* wnd, double xpos, double ypos){
    // Set hover state
    double xs = (double)(position.x);
    double xe = (double)(position.x + dimension.x);
    double ys = (double)(position.y);
    double ye = (double)(position.y + dimension.y);
    mouseHover = (xpos >= xs) && (xpos <= xe) && (ypos >= ys) && (ypos <= ye);

    // Change time pointer
    if(timeScrollMode){
        double scale = timeHorizon / (double)dimension.y;
        AudioEngine::SetTimePointer(timeBegin + scale * (ypos - yCursorBegin));
    }
    (void)wnd;
}

void LaneManager::CallbackMouseButton(GLFWwindow* wnd, int button, int action, int mods){
    if(GLFW_MOUSE_BUTTON_LEFT == button){
        if((GLFW_PRESS == action) && mouseHover){
            timeScrollMode = true;
            double tmp;
            glfwGetCursorPos(wnd, &tmp, &yCursorBegin);
            playState = AudioEngine::StreamIsPlaying();
            AudioEngine::StopStream();
            timeBegin = AudioEngine::GetTimePointer();
        }
        else if((GLFW_RELEASE == action) && timeScrollMode){
            timeScrollMode = false;
            if(playState){
                AudioEngine::StartStream();
            }
            playState = false;
        }
    }
    (void)wnd;
    (void)mods;
}

