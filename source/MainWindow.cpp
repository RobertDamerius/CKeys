#include <MainWindow.hpp>
#include <AudioEngine.hpp>


GLFWwindow* MainWindow::glfwWindow = nullptr;
Canvas MainWindow::canvas;
GLuint MainWindow::rectVAO = 0;
GLuint MainWindow::rectVBO = 0;


bool MainWindow::Initialize(void){
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Make sure that the window is terminated
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        MainWindow::Terminate();

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Initialize the audio engine
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        if(!AudioEngine::Initialize()){
            LogError("Could not initialize audio engine!\n");
            MainWindow::Terminate();
            return false;
        }

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Initialize GLFW and set some window hints
    // For MAC OS X also call: glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        if(GL_TRUE != glfwInit()){
            LogError("Could not initialize GLFW!\n");
            MainWindow::Terminate();
            return false;
        }
        glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
        glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
        // glfwWindowHint(GLFW_STENCIL_BITS, 0);
        // glfwWindowHint(GLFW_DEPTH_BITS, 0);
        // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 0);

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Create GLFW window, make context and set callbacks
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        const bool fullScreen = false;
        GLFWmonitor* monitor = fullScreen ? glfwGetPrimaryMonitor() : nullptr;
        glfwWindow = glfwCreateWindow(WINDOW_INITIAL_WIDTH, WINDOW_INITIAL_HEIGHT, WINDOW_TITLE, monitor, nullptr);
        if(!glfwWindow){
            LogError("Could not create GLFW window!\n");
            MainWindow::Terminate();
            return false;
        }
        glfwMakeContextCurrent(glfwWindow);
        glfwSetFramebufferSizeCallback(glfwWindow, MainWindow::CallbackFramebufferSize);
        glfwSetDropCallback(glfwWindow, MainWindow::CallbackDrop);
        glfwSetKeyCallback(glfwWindow, MainWindow::CallbackKey);
        glfwSetCursorPosCallback(glfwWindow, MainWindow::CallbackCursorPosition);
        glfwSetMouseButtonCallback(glfwWindow, MainWindow::CallbackMouseButton);
        glfwSetCharCallback(glfwWindow, MainWindow::CallbackChar);
        glfwSetScrollCallback(glfwWindow, MainWindow::CallbackScroll);

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Initiate GLEW or GLAD
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // glewExperimental = GL_TRUE;
        // if(GLEW_OK != glewInit()){
            // LogError("Could not initialize GLEW!\n");
            // MainWindow::Terminate();
            // return false;
        // }
        if(!gladLoadGL()) {
            LogError("Could not initialize GLAD!\n");
            Terminate();
            return false;
        }

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Create a normalized rectangle
    // Setup a vertex array object/vertex buffer object for the quad
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        const GLfloat rectVertices[] = {
            -1.0f,  1.0f,
            -1.0f, -1.0f,
             1.0f,  1.0f,
             1.0f, -1.0f
        };
        DEBUG_GLCHECK( glGenVertexArrays(1, &rectVAO); );
        DEBUG_GLCHECK( glGenBuffers(1, &rectVBO); );
        DEBUG_GLCHECK( glBindVertexArray(rectVAO); );
            DEBUG_GLCHECK( glBindBuffer(GL_ARRAY_BUFFER, rectVBO); );
            DEBUG_GLCHECK( glBufferData(GL_ARRAY_BUFFER, sizeof(rectVertices), &rectVertices[0], GL_STATIC_DRAW); );
            DEBUG_GLCHECK( glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0); );
            DEBUG_GLCHECK( glEnableVertexAttribArray(0); );
        DEBUG_GLCHECK( glBindVertexArray(0); );

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Initialize canvas
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        if(!canvas.Initialize(glfwWindow)){
            LogError("Could not initialize canvas!\n");
            MainWindow::Terminate();
            return false;
        }
        return true;
}

void MainWindow::Terminate(void){
    if(rectVBO){
        glDeleteBuffers(1, &rectVBO);
        rectVBO = 0;
    }
    if(rectVAO){
        glDeleteVertexArrays(1, &rectVAO);
        rectVAO = 0;
    }
    if(glfwWindow){
        canvas.Terminate(glfwWindow);
        glfwDestroyWindow(glfwWindow);
        glfwWindow = nullptr;
    }
    glfwTerminate();
    AudioEngine::Terminate();
}

void MainWindow::MainLoop(void){
    // Do nothing if window has not been created
    if(!glfwWindow)
        return;

    // Maximize window and run actual main loop
    glfwMaximizeWindow(glfwWindow);
    glfwSetTime(0.0);
    double previousTime = 0.0;
    while(!glfwWindowShouldClose(glfwWindow)){
        // Get elapsed time to last cycle
        double time = glfwGetTime();
        double dt = time - previousTime;

        // Limit FPS
        if(dt < 0.01){
            std::this_thread::sleep_for(std::chrono::microseconds((int)((0.01 - dt)*1e6)));
            time = glfwGetTime();
            dt = time - previousTime;
        }
        previousTime = time;

        // Rendering + swap buffers
        canvas.Render(glfwWindow, dt);
        glfwSwapBuffers(glfwWindow);

        // Poll all events
        glfwPollEvents();
    }
}

void MainWindow::DrawNormalizedRect(void){
    DEBUG_GLCHECK( glBindVertexArray(rectVAO); );
    DEBUG_GLCHECK( glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); );
}

