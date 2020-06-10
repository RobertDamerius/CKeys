#include <WidgetPerformanceInfo.hpp>


WidgetPerformanceInfo::WidgetPerformanceInfo(nanogui::Widget *parent): NonmovableWindow(parent, ""){
    using namespace nanogui;
    this->setPosition(Vector2i(0, 0));

    this->button = new Button(this, "Test Button", ENTYPO_ICON_TV);
    this->button->setFixedSize(Vector2i(0,0));
    this->button->setFontSize(50);
    this->button->setCallback([](){
        printf("# BUTTON CALLBACK #\n");
    });
}

nanogui::Vector2i WidgetPerformanceInfo::preferredSize(NVGcontext *ctx)const{
    using namespace nanogui;
    (void)ctx;
    return Vector2i(500, 500);
}

void WidgetPerformanceInfo::performLayout(NVGcontext *ctx){
    using namespace nanogui;
    Widget::performLayout(ctx);

    this->button->setPosition(Vector2i(100,100));
    (void)ctx;
}

void WidgetPerformanceInfo::update(NVGcontext *ctx){
    (void)ctx;
}

