#pragma once

namespace meshcamper
{
    class Display
    {
    protected:
        Display();

    public:
        static Display *create();

        virtual void update_battery_status(const float &soc, const float &power) = 0;

        virtual void update_mesh_status(const int &active_nodes) = 0;

        virtual void redraw() = 0;

        virtual ~Display() = 0;
    };
}