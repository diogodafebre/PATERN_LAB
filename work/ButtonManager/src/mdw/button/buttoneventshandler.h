#ifndef MDW_BUTTON_BUTTONEVENTSHANDLER_H
#define MDW_BUTTON_BUTTONEVENTSHANDLER_H

#include "interface/buttoneventshandlersubject.h"
#include "xf/behavior.h"

namespace mdw {
namespace button {

/**
 * @brief Handles button events and notifies registered observers.
 *
 * This class monitors button states, detects short and long presses,
 * and notifies observers through the observer pattern.
 */
class ButtonEventsHandler : public interface::ButtonEventsHandlerSubject,
                            public XFBehavior
{
public:
    ButtonEventsHandler();
    virtual ~ButtonEventsHandler();

    /**
     * @brief Initialize the button events handler.
     * @return true if initialization was successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Called from button IRQ to signal a button state change.
     */
    void onIrq();

    // From ButtonEventsHandlerSubject
public:
    bool subscribe(interface::ButtonEventsHandlerObserver * observer) override;
    void unsubscribe(interface::ButtonEventsHandlerObserver * observer) override;

protected:
    void notifyButtonShortPressed(ButtonIndex buttonIndex) override;
    void notifyButtonLongPressed(ButtonIndex buttonIndex) override;

    // From XFBehavior
protected:
    XFEventStatus processEvent() override;

private:
    static const uint8_t MAX_OBSERVERS = 4;
    static const uint32_t LONG_PRESS_DURATION_MS = 1000;  // 1 second for long press
    static const uint32_t DEBOUNCE_TIME_MS = 50;          // 50ms debounce

    interface::ButtonEventsHandlerObserver * observers_[MAX_OBSERVERS];
    uint8_t observerCount_;

    // Internal event IDs
    typedef enum {
        evButtonCheck = 0xA0,
        evIrqReceived = 0xA1
    } InternalEventId;

    struct ButtonState {
        bool pressed;
        bool previousPressed;
        uint32_t pressStartTime;
        bool longPressNotified;
    };

    static const uint8_t BUTTON_COUNT = 4;
    ButtonState buttonStates_[BUTTON_COUNT];

    void checkButtons();
    void handleButtonPress(ButtonIndex buttonIndex);
    void handleButtonRelease(ButtonIndex buttonIndex);
};

} // namespace button
} // namespace mdw

#endif // MDW_BUTTON_BUTTONEVENTSHANDLER_H
