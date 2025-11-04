#ifndef BOARD_LEDCONTROLLER_H
#define BOARD_LEDCONTROLLER_H

#include <cassert>
#include <cstdint>

class LedController
{
public:
	#define LED_COUNT	4

public:
	LedController();
	virtual ~LedController();

	inline static LedController & getInstance() { assert(pInstance_); return *pInstance_; }

	void setLed(uint8_t index, bool bOn = true);
	void setLeds(uint8_t ledMask, bool bOn = true);

	inline uint8_t ledCount() const { return LED_COUNT; }

	void setLed0(bool bOn = true);
	void setLed1(bool bOn = true);
	void setLed2(bool bOn = true);
	void setLed3(bool bOn = true);

protected:
	typedef void (LedController::*ledMethod)(bool bOn);	///< Function prototype to led operation. Used for fast access to LED operation.

protected:
	static LedController * pInstance_;

	ledMethod ledOperation_[LED_COUNT];		///< Array of pointers to led functions.
};

#endif // BOARD_LEDCONTROLLER_H
