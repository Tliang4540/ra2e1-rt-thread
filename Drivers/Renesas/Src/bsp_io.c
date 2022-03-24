#include "bsp_io.h"

void bsp_port_init(bsp_io_port_pin_menu port, 
                   bsp_io_peripheral_enum peripheral,
                   bsp_io_mode_enum mode)
{
    R_PMISC->PWPR = 0;
    R_PMISC->PWPR = 1<<R_PMISC_PWPR_PFSWE_Pos;
    
    R_PFS->PORT[port >> 8].PIN[port&0xFF].PmnPFS = peripheral | mode;

    R_PMISC->PWPR = 0;
    R_PMISC->PWPR = 1<<R_PMISC_PWPR_B0WI_Pos;
}