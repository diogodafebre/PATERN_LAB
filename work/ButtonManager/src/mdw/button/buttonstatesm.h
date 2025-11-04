#ifndef MDW_BUTTON_BUTTONSTATESM_H
#define MDW_BUTTON_BUTTONSTATESM_H

#include <cstdint>
#include "xf/behavior.h"

namespace mdw {
namespace button {

// Forward declaration
class ButtonEventsHandler;

/**
 * @brief State machine for a single button.
 *
 * This state machine:
 * - Tracks button press/release events
 * - Detects short press (< 1 second)
 * - Detects long press (>= 1 second)
 * - Notifies ButtonEventsHandler of events
 */
class ButtonStateSm : public XFBehavior
{
public:
    /**
     * @brief Constructor
     * @param buttonIndex The index of this button (0-3)
     * @param handler Reference to the ButtonEventsHandler
     */
    ButtonStateSm(uint8_t buttonIndex, ButtonEventsHandler & handler);
    virtual ~ButtonStateSm();

    /**
     * @brief Notify the state machine that the button was pressed
     */
    void onButtonPressed();

    /**
     * @brief Notify the state machine that the button was released
     */
    void onButtonReleased();

    uint8_t getButtonIndex() const { return buttonIndex_; }

protected:
    // From XFBehavior
    XFEventStatus processEvent() override;

private:
    // Internal event IDs
    typedef enum {
        evButtonPressed = 0xD0,
        evButtonReleased = 0xD1,
        evLongPressTimeout = 0xD2
    } InternalEventId;

    // States
    typedef enum {
        STATE_IDLE,
        STATE_PRESSED,
        STATE_LONG_PRESS_DETECTED
    } State;

    static const uint32_t LONG_PRESS_DURATION_MS = 1000;  // 1 second

    uint8_t buttonIndex_;
    ButtonEventsHandler & handler_;
    State currentState_;

    void enterIdle();
    void enterPressed();
    void enterLongPressDetected();
};

} // namespace button
} // namespace mdw

#endif // MDW_BUTTON_BUTTONSTATESM_H
