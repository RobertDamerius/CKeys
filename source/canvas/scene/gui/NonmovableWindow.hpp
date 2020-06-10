#pragma once


#include <nanogui.h>


class NonmovableWindow : public nanogui::Window {
    // friend class nanogui::Popup;
    public:
        NonmovableWindow(nanogui::Widget *parent, const std::string &title = "Untitled");
        virtual bool mouseDragEvent(const nanogui::Vector2i &p, const nanogui::Vector2i &rel, int button, int modifiers) override;
};

