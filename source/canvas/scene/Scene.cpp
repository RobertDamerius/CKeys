#include <Scene.hpp>


Scene::Scene(){
    sceneMode = SCENE_MODE_PERFORMANCE;
    ctxVG = nullptr;
}

bool Scene::Initialize(GLFWwindow* wnd){
    Terminate(wnd);
    ctxVG = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
    if(!ctxVG){
        LogError("Could not create NanoVG context!\n");
        Terminate(wnd);
        return false;
    }
    if(!menu.Initialize(wnd)){
        LogError("Could not initialize GUI!\n");
        Terminate(wnd);
        return false;
    }
    sceneMode = SCENE_MODE_PERFORMANCE;
    (void)wnd;
    return true;
}

void Scene::Terminate(GLFWwindow* wnd){
    menu.Terminate();
    if(ctxVG){
        nvgDeleteGL3(ctxVG);
        ctxVG = nullptr;
    }
    (void)wnd;
}

void Scene::Update(GLFWwindow* wnd, double dt){
    (void)wnd;
    (void)dt;
}

void Scene::Draw(GLFWwindow* wnd){
    if(!ctxVG) return;
    int winWidth, winHeight, fbWidth, fbHeight;
    glfwGetWindowSize(wnd, &winWidth, &winHeight);
    glfwGetFramebufferSize(wnd, &fbWidth, &fbHeight);
    float pxRatio = (float)fbWidth / (float)winWidth;
    nvgBeginFrame(ctxVG, winWidth, winHeight, pxRatio);
    switch(sceneMode){
        case SCENE_MODE_PERFORMANCE: performance.Draw(ctxVG); break;
        case SCENE_MODE_RECORDING: recording.Draw(ctxVG); break;
    }
    nvgEndFrame(ctxVG);
    menu.Render();
}

void Scene::Resize(GLFWwindow* wnd, int width, int height){
    performance.Resize(wnd, width, height);
    recording.Resize(wnd, width, height);
    menu.CallbackFramebufferResize(wnd, width, height);
}

void Scene::CallbackKey(GLFWwindow* wnd, int key, int scancode, int action, int mods){
    switch(sceneMode){
        case SCENE_MODE_PERFORMANCE: performance.CallbackKey(wnd, key, scancode, action, mods); break;
        case SCENE_MODE_RECORDING: recording.CallbackKey(wnd, key, scancode, action, mods); break;
    }
    menu.CallbackKey(wnd, key, scancode, action, mods);
}

void Scene::CallbackCursorPosition(GLFWwindow* wnd, double xpos, double ypos){
    switch(sceneMode){
        case SCENE_MODE_PERFORMANCE: performance.CallbackCursorPosition(wnd, xpos, ypos); break;
        case SCENE_MODE_RECORDING: break;
    }
    menu.CallbackCursorPosition(wnd, xpos, ypos);
}

void Scene::CallbackMouseButton(GLFWwindow* wnd, int button, int action, int mods){
    switch(sceneMode){
        case SCENE_MODE_PERFORMANCE: performance.CallbackMouseButton(wnd, button, action, mods); break;
        case SCENE_MODE_RECORDING: break;
    }
    menu.CallbackMouseButton(wnd, button, action, mods);
}

void Scene::CallbackChar(GLFWwindow* wnd, unsigned codepoint){
    menu.CallbackChar(wnd, codepoint);
}

void Scene::CallbackScroll(GLFWwindow* wnd, double xoffset, double yoffset){
    menu.CallbackScroll(wnd, xoffset, yoffset);
}

