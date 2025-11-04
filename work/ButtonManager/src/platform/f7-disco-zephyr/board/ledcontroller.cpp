#include "mcu/mcu.h"
#include "config/ledcontroller-config.h"
#if (LEDCONTROLLER_TRACE_ENABLE != 0)
    #include "trace/trace.h"
#endif // LEDCONTROLLER_TRACE_ENABLE
#include <zephyr/drivers/gpio.h>
#include "ledcontroller.h"

#define EXT_LED0_NODE DT_ALIAS(ext_led0)
#define EXT_LED1_NODE DT_ALIAS(ext_led1)
#define EXT_LED2_NODE DT_ALIAS(ext_led2)
#define EXT_LED3_NODE DT_ALIAS(ext_led3)

static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(EXT_LED0_NODE, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(EXT_LED1_NODE, gpios);
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(EXT_LED2_NODE, gpios);
static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(EXT_LED3_NODE, gpios);

LedController * LedController::pInstance_ = nullptr;

LedController::LedController()
{
  	assert(!pInstance_);	// Only one instance of this class allowed!
	pInstance_ = this;

	// Initialize the method array with the right methods.
	ledOperation_[0] = &LedController::setLed0;
	ledOperation_[1] = &LedController::setLed1;
	ledOperation_[2] = &LedController::setLed2;
	ledOperation_[3] = &LedController::setLed3;

	/*
	 * F7-DISCO LED GPIOs (Extension Board):
	 * LED0: PA15
	 * LED1: PH6
	 * LED2: PA8
	 * LED3: PB4
	 */

	gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&led1, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&led2, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&led3, GPIO_OUTPUT_INACTIVE);
}

LedController::~LedController()
{
}

void LedController::setLed(uint8_t index, bool bOn)
{
	setLeds(0x01 << index, bOn);
}

void LedController::setLeds(uint8_t ledMask, bool bOn)
{
	uint8_t mask = 0x01;

	for (uint8_t i = 0; i < ledCount(); i++, mask <<= 1)
	{
		if ((ledMask & mask) == mask && ledOperation_[i])
		{
			(this->*ledOperation_[i])(bOn);
		}
	}
}

void LedController::setLed0(bool bOn /* = true */)
{
	if (bOn)
	{
		gpio_pin_set(led0.port, led0.pin, 1);
	}
	else
	{
		gpio_pin_set(led0.port, led0.pin, 0);
	}

#if (LEDCONTROLLER_TRACE_ENABLE != 0)
	if (bOn)
	{
		// Not using "%s" here (bug in gcc c-library!)
		Trace::out(" LED0: on");
	}
	else
	{
		Trace::out(" LED0: off");
	}
#endif // LEDCONTROLLER_TRACE_ENABLE
}

void LedController::setLed1(bool bOn /* = true */)
{
    if (bOn)
    {
		gpio_pin_set(led1.port, led1.pin, 1);
    }
    else
    {
		gpio_pin_set(led1.port, led1.pin, 0);
    }

#if (LEDCONTROLLER_TRACE_ENABLE != 0)
    if (bOn)
	{
		Trace::out("  LED1: on");
	}
	else
	{
		Trace::out("  LED1: off");
	}
#endif // LEDCONTROLLER_TRACE_ENABLE
}

void LedController::setLed2(bool bOn /* = true */)
{
    if (bOn)
    {
		gpio_pin_set(led2.port, led2.pin, 1);
    }
    else
    {
		gpio_pin_set(led2.port, led2.pin, 0);
    }

#if (LEDCONTROLLER_TRACE_ENABLE != 0)
    if (bOn)
	{
		Trace::out("   LED2: on");
	}
	else
	{
		Trace::out("   LED2: off");
	}
#endif // LEDCONTROLLER_TRACE_ENABLE
}

void LedController::setLed3(bool bOn /* = true */)
{
    if (bOn)
    {
		gpio_pin_set(led3.port, led3.pin, 1);
    }
    else
    {
		gpio_pin_set(led3.port, led3.pin, 0);
    }

#if (LEDCONTROLLER_TRACE_ENABLE != 0)
    if (bOn)
	{
		Trace::out("    LED3: on");
	}
	else
	{
		Trace::out("    LED3: off");
	}
#endif // LEDCONTROLLER_TRACE_ENABLE
}
