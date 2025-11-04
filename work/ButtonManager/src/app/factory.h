// factory.h
#ifndef APP_FACTORY_H
#define APP_FACTORY_H

#ifdef __cplusplus
#if defined(CONFIG_LVGL) && (CONFIG_LVGL != 0)
    #include "gui-task.h"
#endif

#include "app/buttoneventslogger.h"
#include "mdw/button/buttoneventshandler.h"

namespace app {

class Factory
{
public:
    Factory();
    static void initialize();
    static void build();

#if defined(CONFIG_LVGL) && (CONFIG_LVGL != 0)
    static GuiTask & getGuiTask();
#endif

protected:
    static mdw::button::ButtonEventsHandler _buttonEventsHandler;
    static ButtonEventsLogger _buttonEventsLogger;
};

} // namespace app
#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif
void Factory_initialize();
void Factory_build();
#ifdef __cplusplus
}
#endif

#endif // APP_FACTORY_H
