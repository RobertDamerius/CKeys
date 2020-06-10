#include <Renderer.hpp>
#include <MainWindow.hpp>


bool Renderer::Initialize(GLFWwindow* wnd){
    // Make sure that the renderer is terminated
    Terminate(wnd);

    // Generate framebuffers
    int width, height;
    glfwGetWindowSize(wnd, &width, &height);
    if(!GenerateFrameBuffers(width, height)){
        Terminate(wnd);
        return false;
    }

    // Generate shaders
    if(!GenerateShaders()){
        Terminate(wnd);
        return false;
    }

    // Set default texture unit and return success
    DEBUG_GLCHECK( glActiveTexture(TEXTUREUNIT_DEFAULT); );
    return true;
}

void Renderer::Terminate(GLFWwindow* wnd){
    DeleteShaders();
    DeleteFrameBuffers();
    (void)wnd;
}

void Renderer::RenderFrame(GLFWwindow* wnd, Scene& scene){
    // Render Scene + GUI
    DEBUG_GLCHECK( glBindFramebuffer(GL_FRAMEBUFFER, fbGUI.fbo); );
    DEBUG_GLCHECK( glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); );
    scene.Draw(wnd);
    DEBUG_GLCHECK( glDisable(GL_BLEND); );
    DEBUG_GLCHECK( glDisable(GL_DEPTH_TEST); );

    // Post processing, render to default framebuffer (0)
    DEBUG_GLCHECK( glBindFramebuffer(GL_FRAMEBUFFER, 0); );
    shaderPostProcessing.Use();
    MainWindow::DrawNormalizedRect();
    (void)wnd;
}

void Renderer::Resize(GLFWwindow* wnd, int width, int height){
    width = (width < 1) ? 1 : width;
    height = (height < 1) ? 1 : height;
    GenerateFrameBuffers(width, height);
    DEBUG_GLCHECK( glViewport(0, 0, width, height); );
    (void)wnd;
}

bool Renderer::GenerateFrameBuffers(int width, int height){
    // (Re-)generate framebuffers
    if(!fbGUI.Generate(width, height, TEXTUREUNIT_GUI)){
        LogError("Could not generate GUI FBO.\n");
        return false;
    }

    // Bind texture for respective (reserved) texture unit
    DEBUG_GLCHECK( glActiveTexture(TEXTUREUNIT_GUI); );
    DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, fbGUI.cbo); );

    // Set default texture unit
    DEBUG_GLCHECK( glActiveTexture(TEXTUREUNIT_DEFAULT); );

    return true;
}

void Renderer::DeleteFrameBuffers(void){
    fbGUI.Delete();
}

bool Renderer::GenerateShaders(void){
    if(!shaderPostProcessing.Generate()){
        LogError("Could not create post processing shader!\n");
        return false;
    }
    return true;
}

void Renderer::DeleteShaders(void){
    shaderPostProcessing.Delete();
}

