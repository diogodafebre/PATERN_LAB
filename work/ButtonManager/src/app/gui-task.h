#ifndef GUI_TASK_H
#define GUI_TASK_H

#include "config/gui-task-config.h"

#if defined(CONFIG_LVGL) && (CONFIG_LVGL != 0)

#include <lvgl.h>
#include "xf/behavior.h"
#include "mdw/button/buttoneventshandler.h"

namespace app
{

/**
 * @brief GuiTask task used to show button events on the LCD display.
 * 
 * To enabled the GuiTask, set in the 'zephyr/prj.conf' file the defines CONFIG_DISPLAY and CONFIG_LVGL to 'y'.
 */
class GuiTask : public XFBehavior,
                public interface::ButtonEventsHandlerObserver
{
    friend class evButton;      // GuiTask private event class

public:
    GuiTask();

    void initialize(ButtonEventsHandler & buttonEventsHandler);
    void start();

    // From XFBehavior
protected:
    XFEventStatus processEvent() override;

    typedef enum {
        evTimeout = 0xFF,
        evButtonShortPressedId = 0xB5,
        evButtonLongPressedId = 0xB7
    } InternalEventId;

    // interface::ButtonEventsHandlerObserver implementation
protected:
    void onButtonShortPressed(ButtonIndex buttonIndex) override;
    void onButtonLongPressed(ButtonIndex buttonIndex) override;

protected:
    void addMessage(const char * message);

protected:
    const int32_t interval_;

    lv_obj_t * textareaMessages;
};
} // namespace app
#endif // CONFIG_LVGL
#endif // GUI_TASK_H
