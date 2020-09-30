/*
	VibHub Button
*/
#ifndef OnOffController_h
#define OnOffController_h
#include "Configuration.h"


class OnOffController {
    public:
        OnOffController(void);
        
        static void setup();
        static void IRAM_ATTR onInterrupt();
        static uint8_t powerReleased();
        static const uint8_t POWER_STATE_HELD;  // Waiting to release button
        static const uint8_t POWER_STATE_RELEASED;  // Power button was just released
        static const uint8_t POWER_STATE_DONE;  // Release has been triggered
        

    private:
        static bool isPowerReleased;
};


#endif //ConfigButton_h
