#ifndef MDW_BUTTON_BUTTONEVENTSHANDLER_H
#define MDW_BUTTON_BUTTONEVENTSHANDLER_H

#include "interface/buttoneventshandlersubject.h"
#include "interface/buttonscontrollercallbackprovider.h"
#include "xf/behavior.h"

namespace mdw {
namespace button {

// Forward declaration
class ButtonStateSm;

/**
 * @brief Handles button events and notifies registered observers.
 *
 * This class:
 * - Receives button press/release events from ButtonsController
 * - Manages ButtonStateSm instances for each button
 * - Detects short and long presses
 * - Notifies observers through the observer pattern using internal events
 */
class ButtonEventsHandler : public interface::ButtonEventsHandlerSubject,
                            public interface::ButtonsControllerCallbackProvider,
                            public XFBehavior
{
    friend class ButtonStateSm;

public:
    ButtonEventsHandler();
    virtual ~ButtonEventsHandler();

    /**
     * @brief Initialize the button events handler.
     * @return true if initialization was successful, false otherwise
     */
    bool initialize();

    // From ButtonEventsHandlerSubject
public:
    bool subscribe(interface::ButtonEventsHandlerObserver * observer) override;
    void unsubscribe(interface::ButtonEventsHandlerObserver * observer) override;

protected:
    void notifyButtonShortPressed(ButtonIndex buttonIndex) override;
    void notifyButtonLongPressed(ButtonIndex buttonIndex) override;

    // From ButtonsControllerCallbackProvider
protected:
    /**
     * @brief Callback method called by ButtonsController on button state changes
     * @param buttonIndex Index of the button (0-3)
     * @param pressed true if button was pressed, false if released
     */
    void onButtonChanged(uint16_t buttonIndex, bool pressed) override;

    // From XFBehavior
protected:
    XFEventStatus processEvent() override;

private:
    static const uint8_t MAX_OBSERVERS = 4;
    static const uint8_t BUTTON_COUNT = 4;

    // Internal event IDs
    typedef enum {
        evNotifyShortPress = 0xA0,
        evNotifyLongPress = 0xA1
    } InternalEventId;

    interface::ButtonEventsHandlerObserver * observers_[MAX_OBSERVERS];
    uint8_t observerCount_;

    ButtonStateSm * buttonStateMachines_[BUTTON_COUNT];

    /**
     * @brief Called by ButtonStateSm when a short press is detected
     */
    void onButtonShortPressDetected(uint8_t buttonIndex);

    /**
     * @brief Called by ButtonStateSm when a long press is detected
     */
    void onButtonLongPressDetected(uint8_t buttonIndex);
};

} // namespace button
} // namespace mdw

#endif // MDW_BUTTON_BUTTONEVENTSHANDLER_H
