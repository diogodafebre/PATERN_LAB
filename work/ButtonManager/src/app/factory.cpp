// factory.cpp
#include "trace/trace.h"
#include "board/board.h"
#if defined(CONFIG_LVGL) && (CONFIG_LVGL != 0)
    #include <lvgl.h>
    #include <lvgl_input_device.h>
    #include <zephyr/kernel.h>
    #include <zephyr/drivers/display.h>
#endif
#if defined(CONFIG_DISK_ACCESS) && (CONFIG_DISK_ACCESS != 0)
    #include <zephyr/storage/disk_access.h>
    #include <zephyr/fs/fs.h>
    #include <ff.h>
#endif
#include "factory.h"

namespace app
{

mdw::button::ButtonEventsHandler Factory::_buttonEventsHandler;
ButtonEventsLogger Factory::_buttonEventsLogger;

Factory::Factory() {}

// static
void Factory::initialize()
{
    Trace::initialize();
    board::initialize();

    Trace::out("Factory: Initializing app components...");

    _buttonEventsHandler.initialize();
    if (!_buttonEventsLogger.initialize(_buttonEventsHandler))
    {
        Trace::out("Factory: Failed to register ButtonEventsLogger with handler");
    }

#if defined(CONFIG_LVGL) && (CONFIG_LVGL != 0)
    getGuiTask().initialize(_buttonEventsHandler);
#endif
}

// static
void Factory::build()
{
    Trace::out("Factory: Starting app components...");

#if defined(CONFIG_LVGL) && (CONFIG_LVGL != 0)
    getGuiTask().start();
#endif

    Trace::out("Factory: App ready");
}

#if defined(CONFIG_LVGL) && (CONFIG_LVGL != 0)
GuiTask & Factory::getGuiTask()
{
    static GuiTask guiTask;
    return guiTask;
}
#endif

} // namespace app

void Factory_initialize() { app::Factory::initialize(); }
void Factory_build() { app::Factory::build(); }
