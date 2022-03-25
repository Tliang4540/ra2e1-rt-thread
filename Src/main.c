#include "ra2e1.h"
#include "rtthread.h"
#include "uart.h"
#include "OLED.h"
#include "app.h"

#define LED_PORT    R_PORT1
#define LED_PIN     BSP_IO_PIN_03

#define KEY_PORT    R_PORT0
#define KEY_PIN     BSP_IO_PIN_15

ALIGN(RT_ALIGN_SIZE)
struct rt_thread rt_task1;
static uint8_t task1_stack[256];
struct rt_thread rt_inpass_thread;
static uint8_t inpass_thread_stack[256];

struct rt_semaphore key_sem;
uint32_t led_state = 0;
void task1(void *parameter)
{
    while(1)
    {
        if(rt_sem_take(&key_sem, RT_WAITING_FOREVER) == RT_EOK)
        {
            if(led_state == 0)
            {
                led_state = 1;
                bsp_set_pin(LED_PORT, LED_PIN);
            }
            else
            {
                led_state = 0;
                bsp_reset_pin(LED_PORT, LED_PIN);
            }
        }
        rt_thread_delay(20);
    }
}

void icu_irq7_isr(void)
{
    rt_sem_release(&key_sem);
    bsp_irq_status_clear(ICU_IRQ_NUMBER_7);
}

void led_sw(int argc, char **argv)
{
    if(!rt_strcmp(argv[1], "on"))
    {
        led_state = 1;
        bsp_set_pin(LED_PORT, LED_PIN);
        rt_kprintf("LED ON.\n");
    }
    else if(!rt_strcmp(argv[1], "off"))
    {
        led_state = 0;
        bsp_reset_pin(LED_PORT, LED_PIN);
        rt_kprintf("LED OFF.\n");
    }
    else 
    {
        rt_kprintf("input error.\n");
    }
}
MSH_CMD_EXPORT(led_sw, open switch);

void sys_init(void)
{
    R_PORT1->PDR_b.PDR3 = 1;
    R_PORT1->PDR_b.PDR4 = 1;    

    bsp_port_init(IOPORT_PORT_02_PIN_06, IOPORT_PERIPHERAL_IO, IO_OUT_MODE_HIGH);
    bsp_port_init(IOPORT_PORT_02_PIN_01, IOPORT_PERIPHERAL_IO, IO_OUT_MODE_HIGH);
    bsp_port_init(IOPORT_PORT_02_PIN_08, IOPORT_PERIPHERAL_IO, IO_OUT_MODE_HIGH);
    bsp_port_init(IOPORT_PORT_03_PIN_02, IOPORT_PERIPHERAL_IO, IO_OUT_MODE_HIGH);

    OLED_Init();
    ShowBrand();
    OLED_Switch(1);
    bsp_irq_disable(ICU_IRQ_NUMBER_7);
    bsp_irq_status_clear(ICU_IRQ_NUMBER_7);
    NVIC_SetPriority(ICU_IRQ_NUMBER_7, 4);
    bsp_port_init(IOPORT_PORT_00_PIN_15, IOPORT_PERIPHERAL_IO, IO_IN_PULL_IRQ_MODE);
    bsp_irq_mode_set(7, IRQ_MODE_FALLING | IRQ_MODE_FILTER_PCLKB8);
    bsp_irq_clear_pending(ICU_IRQ_NUMBER_7);
    bsp_irq_enable(ICU_IRQ_NUMBER_7);
}

int main(void)
{
    sys_init();
    rt_sem_init(&key_sem, "key_sem", 0, RT_IPC_FLAG_FIFO);
    rt_thread_init(&rt_task1, "task1", task1, RT_NULL, &task1_stack[0], sizeof(task1_stack), 2, 10);
    rt_thread_startup(&rt_task1);
    
    rt_thread_init(&rt_inpass_thread, "inpass", inpass_thread_entry, RT_NULL, &inpass_thread_stack[0], sizeof(inpass_thread_stack), 2, 10);
    rt_thread_startup(&rt_inpass_thread);
    Display("正在呵绘画init..\0", 0, 0);
    while(1)
    {
        bsp_set_pin(R_PORT1, BSP_IO_PIN_04);
        rt_thread_delay(500);
        bsp_reset_pin(R_PORT1, BSP_IO_PIN_04);
        rt_thread_delay(500);
    }
}
