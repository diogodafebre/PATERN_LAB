#ifndef INTERFACE_BUTTONEVENTSHANDLER_OBSERVER_H
#define INTERFACE_BUTTONEVENTSHANDLER_OBSERVER_H

#include "buttoneventshandlersubject.h"

class ButtonEventsHandler;

namespace interface {

/**
 * @brief Interface to be provided by the class who wants to receive button short/long pressed notifications.
 */
class ButtonEventsHandlerObserver
{
	friend class ButtonEventsHandlerSubject;
	friend class ::ButtonEventsHandler;

public:
	typedef ButtonEventsHandlerSubject::ButtonIndex ButtonIndex;

    virtual ~ButtonEventsHandlerObserver() {}

protected:
	virtual void onButtonShortPressed(ButtonIndex buttonIndex) = 0;         ///< @brief Called by the subject upon a button short pressed.
    virtual void onButtonLongPressed(ButtonIndex buttonIndex) = 0;          ///< @brief Called by the subject upon a button long pressed.

protected:
    ButtonEventsHandlerObserver() {}                                        ///< @brief Not allowing to instantiate object of interface.
};

} // namespace interface
#endif // INTERFACE_BUTTONEVENTSHANDLER_OBSERVER_H
