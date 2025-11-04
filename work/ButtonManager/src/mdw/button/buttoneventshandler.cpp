#include "buttoneventshandler.h"
#include "buttonstatesm.h"
#include "interface/buttoneventshandlerobserver.h"
#include "board/buttonscontroller.h"
#include "trace/trace.h"
#include "xf/customevent.h"

namespace mdw {
namespace button {

/**
 * @brief Internal event carrying button index for short press notification
 */
class evInternalShortPress : public XFCustomEvent
{
public:
    evInternalShortPress(uint8_t buttonIndex, interface::XFBehavior * pBehavior) :
        XFCustomEvent(ButtonEventsHandler::evNotifyShortPress, pBehavior),
        buttonIndex(buttonIndex)
    {}

    uint8_t buttonIndex;
};

/**
 * @brief Internal event carrying button index for long press notification
 */
class evInternalLongPress : public XFCustomEvent
{
public:
    evInternalLongPress(uint8_t buttonIndex, interface::XFBehavior * pBehavior) :
        XFCustomEvent(ButtonEventsHandler::evNotifyLongPress, pBehavior),
        buttonIndex(buttonIndex)
    {}

    uint8_t buttonIndex;
};

ButtonEventsHandler::ButtonEventsHandler()
: XFBehavior(/* active = */ true, /* threadName = */ "ButtonEventsHandler")
, observerCount_(0)
{
    // Initialize observer array
    for (uint8_t i = 0; i < MAX_OBSERVERS; ++i)
    {
        observers_[i] = nullptr;
    }

    // Create ButtonStateSm instances for each button
    for (uint8_t i = 0; i < BUTTON_COUNT; ++i)
    {
        buttonStateMachines_[i] = new ButtonStateSm(i, *this);
    }
}

ButtonEventsHandler::~ButtonEventsHandler()
{
    // Delete ButtonStateSm instances
    for (uint8_t i = 0; i < BUTTON_COUNT; ++i)
    {
        if (buttonStateMachines_[i])
        {
            delete buttonStateMachines_[i];
            buttonStateMachines_[i] = nullptr;
        }
    }
}

bool ButtonEventsHandler::initialize()
{
    Trace::out("ButtonEventsHandler: Initializing...");

    // Register this handler as callback provider with ButtonsController
    board::ButtonsController & controller = board::ButtonsController::getInstance();
    if (!controller.registerCallback(this, &ButtonEventsHandler::onButtonChanged))
    {
        Trace::out("ButtonEventsHandler: Failed to register callback with ButtonsController");
        return false;
    }

    // Start the behavior (XF active object)
    startBehavior();

    // Start all button state machines
    for (uint8_t i = 0; i < BUTTON_COUNT; ++i)
    {
        buttonStateMachines_[i]->startBehavior();
    }

    Trace::out("ButtonEventsHandler: Initialized successfully");
    return true;
}

void ButtonEventsHandler::onButtonChanged(uint16_t buttonIndex, bool pressed)
{
    if (buttonIndex >= BUTTON_COUNT)
    {
        return;
    }

    Trace::out("ButtonEventsHandler: Button %u %s", buttonIndex, pressed ? "pressed" : "released");

    // Forward to appropriate ButtonStateSm
    if (pressed)
    {
        buttonStateMachines_[buttonIndex]->onButtonPressed();
    }
    else
    {
        buttonStateMachines_[buttonIndex]->onButtonReleased();
    }
}

void ButtonEventsHandler::onButtonShortPressDetected(uint8_t buttonIndex)
{
    // Push internal event to decouple from ButtonStateSm
    GEN(evInternalShortPress(buttonIndex, this));
}

void ButtonEventsHandler::onButtonLongPressDetected(uint8_t buttonIndex)
{
    // Push internal event to decouple from ButtonStateSm
    GEN(evInternalLongPress(buttonIndex, this));
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
        Trace::out("ButtonEventsHandler: State machine started");
        return XFEventStatus::Consumed;
    }

    if (getCurrentEvent()->getEventType() == XFEvent::Event)
    {
        switch (getCurrentEvent()->getId())
        {
        case evNotifyShortPress:
            {
                const evInternalShortPress * event = static_cast<const evInternalShortPress *>(getCurrentEvent());
                notifyButtonShortPressed(event->buttonIndex);
                return XFEventStatus::Consumed;
            }

        case evNotifyLongPress:
            {
                const evInternalLongPress * event = static_cast<const evInternalLongPress *>(getCurrentEvent());
                notifyButtonLongPressed(event->buttonIndex);
                return XFEventStatus::Consumed;
            }

        default:
            break;
        }
    }

    return XFEventStatus::Unknown;
}

} // namespace button
} // namespace mdw
