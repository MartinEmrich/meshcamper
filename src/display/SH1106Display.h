#pragma once

#include "Logging.h"
#include "Display.h"

#include <OLEDDisplay.h>
#include <SH1106Wire.h>

#ifndef DISPLAY_I2C_ADDRESS
#define DISPLAY_I2C_ADDRESS 0x3C
#endif

#define DISPLAY_I2C_SCL 22
#define DISPLAY_I2C_SDA 21

namespace meshcamper
{
    class SH1106Display : public Display
    {
    private:
        SH1106Wire display;

        /* for now: 3 text lines with data */
        char battery_line[32] = {0};
        char mesh_line[32] = {0};
        char camper_line[32] = {0};

    public:
        SH1106Display();

        virtual void redraw();

        virtual void update_battery_status(const float &soc, const float &power);

        virtual void update_mesh_status(const int &active_nodes);

        virtual ~SH1106Display();
    };
}