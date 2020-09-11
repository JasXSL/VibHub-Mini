/*
	VibHub Button
*/
#ifndef OnOffController_h
#define OnOffController_h
#include "Configuration.h"


class OnOffController {
    public:
        OnOffController(void);
        
        void setup();
        void loop();

    private:
        uint32_t time_released = 0;     // Time when it was first released, used for debounce
};

extern OnOffController onOffController;

#endif //ConfigButton_h
