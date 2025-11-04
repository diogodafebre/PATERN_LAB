#include "buttoneventshandler.h"
#include "interface/buttoneventshandlerobserver.h"
#include "board/hal/buttons.h"
#include "trace/trace.h"
#include "xf/customevent.h"
#include <zephyr/kernel.h>

namespace mdw {
namespace button {

ButtonEventsHandler::ButtonEventsHandler()
: XFBehavior(/* active = */ true, /* threadName = */ "ButtonEventsHandler")
, observerCount_(0)
{
    // Initialize observer array
    for (uint8_t i = 0; i < MAX_OBSERVERS; ++i)
    {
        observers_[i] = nullptr;
    }

    // Initialize button states
    for (uint8_t i = 0; i < BUTTON_COUNT; ++i)
    {
        buttonStates_[i].pressed = false;
        buttonStates_[i].previousPressed = false;
        buttonStates_[i].pressStartTime = 0;
        buttonStates_[i].longPressNotified = false;
    }
}

ButtonEventsHandler::~ButtonEventsHandler()
{
}

bool ButtonEventsHandler::initialize()
{
    Trace::out("ButtonEventsHandler: Initializing...");

    // Initialize button HAL
    if (!board::hal::buttons::initialize())
    {
        Trace::out("ButtonEventsHandler: Failed to initialize button HAL");
        return false;
    }

    // Start the behavior (XF active object)
    startBehavior();

    Trace::out("ButtonEventsHandler: Initialized successfully");
    return true;
}

void ButtonEventsHandler::onIrq()
{
    // Called from button interrupt - push event to check buttons
    pushEvent(evIrqReceived);
}

bool ButtonEventsHandler::subscribe(interface::ButtonEventsHandlerObserver * observer)
{
    if (observer == nullptr)
    {
        return false;
    }

    // Check if already subscribed
    for (uint8_t i = 0; i < observerCount_; ++i)
    {
        if (observers_[i] == observer)
        {
            return true; // Already subscribed
        }
    }

    // Add new observer if space available
    if (observerCount_ < MAX_OBSERVERS)
    {
        observers_[observerCount_] = observer;
        observerCount_++;
        Trace::out("ButtonEventsHandler: Observer subscribed (total: %u)", observerCount_);
        return true;
    }

    Trace::out("ButtonEventsHandler: Cannot subscribe - maximum observers reached");
    return false;
}

void ButtonEventsHandler::unsubscribe(interface::ButtonEventsHandlerObserver * observer)
{
    if (observer == nullptr)
    {
        return;
    }

    // Find and remove observer
    for (uint8_t i = 0; i < observerCount_; ++i)
    {
        if (observers_[i] == observer)
        {
            // Shift remaining observers down
            for (uint8_t j = i; j < observerCount_ - 1; ++j)
            {
                observers_[j] = observers_[j + 1];
            }
            observers_[observerCount_ - 1] = nullptr;
            observerCount_--;
            Trace::out("ButtonEventsHandler: Observer unsubscribed (total: %u)", observerCount_);
            return;
        }
    }
}

void ButtonEventsHandler::notifyButtonShortPressed(ButtonIndex buttonIndex)
{
    Trace::out("ButtonEventsHandler: Notifying short press on button %u to %u observers",
               static_cast<unsigned int>(buttonIndex), observerCount_);

    for (uint8_t i = 0; i < observerCount_; ++i)
    {
        if (observers_[i] != nullptr)
        {
            observers_[i]->onButtonShortPressed(buttonIndex);
        }
    }
}

void ButtonEventsHandler::notifyButtonLongPressed(ButtonIndex buttonIndex)
{
    Trace::out("ButtonEventsHandler: Notifying long press on button %u to %u observers",
               static_cast<unsigned int>(buttonIndex), observerCount_);

    for (uint8_t i = 0; i < observerCount_; ++i)
    {
        if (observers_[i] != nullptr)
        {
            observers_[i]->onButtonLongPressed(buttonIndex);
        }
    }
}

XFEventStatus ButtonEventsHandler::processEvent()
{
    if (getCurrentEvent()->getEventType() == XFEvent::Initial)
    {
        // Start periodic button checking
        pushEvent(evButtonCheck, 10); // Check every 10ms
        return XFEventStatus::Consumed;
    }

    if (getCurrentEvent()->getEventType() == XFEvent::Event)
    {
        switch (getCurrentEvent()->getId())
        {
        case evButtonCheck:
            checkButtons();
            pushEvent(evButtonCheck, 10); // Schedule next check
            return XFEventStatus::Consumed;

        case evIrqReceived:
            // Button IRQ received - check buttons immediately
            checkButtons();
            return XFEventStatus::Consumed;

        default:
            break;
        }
    }

    return XFEventStatus::Unknown;
}

void ButtonEventsHandler::checkButtons()
{
    uint32_t currentTime = k_uptime_get_32();

    for (uint8_t i = 0; i < BUTTON_COUNT; ++i)
    {
        // Read current button state (active LOW)
        bool currentPressed = !board::hal::buttons::isButtonPressed(i);

        // Detect button press (transition from not pressed to pressed)
        if (currentPressed && !buttonStates_[i].previousPressed)
        {
            handleButtonPress(i);
        }
        // Detect button release (transition from pressed to not pressed)
        else if (!currentPressed && buttonStates_[i].previousPressed)
        {
            handleButtonRelease(i);
        }
        // Button held - check for long press
        else if (currentPressed && buttonStates_[i].pressed)
        {
            uint32_t pressDuration = currentTime - buttonStates_[i].pressStartTime;
            if (pressDuration >= LONG_PRESS_DURATION_MS && !buttonStates_[i].longPressNotified)
            {
                notifyButtonLongPressed(i);
                buttonStates_[i].longPressNotified = true;
            }
        }

        buttonStates_[i].previousPressed = currentPressed;
    }
}

void ButtonEventsHandler::handleButtonPress(ButtonIndex buttonIndex)
{
    uint32_t currentTime = k_uptime_get_32();

    Trace::out("ButtonEventsHandler: Button %u pressed", static_cast<unsigned int>(buttonIndex));

    buttonStates_[buttonIndex].pressed = true;
    buttonStates_[buttonIndex].pressStartTime = currentTime;
    buttonStates_[buttonIndex].longPressNotified = false;
}

void ButtonEventsHandler::handleButtonRelease(ButtonIndex buttonIndex)
{
    uint32_t currentTime = k_uptime_get_32();
    uint32_t pressDuration = currentTime - buttonStates_[buttonIndex].pressStartTime;

    Trace::out("ButtonEventsHandler: Button %u released (duration: %u ms)",
               static_cast<unsigned int>(buttonIndex), pressDuration);

    // Only notify short press if long press was not already notified
    if (!buttonStates_[buttonIndex].longPressNotified &&
        pressDuration >= DEBOUNCE_TIME_MS)
    {
        notifyButtonShortPressed(buttonIndex);
    }

    buttonStates_[buttonIndex].pressed = false;
}

} // namespace button
} // namespace mdw
