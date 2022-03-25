#include"OLED.h"
#include"zk.h"

void Write_Byte(uint8_t Data, uint8_t DC)
{
    uint32_t i;	
    
    if(DC)
        Set_DC;
    else
        Clr_DC;
    Clr_CS;
    
    for(i = 0;i < 8;i++)
    {
        Clr_CLK;
        if(Data & 0x80)
            Set_DIN;
        else
            Clr_DIN;
        __asm__("nop");
        Set_CLK;
        Data <<= 1;
    }

    Set_CS;
    Set_DC;
    Set_DIN;
}

void OLED_Switch(uint8_t s)
{
    Write_Byte((0xAE | s), 0);
}

void Set_ShowAddr(uint8_t y, uint8_t x)
{
    y = 0xb0 | (7-y);
    Write_Byte(y, 0);
    x+=2;
    Write_Byte(x & 0x0f, 0);
    Write_Byte(0x10 | (x >> 4), 0);
}

void OLED_Init(void)
{
    Write_Byte(0xAE, 0);			//display 0ff
    
    Write_Byte(0xD5, 0);
    Write_Byte(0x80, 0);

    Write_Byte(0xA8, 0);
    Write_Byte(0x3F, 0);
    
    Write_Byte(0xD3, 0);			
    Write_Byte(0x00, 0);

    Write_Byte(0x40, 0);

    Write_Byte(0x8D, 0);			//Charge Pump Setting
    Write_Byte(0x14, 0);

    Write_Byte(0xA1, 0);			//Set Segment Re-map.0xA0:0 to SEG0,0xA1:127 to SEG0
    
    Write_Byte(0xC0, 0);			//Set COM Output Scan Direction. 0xC0:COM0 to COM[N-1],0xC8:COM[N-1] to COM0
    
    Write_Byte(0x21, 0);
    Write_Byte(0x02, 0);
  
    Write_Byte(0xDA, 0);			//Set COM Pins Hardware Configuration
    Write_Byte(0x12, 0);
    
    Write_Byte(0x81, 0);			//Set Contrast Control
    Write_Byte(0xCF, 0);
    
    Write_Byte(0xD9, 0);			//Set Pre-charge Period
    Write_Byte(0xF1, 0);
    
    Write_Byte(0xDB, 0);			//Set VCOMH Deselect Level
    Write_Byte(0x30, 0);
    
    Write_Byte(0xA4, 0);			//Entire Display ON
    
    Write_Byte(0xA6, 0);			//Set Normal/Inverse Display
    
    //Write_Byte(0xAF, 0);			//display on
    
    //Cls(0xff);
  //OLED_Switch(0);
}

void Cls(void)
{
    uint32_t i, j;
    for(i = 0;i < 8;i++)
    {
        Set_ShowAddr(i, 0);
        for(j = 0;j < 128;j++)
        {
            Write_Byte(0x00, 1);
        }
    }
}

                       /*
void Show_Bmp(uint8 *bmp, uint8 sx, uint8 sy, uint8 x, uint8 y)
{
    uint8 i, j;
    for(i = sy;i < y;i++)
    {
        Set_ShowAddr(i, sx);
        for(j = 0;j < x - sx;j++)
        {
            Write_Byte(bmp[(i * (x - sx)) + j], 1);
        }
    }
}
                */

void ShowBrand(void)
{
    uint32_t i, j;
    uint32_t k = 0;

    for(i = 0;i < 8;i++)
    {
        Set_ShowAddr(i, 0);
        for(j = 0;j < 128;j++)
        {
            Write_Byte(Brand[k++], 1);
        }
    }
}

