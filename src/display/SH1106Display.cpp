#include "SH1106Display.h"

namespace meshcamper
{
    SH1106Display::SH1106Display()
        : display(DISPLAY_I2C_ADDRESS, -1, -1, GEOMETRY_128_64, I2C_ONE)
    {
        LOG_DEBUG("waiting 2s for display.\n");
        delay(2000);

        LOG_DEBUG("Initializing display\n");
        display.init();
        LOG_DEBUG("Displaying welcome message\n");

        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(0, 0, "Welcome to");
        display.drawString(1, 0, "MeshCamper");
        display.display();

        strcpy(battery_line, "No Battery data.");
        strcpy(mesh_line, "No mesh data.");
        strcpy(camper_line, "No camper data.");
    }

    void SH1106Display::redraw()
    {
        display.clear();
        display.setFont(ArialMT_Plain_16);
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(0, 0, battery_line);
        display.drawString(0, 21, mesh_line);
        display.drawString(0, 42, camper_line);
        display.display();
    }

    void SH1106Display::update_battery_status(const float &soc, const float &power)
    {
        if (soc == 0.0f)
        {
            snprintf(battery_line, 32, "No battery data");
        }
        else
        {
            snprintf(battery_line, 32, "%3.1f%% %3.2fW", soc, power);
        }
    }

    void SH1106Display::update_mesh_status(const int &active_nodes)
    {
        snprintf(battery_line, 32, "%2d nodes in range", active_nodes);
    }

    SH1106Display::~SH1106Display()
    {
    }
}