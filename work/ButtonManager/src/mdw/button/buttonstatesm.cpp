#include "buttonstatesm.h"
#include "buttoneventshandler.h"
#include "trace/trace.h"

namespace mdw {
namespace button {

ButtonStateSm::ButtonStateSm(uint8_t buttonIndex, ButtonEventsHandler & handler)
: XFBehavior(/* active = */ true, /* threadName = */ nullptr)  // Non-active behavior
, buttonIndex_(buttonIndex)
, handler_(handler)
, currentState_(STATE_IDLE)
{
}

ButtonStateSm::~ButtonStateSm()
{
}

void ButtonStateSm::onButtonPressed()
{
    pushEvent(evButtonPressed);
}

void ButtonStateSm::onButtonReleased()
{
    pushEvent(evButtonReleased);
}

XFEventStatus ButtonStateSm::processEvent()
{
    // Initial event
    if (getCurrentEvent()->getEventType() == XFEvent::Initial)
    {
        enterIdle();
        return XFEventStatus::Consumed;
    }

    // Process events based on current state
    if (getCurrentEvent()->getEventType() == XFEvent::Event)
    {
        switch (currentState_)
        {
        case STATE_IDLE:
            if (getCurrentEvent()->getId() == evButtonPressed)
            {
                enterPressed();
                return XFEventStatus::Consumed;
            }
            break;

        case STATE_PRESSED:
            if (getCurrentEvent()->getId() == evButtonReleased)
            {
                // Short press detected
                Trace::out("ButtonStateSm[%u]: Short press detected", buttonIndex_);
                handler_.onButtonShortPressDetected(buttonIndex_);
                enterIdle();
                return XFEventStatus::Consumed;
            }
            else if (getCurrentEvent()->getId() == evLongPressTimeout)
            {
                // Long press detected
                Trace::out("ButtonStateSm[%u]: Long press detected", buttonIndex_);
                handler_.onButtonLongPressDetected(buttonIndex_);
                enterLongPressDetected();
                return XFEventStatus::Consumed;
            }
            break;

        case STATE_LONG_PRESS_DETECTED:
            if (getCurrentEvent()->getId() == evButtonReleased)
            {
                // Button released after long press
                Trace::out("ButtonStateSm[%u]: Released after long press", buttonIndex_);
                enterIdle();
                return XFEventStatus::Consumed;
            }
            break;

        default:
            break;
        }
    }

    return XFEventStatus::Unknown;
}

void ButtonStateSm::enterIdle()
{
    currentState_ = STATE_IDLE;
    Trace::out("ButtonStateSm[%u]: -> IDLE", buttonIndex_);
}

void ButtonStateSm::enterPressed()
{
    currentState_ = STATE_PRESSED;
    Trace::out("ButtonStateSm[%u]: -> PRESSED", buttonIndex_);

    // Start long press timer
    pushEvent(evLongPressTimeout, LONG_PRESS_DURATION_MS);
}

void ButtonStateSm::enterLongPressDetected()
{
    currentState_ = STATE_LONG_PRESS_DETECTED;
    Trace::out("ButtonStateSm[%u]: -> LONG_PRESS_DETECTED", buttonIndex_);
}

} // namespace button
} // namespace mdw
