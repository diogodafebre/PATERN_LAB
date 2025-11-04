#ifndef INTERFACE_BUTTONEVENTSHANDLER_SUBJECT_H
#define INTERFACE_BUTTONEVENTSHANDLER_SUBJECT_H

#include <stdint.h>

namespace interface {

class ButtonEventsHandlerObserver;

/**
 * @brief Interface for the class who handles and notifies button short/long pressed events.
 */
class ButtonEventsHandlerSubject
{
public:
	typedef uint8_t ButtonIndex;

	virtual ~ButtonEventsHandlerSubject() {}

public:
	virtual bool subscribe(ButtonEventsHandlerObserver * observer) = 0;			    ///< @brief Registers a new observer. Returns true on success.
	virtual void unsubscribe(ButtonEventsHandlerObserver * observer) = 0;	        ///< @brief Un-subscribes a registered observer.

protected:
	virtual void notifyButtonShortPressed(ButtonIndex buttonIndex) = 0;	            ///< @brief Notifies observers about a button short pressed.
    virtual void notifyButtonLongPressed(ButtonIndex buttonIndex) = 0;              ///< @brief Notifies observers about a button long pressed.

protected:
    ButtonEventsHandlerSubject() {}                                                 ///< @brief Not allowing to instantiate object of interface.
};

} // namespace interface
#endif // INTERFACE_BUTTONEVENTSHANDLER_SUBJECT_H
