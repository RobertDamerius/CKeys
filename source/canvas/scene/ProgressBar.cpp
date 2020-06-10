#include <ProgressBar.hpp>
#include <MainWindow.hpp>
#include <Sequencer.hpp>
#include <AudioEngine.hpp>


ProgressBar::ProgressBar(){
    padding = 0.0f;
    edge = 0.0f;
    radius = 0.0f;
    mouseHover = false;
    timeScrollMode = false;
    playState = false;
}

void ProgressBar::Draw(NVGcontext* vg){
    // Background of widget
    nvgBeginPath(vg);
    nvgRect(vg, position.x, position.y, dimension.x, dimension.y);
    nvgFillColor(vg, nvgRGBA(10,10,10,255));
    nvgFill(vg);

    // Edge
    float padding2 = padding + padding;
    nvgBeginPath(vg);
    nvgRoundedRect(vg, position.x + padding, position.y + padding, dimension.x - padding2, dimension.y - padding2, radius);
    nvgFillColor(vg, nvgRGBA(0,0,0,255));
    nvgFill(vg);

    // Background of bar
    bool b = mouseHover || timeScrollMode;
    float edge2 = edge + edge;
    nvgBeginPath(vg);
    nvgRoundedRect(vg, position.x + padding + edge, position.y + padding + edge, dimension.x - padding2 - edge2, dimension.y - padding2 - edge2, radius - edge);
    nvgFillPaint(vg, nvgLinearGradient(vg, 0.0f, position.y + padding + edge, 0.0f, position.y + dimension.y - padding2 - edge2, b ? nvgRGBA(70,70,70,255) : nvgRGBA(40,40,40,255), b ? nvgRGBA(90,90,90,255) : nvgRGBA(60,60,60,255)));
    nvgFill(vg);

    // Progress bar
    float progress = (float)(!MainWindow::canvas.scene.performance.sequencer.maxNumSamples ? 0.0 : (AudioEngine::GetTimePointer() / ((double)MainWindow::canvas.scene.performance.sequencer.maxNumSamples / ((double)(AUDIO_ENGINE_SAMPLE_RATE + AUDIO_ENGINE_SAMPLE_RATE)))));
    progress = std::clamp(progress, 0.0f, 1.0f);
    if(progress > 0.0){
        float x = position.x + padding + edge;
        float y = position.y + padding + edge;
        float w = radius + progress*(dimension.x - padding2 - edge2 - radius);
        nvgBeginPath(vg);
        nvgRoundedRect(vg, x, y, std::min(w + edge, dimension.x - padding2 - edge2), dimension.y - padding2 - edge2, radius - edge);
        nvgFillColor(vg, nvgRGBA(0,25,40,255));
        nvgFill(vg);
        nvgBeginPath(vg);
        nvgRoundedRect(vg, x, y, w, dimension.y - padding2 - edge2, radius - edge);
        nvgFillPaint(vg, nvgLinearGradient(vg, 0.0f, position.y + padding + edge, 0.0f, position.y + dimension.y - padding2 - edge2, b ? nvgRGBA(9,175,255,255) : nvgRGBA(0,135,200,255), b ? nvgRGBA(0,145,215,255) : nvgRGBA(0,100,150,255)));
        nvgFill(vg);
    }
}

void ProgressBar::Resize(GLFWwindow* wnd, glm::ivec2 lowerBound, glm::ivec2 upperBound){
    position = glm::vec2(lowerBound);
    dimension = glm::vec2(upperBound - lowerBound);
    padding = dimension.y * 0.2f;
    edge = std::max(1.0f, 0.0006f * dimension.x);
    radius = std::max(edge, 0.12f * dimension.y);
    (void)wnd;
}

void ProgressBar::CallbackCursorPosition(GLFWwindow* wnd, double xpos, double ypos){
    // Set hover state
    double xs = (double)(position.x + padding + edge + radius);
    double xe = (double)(position.x + dimension.x - padding - edge);
    double ys = (double)(position.y + padding + edge);
    double ye = (double)(position.y + dimension.y - padding - edge);
    mouseHover = (xpos >= xs) && (xpos <= xe) && (ypos >= ys) && (ypos <= ye);

    // Change time pointer
    if(timeScrollMode){
        double scale = std::clamp((xpos - xs) / (xe - xs), 0.0, 1.0);
        AudioEngine::SetTimePointer(scale * (double)(MainWindow::canvas.scene.performance.sequencer.maxNumSamples / 2) / (double)(AUDIO_ENGINE_SAMPLE_RATE));
    }
    (void)wnd;
}

void ProgressBar::CallbackMouseButton(GLFWwindow* wnd, int button, int action, int mods){
    if(GLFW_MOUSE_BUTTON_LEFT == button){
        if((GLFW_PRESS == action) && mouseHover){
            // Stop audio stream
            timeScrollMode = true;
            playState = AudioEngine::StreamIsPlaying();
            AudioEngine::StopStream();

            // Set time pointer
            double xpos, ypos;
            glfwGetCursorPos(wnd, &xpos, &ypos);
            double xs = (double)(position.x + padding + edge + radius);
            double xe = (double)(position.x + dimension.x - padding - edge);
            double scale = std::clamp((xpos - xs) / (xe - xs), 0.0, 1.0);
            AudioEngine::SetTimePointer(scale * (double)(MainWindow::canvas.scene.performance.sequencer.maxNumSamples / 2) / (double)(AUDIO_ENGINE_SAMPLE_RATE));
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

