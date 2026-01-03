#include "Display.h"

#include "SH1106Display.h"

namespace meshcamper
{
    Display::Display() {

    }
    
    Display *Display::create()
    {
        /* for now: only choice */
        return new SH1106Display();
    }

    void Display::redraw() {
        
    }

    Display::~Display() {

    }
}