#include "buttonscontroller.h"
#include "hal/buttons.h"
#include "trace/trace.h"
#include "event/evbuttonirq.h"
#include <zephyr/kernel.h>

namespace board {

ButtonsController * ButtonsController::pInstance_ = nullptr;

ButtonsController::ButtonsController()
: XFBehavior(/* active = */ true, /* threadName = */ "ButtonsController")
, callbackProvider_(nullptr)
, callbackMethod_(nullptr)
{
    assert(!pInstance_);    // Only one instance of this class allowed!
    pInstance_ = this;

    // Initialize button states
    for (uint8_t i = 0; i < BUTTON_COUNT; ++i)
    {
        buttonStates_[i].currentState = false;
        buttonStates_[i].previousState = false;
        buttonStates_[i].rawState = false;
        buttonStates_[i].debounceTimer = 0;
        buttonStates_[i].stateChanged = false;
    }
}

ButtonsController::~ButtonsController()
{
    pInstance_ = nullptr;
}

bool ButtonsController::initialize()
{
    Trace::out("ButtonsController: Initializing...");

    // Initialize button HAL
    if (!hal::buttons::initialize())
    {
        Trace::out("ButtonsController: Failed to initialize button HAL");
        return false;
    }

    // Start the state machine
    startBehavior();

    Trace::out("ButtonsController: Initialized successfully");
    return true;
}

void ButtonsController::onIrq()
{
    // Called from ISR - send event to state machine
    static evButtonIrq irqEvent;
    GEN(&irqEvent);
}

bool ButtonsController::registerCallback(interface::ButtonsControllerCallbackProvider * callbackProvider,
                                        interface::ButtonsControllerCallbackProvider::CallbackMethod callbackMethod)
{
    if (callbackProvider == nullptr || callbackMethod == nullptr)
    {
        Trace::out("ButtonsController: Cannot register null callback");
        return false;
    }

    if (callbackProvider_ != nullptr)
    {
        Trace::out("ButtonsController: Callback already registered");
        return false;
    }

    callbackProvider_ = callbackProvider;
    callbackMethod_ = callbackMethod;

    Trace::out("ButtonsController: Callback registered");
    return true;
}

XFEventStatus ButtonsController::processEvent()
{
    if (getCurrentEvent()->getEventType() == XFEvent::Initial)
    {
        Trace::out("ButtonsController: State machine started");
        // Start periodic button checking
        pushEvent(evCheckButtons, 10);  // Check every 10ms
        return XFEventStatus::Consumed;
    }

    if (getCurrentEvent()->getEventType() == XFEvent::Event)
    {
        switch (getCurrentEvent()->getId())
        {
        case evCheckButtons:
            checkButtonStates();
            pushEvent(evCheckButtons, 10);  // Schedule next check
            return XFEventStatus::Consumed;

        case evButtonIrqId:
            // Button IRQ received - check buttons immediately
            checkButtonStates();
            return XFEventStatus::Consumed;

        default:
            break;
        }
    }

    return XFEventStatus::Unknown;
}

void ButtonsController::checkButtonStates()
{
    uint32_t currentTime = k_uptime_get_32();

    for (uint8_t i = 0; i < BUTTON_COUNT; ++i)
    {
        // Read raw button state (active LOW - inverted)
        bool rawPressed = !hal::buttons::isButtonPressed(i);

        // Debouncing logic
        if (rawPressed != buttonStates_[i].rawState)
        {
            // State changed - reset debounce timer
            buttonStates_[i].rawState = rawPressed;
            buttonStates_[i].debounceTimer = currentTime;
            buttonStates_[i].stateChanged = false;
        }
        else if (!buttonStates_[i].stateChanged)
        {
            // State stable - check if debounce time elapsed
            if ((currentTime - buttonStates_[i].debounceTimer) >= DEBOUNCE_TIME_MS)
            {
                // Debounce time elapsed - update current state if changed
                if (buttonStates_[i].currentState != buttonStates_[i].rawState)
                {
                    buttonStates_[i].previousState = buttonStates_[i].currentState;
                    buttonStates_[i].currentState = buttonStates_[i].rawState;
                    buttonStates_[i].stateChanged = true;

                    // Notify callback about state change
                    notifyButtonChange(i, buttonStates_[i].currentState);
                }
            }
        }
    }
}

void ButtonsController::notifyButtonChange(uint16_t buttonIndex, bool pressed)
{
    if (pressed)
    {
        Trace::out("ButtonsController: Button %u pressed", buttonIndex);
    }
    else
    {
        Trace::out("ButtonsController: Button %u released", buttonIndex);
    }

    // Call registered callback if available
    if (callbackProvider_ && callbackMethod_)
    {
        (callbackProvider_->*callbackMethod_)(buttonIndex, pressed);
    }
}

} // namespace board
