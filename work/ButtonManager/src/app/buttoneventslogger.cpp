// buttoneventslogger.cpp
#include "buttoneventslogger.h"
#include "trace/trace.h"
#include "interface/buttoneventshandlersubject.h"

namespace app
{

ButtonEventsLogger::ButtonEventsLogger()
: subject_(nullptr)
, subscribed_(false)
{}

ButtonEventsLogger::~ButtonEventsLogger()
{
    deinitialize();
}

bool ButtonEventsLogger::initialize(interface::ButtonEventsHandlerSubject & subject)
{
    if (subscribed_) { return true; }

    if (subject.subscribe(this))
    {
        subject_ = &subject;
        subscribed_ = true;
        return true;
    }
    return false;
}

void ButtonEventsLogger::deinitialize()
{
    if (subject_ && subscribed_)
    {
        subject_->unsubscribe(this);
    }
    subject_ = nullptr;
    subscribed_ = false;
}

void ButtonEventsLogger::onButtonShortPressed(ButtonIndex buttonIndex)
{
    Trace::out("Button %u short pressed", static_cast<unsigned int>(buttonIndex));
}

void ButtonEventsLogger::onButtonLongPressed(ButtonIndex buttonIndex)
{
    Trace::out("Button %u long pressed", static_cast<unsigned int>(buttonIndex));
}

} // namespace app
