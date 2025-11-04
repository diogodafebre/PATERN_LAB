#ifndef BOARD_BUTTONSCONTROLLER_H
#define BOARD_BUTTONSCONTROLLER_H

#include <cassert>
#include <cstdint>
#include "xf/behavior.h"
#include "interface/buttonirq.h"
#include "interface/buttonscontrollercallbackcaller.h"

namespace board {

/**
 * @brief ButtonsController handles button debouncing and state changes.
 *
 * This class implements a state machine that:
 * - Receives button IRQ notifications
 * - Debounces button inputs
 * - Notifies registered callbacks about button pressed/released events
 */
class ButtonsController : public XFBehavior,
                         public interface::ButtonIrq,
                         public interface::ButtonsControllerCallbackCaller
{
public:
    static const uint8_t BUTTON_COUNT = 4;
    static const uint32_t DEBOUNCE_TIME_MS = 20;  // 20ms debounce time

public:
    ButtonsController();
    virtual ~ButtonsController();

    inline static ButtonsController & getInstance() { assert(pInstance_); return *pInstance_; }

    bool initialize();

    // From ButtonIrq interface
protected:
    void onIrq() override;

    // From ButtonsControllerCallbackCaller interface
protected:
    bool registerCallback(interface::ButtonsControllerCallbackProvider * callbackProvider,
                         interface::ButtonsControllerCallbackProvider::CallbackMethod callbackMethod) override;

    // From XFBehavior
protected:
    XFEventStatus processEvent() override;

private:
    typedef enum {
        evCheckButtons = 0xC0,
        evDebounceTimeout = 0xC1
    } InternalEventId;

    struct ButtonState {
        bool currentState;      // Current debounced state
        bool previousState;     // Previous debounced state
        bool rawState;          // Raw GPIO reading
        uint32_t debounceTimer; // Debounce timer counter
        bool stateChanged;      // Flag to indicate state change
    };

    static ButtonsController * pInstance_;

    ButtonState buttonStates_[BUTTON_COUNT];

    interface::ButtonsControllerCallbackProvider * callbackProvider_;
    interface::ButtonsControllerCallbackProvider::CallbackMethod callbackMethod_;

    void checkButtonStates();
    void notifyButtonChange(uint16_t buttonIndex, bool pressed);
};

} // namespace board

#endif // BOARD_BUTTONSCONTROLLER_H
