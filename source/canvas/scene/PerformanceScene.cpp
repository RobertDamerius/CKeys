#include <PerformanceScene.hpp>
#include <AudioEngine.hpp>
#include <MainWindow.hpp>


void PerformanceScene::Resize(GLFWwindow* wnd, int width, int height){
    int h = (int)(0.02 * (double)width);
    keyboard.Resize(wnd, glm::ivec2(0,0), glm::ivec2(width, height - h));
    laneManager.Resize(wnd, glm::ivec2(0,0), glm::ivec2(width, height - h), keyboard);
    progressBar.Resize(wnd, glm::ivec2(0, height - h), glm::ivec2(width, height));
}

void PerformanceScene::Draw(NVGcontext* vg){
    double timePointer = AudioEngine::GetTimePointer();
    laneManager.Draw(vg, sequencer, timePointer, keyboard);
    keyboard.Draw(vg);
    progressBar.Draw(vg);
}

void PerformanceScene::Load(GLFWwindow* wnd, std::string filename){
    glfwFocusWindow(wnd);
    (void) AudioEngine::StopStream();
    AudioEngine::SetTimePointer(0.0);
    if(!sequencer.ReadMIDIFile(filename)){
        LogError("Could not load performance \"%s\"!\n",filename.c_str());
        return;
    }
    sequencer.Generate();
}

void PerformanceScene::CallbackKey(GLFWwindow* wnd, int key, int scancode, int action, int mods){
    // Do not allow key actions during time scroll modes
    if(progressBar.GetTimeScrollMode() || laneManager.GetTimeScrollMode()){
        return;
    }

    // Ctrl + R: Stop all performance actions and switch to recording mode
    if((GLFW_KEY_R == key) && (GLFW_PRESS == action) && (GLFW_MOD_CONTROL & mods)){
        if(AudioEngine::StreamIsPlaying()){
            AudioEngine::StopStream();
        }
        MainWindow::canvas.scene.sceneMode = SCENE_MODE_RECORDING;
        return;
    }

    // Space: Start/stop playing
    if((GLFW_KEY_SPACE == key) && (GLFW_PRESS == action)){
        if(AudioEngine::StreamIsPlaying()){
            AudioEngine::StopStream();
        }
        else{
            AudioEngine::StartStream();
        }
    }
    (void)wnd;
    (void)scancode;
    (void)mods;
}

void PerformanceScene::CallbackCursorPosition(GLFWwindow* wnd, double xpos, double ypos){
    progressBar.CallbackCursorPosition(wnd, xpos, ypos);
    laneManager.CallbackCursorPosition(wnd, xpos, ypos);
}

void PerformanceScene::CallbackMouseButton(GLFWwindow* wnd, int button, int action, int mods){
    progressBar.CallbackMouseButton(wnd, button, action, mods);
    laneManager.CallbackMouseButton(wnd, button, action, mods);
}

