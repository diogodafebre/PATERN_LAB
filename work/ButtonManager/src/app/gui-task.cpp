#if defined(CONFIG_LVGL) && (CONFIG_LVGL != 0)

#include <cassert>
#include <cstdio>
#include <zephyr/kernel.h>
#include <zephyr/drivers/display.h>
#include "xf/customevent.h"
#include "gui-task.h"

namespace app
{

/**
 * @brief Internal custom event holding button index as attribute.
 */
class evButton : public XFCustomEvent
{
public:
    evButton(uint8_t buttonIndex, bool longPressed, interface::XFBehavior * pBehavior) :
        XFCustomEvent((EventId)longPressed ? GuiTask::evButtonLongPressedId :
                                             GuiTask::evButtonShortPressedId, pBehavior),
        buttonIndex(buttonIndex)
    {}

    uint8_t buttonIndex;
};

GuiTask::GuiTask():
    XFBehavior(/* active = */ true, /* threadName = */ "GuiTask"),
    interval_(20),
    textareaMessages(nullptr)
{

}

void GuiTask::initialize(ButtonEventsHandler & buttonEventsHandler)
{
    bool success = buttonEventsHandler.subscribe(this);
    assert(success);

    const struct device * display;

    display = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    if (!device_is_ready(display)) {
        printk("Error: Display not ready! Aborting.");
        assert(false);
    }

    // display_set_orientation(display, DISPLAY_ORIENTATION_ROTATED_180);       // Not implemented!

    // Disable scroll bars on actual screen
    lv_obj_set_scrollbar_mode(lv_scr_act(), LV_SCROLLBAR_MODE_OFF);

    // Gui header
    lv_obj_t * title_label = lv_label_create(lv_scr_act());
    lv_label_set_text(title_label, "RSE - Lab - ButtonManager");
    lv_obj_set_pos(title_label, 465, 260);
    lv_obj_set_style_transform_angle(title_label, 1800, LV_STATE_DEFAULT);

    // Text area for trace messages
    textareaMessages = lv_textarea_create(lv_scr_act());
    assert(textareaMessages);
    lv_obj_set_pos(textareaMessages, 465, 240);
    lv_obj_set_size(textareaMessages, 450, 230);
    //lv_textarea_set_text(textareaMessages, "Trace messages\n");
    lv_obj_set_style_transform_angle(textareaMessages, 1800, LV_STATE_DEFAULT);

    display_blanking_off(display);
}

void GuiTask::start()
{
    startBehavior();
}

XFEventStatus GuiTask::processEvent()
{
    if (getCurrentEvent()->getEventType() == XFEvent::Initial)
    {
        pushEvent(evTimeout, 20);
    }

    // Handle GUI update
    if (getCurrentEvent()->getEventType() == XFEvent::Event and
        getCurrentEvent()->getId() == evTimeout)
    {
        pushEvent(evTimeout, 20);

        // Call regularly the LVGL task function to execute GUI related tasks
        lv_task_handler();
    }

    // React on button events short and long pressed
    if (getCurrentEvent()->getEventType() == XFEvent::Event)
    {
        char msg[128];

        switch (getCurrentEvent()->getId())
        {
        case evButtonShortPressedId:
            {
                const evButton * event = static_cast<const evButton *>(getCurrentEvent());
                sprintf(msg, "Button %d short pressed\n", event->buttonIndex);
                addMessage(msg);
            }
            break;
        case evButtonLongPressedId:
            {
                const evButton * event = static_cast<const evButton *>(getCurrentEvent());
                sprintf(msg, "Button %d long pressed\n", event->buttonIndex);
                addMessage(msg);
            }
            break;
        default:
            break;
        }
    }

    return XFEventStatus::Consumed;
}

void GuiTask::onButtonShortPressed(ButtonIndex buttonIndex)
{
    const bool shortPressed = false;

    GEN(evButton(buttonIndex, shortPressed, this));
}

void GuiTask::onButtonLongPressed(ButtonIndex buttonIndex)
{
    const bool longPressed = true;

    GEN(evButton(buttonIndex, longPressed, this));
}

void GuiTask::addMessage(const char * message)
{
    lv_textarea_add_text(textareaMessages, message);
}

} // namespace app
#endif // CONFIG_LVGL
