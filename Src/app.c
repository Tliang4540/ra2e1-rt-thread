#include "app.h"

void inpass_thread_entry(void *parameter)
{
    rt_thread_delay(1000);
    Cls();
    Display("plz input pwd:", 0, 0);

    while (1)
    {
        Display("_", 0, 2);
        rt_thread_delay(500);
        Display(" ", 0, 2);
        rt_thread_delay(500);
    }
    
}
