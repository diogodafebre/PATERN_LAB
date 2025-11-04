#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
// #include "board/buttonscontroller.h"
#include "buttons.h"

constexpr const uint32_t BUTTONS_COUNT = 4;
static struct gpio_dt_spec extButton0 = GPIO_DT_SPEC_GET(DT_ALIAS(ext_btn0), gpios);
static struct gpio_dt_spec extButton1 = GPIO_DT_SPEC_GET(DT_ALIAS(ext_btn1), gpios);
static struct gpio_dt_spec extButton2 = GPIO_DT_SPEC_GET(DT_ALIAS(ext_btn2), gpios);
static struct gpio_dt_spec extButton3 = GPIO_DT_SPEC_GET(DT_ALIAS(ext_btn3), gpios);

// Every button needs its own 'gpio_callback' structure
static struct gpio_callback buttonCallbackHandler[BUTTONS_COUNT];

/** 
 * @brief Callback function called upon button interrupt.
 * 
 * Every button interrupt gets muxed into this function (common callback handler).
 */
static void onButtonPressed_(const struct device * port, struct gpio_callback * cb, gpio_port_pins_t pins);

namespace board {
namespace hal {
namespace buttons {

static struct gpio_dt_spec * button[BUTTONS_COUNT];     ///< Internal button array pointing to buttons defined in the Zephyr DT.

static bool configure(struct gpio_dt_spec & button, struct gpio_callback & callbackHandlder);

bool initialize()
{
    bool success = true;

    // Configure each button and register the onButtonPressed_(...) callback function as callback function
    success &= configure(extButton0, buttonCallbackHandler[0]);
    success &= configure(extButton1, buttonCallbackHandler[1]);
    success &= configure(extButton2, buttonCallbackHandler[2]);
    success &= configure(extButton3, buttonCallbackHandler[3]);

    // Initialize internal button pointer array
    button[0] = &extButton0;
    button[1] = &extButton1;
    button[2] = &extButton2;
    button[3] = &extButton3;

    return success;
}

bool isButtonPressed(uint16_t buttonIndex)
{
    return (buttonIndex < BUTTONS_COUNT) ? gpio_pin_get_dt(button[buttonIndex]) : false;
}

bool configure(struct gpio_dt_spec & button, struct gpio_callback & callbackHandlder)
{
    int ret;

    if (!gpio_is_ready_dt(&button))
    {
        return false;
    }

    // Configure button gpio as input
    ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    if (ret != 0)
    {
        printk("Error %d: Failed to configure %s pin %d\n", ret, button.port->name, button.pin);
        return false;
    }

    // Enable interrupt on button for rising edge
    ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_BOTH);
    if (ret != 0)
    {
        printk("Error %d: Failed to configure interrupt on %s pin %d\n", ret, button.port->name, button.pin);
        return false;
    }

    // Initialize callback structure for button interrupt
    gpio_init_callback(&callbackHandlder, onButtonPressed_, BIT(button.pin));

    // Attach callback function to button interrupt
    gpio_add_callback_dt(&button, &callbackHandlder);

    return true;
}

} // namespace buttons
} // namespace hal
} // namespace board

void onButtonPressed_(const struct device * port, struct gpio_callback * cb, gpio_port_pins_t pins)
{
    // TODO: Uncomment code below
    //ButtonsController::getInstance().onIrq();
}