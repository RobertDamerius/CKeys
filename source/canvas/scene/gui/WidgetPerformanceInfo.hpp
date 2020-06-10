#pragma once


#include <NonmovableWindow.hpp>
#include <nanogui.h>


class WidgetPerformanceInfo: public NonmovableWindow {
    public:
        /**
         *  @brief Create a performance info widget.
         *  @param [in] parent The parent widget.
         */
        explicit WidgetPerformanceInfo(nanogui::Widget *parent);

        /**
         *  @brief The preferred size of this button.
         *  @param [in] ctx NanoVG context.
         *  @return Preferred size in pixels.
         */
        nanogui::Vector2i preferredSize(NVGcontext *ctx)const override;

        /**
         *  @brief Perform layout.
         *  @param [in] ctx NanoVG context.
         */
        void performLayout(NVGcontext *ctx)override;

        /**
         *  @brief Update event.
         *  @param [in] ctx NanoVG context.
         */
        void update(NVGcontext *ctx)override;

    private:
        nanogui::Button* button;
};