void ShowPower(uint8_t vol, uint8_t x, uint8_t y)
{
    Set_ShowAddr(y, x);
    Write_Byte(0x0f, 1);
    if(vol > 0)
        Write_Byte(0x0f, 1);
    else
        Write_Byte(0x08, 1);
    if(vol > 1)
        Write_Byte(0x0f, 1);
    else
        Write_Byte(0x08, 1);
    if(vol > 2)
        Write_Byte(0x0f, 1);
    else
        Write_Byte(0x08, 1);
    if(vol > 3)
        Write_Byte(0x0f, 1);
    else
        Write_Byte(0x08, 1);
    if(vol > 4)
        Write_Byte(0x0f, 1);
    else
        Write_Byte(0x08, 1);
    if(vol > 5)
        Write_Byte(0x0f, 1);
    else
        Write_Byte(0x08, 1);
    if(vol > 6)
        Write_Byte(0x0f, 1);
    else
        Write_Byte(0x08, 1);
    if(vol > 7)
        Write_Byte(0x0f, 1);
    else
        Write_Byte(0x08, 1);
    if(vol > 8)
        Write_Byte(0x0f, 1);
    else
        Write_Byte(0x08, 1);
    if(vol > 9)
        Write_Byte(0x0f, 1);
    else
        Write_Byte(0x08, 1);
    if(vol > 10)
        Write_Byte(0x0f, 1);
    else
        Write_Byte(0x08, 1);
    if(vol > 11)
        Write_Byte(0x0f, 1);
    else
        Write_Byte(0x08, 1);
    Write_Byte(0x0f, 1);
    Write_Byte(0x03, 1);
    Write_Byte(0x03, 1);
    Set_ShowAddr(y+1, x);
    Write_Byte(0xf0, 1);
    if(vol > 0)
        Write_Byte(0xf0, 1);
    else
        Write_Byte(0x10, 1);
    if(vol > 1)
        Write_Byte(0xf0, 1);
    else
        Write_Byte(0x10, 1);
    if(vol > 2)
        Write_Byte(0xf0, 1);
    else
        Write_Byte(0x10, 1);
    if(vol > 3)
        Write_Byte(0xf0, 1);
    else
        Write_Byte(0x10, 1);
    if(vol > 4)
        Write_Byte(0xf0, 1);
    else
        Write_Byte(0x10, 1);
    if(vol > 5)
        Write_Byte(0xf0, 1);
    else
        Write_Byte(0x10, 1);
    if(vol > 6)
        Write_Byte(0xf0, 1);
    else
        Write_Byte(0x10, 1);
    if(vol > 7)
        Write_Byte(0xf0, 1);
    else
        Write_Byte(0x10, 1);
    if(vol > 8)
        Write_Byte(0xf0, 1);
    else
        Write_Byte(0x10, 1);
    if(vol > 9)
        Write_Byte(0xf0, 1);
    else
        Write_Byte(0x10, 1);
    if(vol > 10)
        Write_Byte(0xf0, 1);
    else
        Write_Byte(0x10, 1);
    if(vol > 11)
        Write_Byte(0xf0, 1);
    else
        Write_Byte(0x10, 1);
    Write_Byte(0xF0, 1);
    Write_Byte(0xC0, 1);
    Write_Byte(0xC0, 1);
}

void Display(char *str, uint8_t x, uint8_t y)	 
{
    uint32_t temp, i;
    uint32_t addr;
    while(*str != 0)
    {
        Set_ShowAddr(y, x);
        if(str[0] == '\n')
        {
            if(y < 6) y += 2;
            else y = 0;
            x = 0;
        }
        else if((uint8_t)*str > 127)
        {
            //搜索字库,如果没有找到,显示最后一个字符'口'
            for(temp = 0;temp < (sizeof(GB16) / sizeof(GB16[0])) - 1;temp++)    
            {
                if((str[0] == GB16[temp].Index[0]) && (str[1] == GB16[temp].Index[1]) && (str[2] == GB16[temp].Index[2]))
                    break;
            }

            for(i = 0;i < 16;i++)
            {
                Write_Byte(GB16[temp].Msk[i], 1);
            }
            Set_ShowAddr(y + 1, x);
            for(;i < 32;i++)
            {
                Write_Byte(GB16[temp].Msk[i], 1);
            }
            x += 16;
            str+=2;
        }
        else
        {
            addr = 16 * (str[0] - 32);
            for(i = 0;i < 8;i++)
            {
                Write_Byte(ASCII_Tab[addr + i], 1);
            }
            Set_ShowAddr(y + 1, x);
            for(;i < 16;i++)
            {
                Write_Byte(ASCII_Tab[addr + i], 1);
            }
            x += 8;
        }
        str++;
    }
}
