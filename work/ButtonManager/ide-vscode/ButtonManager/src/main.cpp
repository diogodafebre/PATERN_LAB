#include "board/board.h"
#include "xf/xf.h"
#include "app/factory.h"

int main(int argc, char *argv[])
{
    XF::initialize(10, argc, argv);

    app::Factory factory;

    factory.initialize();
    factory.build();

    return XF::exec();
}
