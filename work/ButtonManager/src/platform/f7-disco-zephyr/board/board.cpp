#include "hal/buttons.h"
#include "buttonscontroller.h"
#include "board.h"

namespace board {

// Instantiate ButtonsController singleton
static ButtonsController buttonsController;

void initialize()
{
    // Initialize ButtonsController (which will initialize hal::buttons internally)
    buttonsController.initialize();
}

} // namespace board
