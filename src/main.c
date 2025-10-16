#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>


static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(DT_ALIAS(my_led), gpios);


int main(void)
{
    if(!gpio_is_ready_dt(&led0))
        return 0;
    
    gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);

    while(1)
    {
        gpio_pin_toggle_dt(&led0);
        k_msleep(200);
    }

    return 0;
}
