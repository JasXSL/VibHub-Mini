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

    private:
        static uint32_t time_released;     // Time when it was first released, used for debounce
};


#endif //ConfigButton_h
