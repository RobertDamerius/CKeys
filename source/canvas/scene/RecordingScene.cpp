#include <RecordingScene.hpp>
#include <MainWindow.hpp>


void RecordingScene::Resize(GLFWwindow* wnd, int width, int height){
    keyboard.Resize(wnd, glm::ivec2(0,0), glm::ivec2(width, height), 1);
    laneManager.Resize(wnd, glm::ivec2(0,0), glm::ivec2(width, height), keyboard, 1);
}

void RecordingScene::Draw(NVGcontext* vg){
    laneManager.Draw(vg, recorder, keyboard);
    keyboard.Draw(vg);
}

void RecordingScene::CallbackKey(GLFWwindow* wnd, int key, int scancode, int action, int mods){
    // Space: Start/stop recording
    if((GLFW_KEY_SPACE == key) && (GLFW_PRESS == action)){
        if(recorder.IsRecording()){
            recorder.StopRecording();
        }
        else{
            if(!recorder.StartRecording()){
                LogError("Failed to start recording!\n");
            }
        }
    }

    // While recording: no more actions are allowed
    if(recorder.IsRecording()){
        return;
    }

    // Ctrl + S: Save recording to MIDI file
    if((GLFW_KEY_S == key) && (GLFW_PRESS == action) && (GLFW_MOD_CONTROL & mods)){
        if(!recorder.Save()){
            LogError("Could not save MIDI file!\n");
        }
    }

    // Ctrl + P: Switch to performance mode
    if((GLFW_KEY_P == key) && (GLFW_PRESS == action) && (GLFW_MOD_CONTROL & mods)){
        MainWindow::canvas.scene.sceneMode = SCENE_MODE_PERFORMANCE;
    }
    (void)wnd;
    (void)scancode;
}

