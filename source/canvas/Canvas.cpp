#include <Canvas.hpp>


bool Canvas::Initialize(GLFWwindow* wnd){
    // Set some initial OpenGL states
    DEBUG_GLCHECK( glDisable(GL_DEPTH_TEST); );
    DEBUG_GLCHECK( glDisable(GL_STENCIL_TEST); );
    DEBUG_GLCHECK( glDepthFunc(GL_LEQUAL); );
    DEBUG_GLCHECK( glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA); );
    DEBUG_GLCHECK( glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD); );
    DEBUG_GLCHECK( glEnable(GL_CULL_FACE); );
    DEBUG_GLCHECK( glCullFace(GL_BACK); );
    DEBUG_GLCHECK( glEnable(GL_STENCIL_TEST); );
    DEBUG_GLCHECK( glStencilMask(GL_FALSE); );
    DEBUG_GLCHECK( glClearColor(0.0f, 0.0f, 0.0f, 0.0f); );

    // Initialize renderer and scene
    if(!renderer.Initialize(wnd)){
        LogError("Could not initialize the renderer!\n");
        return false;
    }
    if(!scene.Initialize(wnd)){
        LogError("Could not initialize the scene!\n");
        return false;
    }

    // Clear both buffers with clear-color and set swap interval to avoid tearing
    DEBUG_GLCHECK( glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); );
    glfwSwapBuffers(wnd);
    DEBUG_GLCHECK( glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); );
    glfwSwapInterval(1);
    return true;
}

void Canvas::Terminate(GLFWwindow* wnd){
    scene.Terminate(wnd);
    renderer.Terminate(wnd);
}

void Canvas::Render(GLFWwindow* wnd, double dt){
    scene.Update(wnd, dt);
    renderer.RenderFrame(wnd, scene);
}

void Canvas::Resize(GLFWwindow* wnd, int width, int height){
    scene.Resize(wnd, width, height);
    renderer.Resize(wnd, width, height);
}

void Canvas::DropFiles(GLFWwindow* wnd, int count, const char** paths){
    // Only allow single path (can be either file or folder)
    // Only allow drag&drop when the scene is in performance mode
    if((1 == count) && (SCENE_MODE_PERFORMANCE == scene.sceneMode)){
        scene.performance.Load(wnd, std::string(paths[0]));
    }
}

void Canvas::CallbackKey(GLFWwindow* wnd, int key, int scancode, int action, int mods){
    scene.CallbackKey(wnd, key, scancode, action, mods);
}

void Canvas::CallbackCursorPosition(GLFWwindow* wnd, double xpos, double ypos){
    scene.CallbackCursorPosition(wnd, xpos, ypos);
}

void Canvas::CallbackMouseButton(GLFWwindow* wnd, int button, int action, int mods){
    scene.CallbackMouseButton(wnd, button, action, mods);
}

void Canvas::CallbackChar(GLFWwindow* wnd, unsigned codepoint){
    scene.CallbackChar(wnd, codepoint);
}

void Canvas::CallbackScroll(GLFWwindow* wnd, double xoffset, double yoffset){
    scene.CallbackScroll(wnd, xoffset, yoffset);
}

