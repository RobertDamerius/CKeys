#include <GUIMenu.hpp>


GUIMenu::GUIMenu(){
    screen = nullptr;
    performanceInfo = nullptr;
}

bool GUIMenu::Initialize(GLFWwindow* wnd){
    Terminate();
    screen = new nanogui::Screen();
    screen->initialize(wnd, false);
    performanceInfo = new WidgetPerformanceInfo(screen);
    screen->setVisible(true);
    screen->performLayout();
    return true;
}

void GUIMenu::Terminate(void){
    if(screen){
        delete screen;
        screen = nullptr;
    }
    performanceInfo = nullptr;
}

void GUIMenu::Render(void){
    //#TODO: Render GUI
    // if(screen){
        // screen->update();
        // screen->performLayout();
        // screen->drawContents();
        // screen->drawWidgets();
    // }
}

bool GUIMenu::CursorOverGUI(glm::dvec2 cursor){
    int32_t x = (int32_t)cursor.x;
    int32_t y = (int32_t)cursor.y;
    if(screen){
        std::vector<nanogui::Widget*> childs = screen->children();
        for(auto&& c : childs){
            Eigen::Vector2i p = c->absolutePosition();
            Eigen::Vector2i d = c->size();
            if(c->visible() && (x >= p[0]) && (y >= p[1]) && (x <= (p[0] + d[0])) && (y <= (p[1] + d[1]))){
                return true;
            }
        }
    }
    return false;
}

bool GUIMenu::IsFocused(void){
    if(screen){
        std::vector<nanogui::Widget*> childs = screen->children();
        for(auto&& c : childs){
            if(c->focused()){
                return true;
            }
        }
    }
    return false;
}

void GUIMenu::CallbackFramebufferResize(GLFWwindow* wnd, int width, int height){
    if(screen){
        screen->resizeCallbackEvent(width, height);
    }
    (void)wnd;
}

void GUIMenu::CallbackKey(GLFWwindow* wnd, int key, int scancode, int action, int mods){
    if(screen){
        screen->keyCallbackEvent(key, scancode, action, mods);
    }
    (void)wnd;
}

void GUIMenu::CallbackChar(GLFWwindow* wnd, unsigned codepoint){
    if(screen){
        screen->charCallbackEvent(codepoint);
    }
    (void)wnd;
}

void GUIMenu::CallbackMouseButton(GLFWwindow* wnd, int button, int action, int mods){
    if(screen){
        screen->mouseButtonCallbackEvent(button, action, mods);
    }
    (void)wnd;
}

void GUIMenu::CallbackCursorPosition(GLFWwindow* wnd, double xpos, double ypos){
    if(screen){
        screen->cursorPosCallbackEvent(xpos, ypos);
    }
    (void)wnd;
}

void GUIMenu::CallbackScroll(GLFWwindow* wnd, double xoffset, double yoffset){
    if(screen){
        screen->scrollCallbackEvent(xoffset, yoffset);
    }
    (void)wnd;
}

