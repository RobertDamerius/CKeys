#include <NonmovableWindow.hpp>


NonmovableWindow::NonmovableWindow(nanogui::Widget *parent, const std::string &title): nanogui::Window(parent, title){}

bool NonmovableWindow::mouseDragEvent(const nanogui::Vector2i &, const nanogui::Vector2i &rel, int button, int modifiers){
    if(mDrag && (button & (1 << GLFW_MOUSE_BUTTON_1)) != 0){
        // mPos += rel;
        (void)rel;
        mPos = mPos.cwiseMax(nanogui::Vector2i::Zero());
        mPos = mPos.cwiseMin(parent()->size() - mSize);
        return true;
    }
    return false;
    (void)modifiers;
}

