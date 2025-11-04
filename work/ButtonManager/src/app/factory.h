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
#ifndef APP_FACTORY_H
#define APP_FACTORY_H

#include "interface/buttoneventshandlersubject.h"  // <- on garde UNIQUEMENT Subject
#include "app/buttoneventslogger.h"

namespace app {

class Factory
{
public:
    static void initialize();
    static void build();

private:
    // On maintient un subject interne (si tu as déjà un subject global ailleurs,
    // tu peux l'y brancher en changeant l'instance ici).
    static interface::ButtonEventsHandlerSubject s_buttonSubject;
    static ButtonEventsLogger                   s_buttonLogger;
};

} // namespace app

#ifdef __cplusplus
extern "C" {
#endif
void Factory_initialize();
void Factory_build();
#ifdef __cplusplus
}
#endif

#endif // APP_FACTORY_H

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
