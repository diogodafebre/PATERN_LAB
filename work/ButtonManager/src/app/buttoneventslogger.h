#ifndef APP_BUTTONEVENTSLOGGER_H
#define APP_BUTTONEVENTSLOGGER_H

#include "interface/buttoneventshandlerobserver.h"

namespace interface {
class ButtonEventsHandlerSubject;
} // namespace interface

namespace app
{

/**
 * @brief Logs button events received from the ButtonEventsHandler.
 */
class ButtonEventsLogger : public interface::ButtonEventsHandlerObserver
{
public:
    ButtonEventsLogger();
    ~ButtonEventsLogger();

    bool initialize(interface::ButtonEventsHandlerSubject & subject);
    void deinitialize();

protected:
    void onButtonShortPressed(ButtonIndex buttonIndex) override;
    void onButtonLongPressed(ButtonIndex buttonIndex) override;

private:
    interface::ButtonEventsHandlerSubject * subject_;
    bool subscribed_;
};

} // namespace app

#endif // APP_BUTTONEVENTSLOGGER_H