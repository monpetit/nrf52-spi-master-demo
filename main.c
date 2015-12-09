/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/**@file
 * @defgroup spi_master_example_with_slave_main main.c
 * @{
 * @ingroup spi_master_example
 *
 * @brief SPI master example application to be used with the SPI slave example application.
 */

#include <stdio.h>
#include <stdbool.h>
#include "app_error.h"
#include "app_util_platform.h"
#include "nrf_delay.h"
#include "bsp.h"
#include "app_timer.h"
#include "nrf_drv_spi.h"
#include "nordic_common.h"
#include "uart_module.h"
#include "spi_module.h"
#include "ds3234_rtc.h"


#define DELAY_MS                 1000                /**< Timer Delay in milli-seconds. */

/*
 * This example uses only one instance of the SPI master.
 * Please make sure that only one instance of the SPI master is enabled in config file.
 */

#define APP_TIMER_PRESCALER      0                      /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_MAX_TIMERS     BSP_APP_TIMERS_NUMBER  /**< Maximum number of simultaneously created timers. */
#define APP_TIMER_OP_QUEUE_SIZE  2                      /**< Size of timer operation queues. */


volatile bool change_rtc = false;
extern uint8_t time_buffer[128];

/**@brief Function for error handling, which is called when an error has occurred.
 *
 * @param[in] error_code  Error code supplied to the handler.
 * @param[in] line_num    Line number where the handler is called.
 * @param[in] p_file_name Pointer to the file name.
 */
void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
    UNUSED_VARIABLE(bsp_indication_set(BSP_INDICATE_FATAL_ERROR));

    for (;;) {
        // No implementation needed.
    }
}



/**@brief Function for initializing bsp module.
 */
void bsp_configuration()
{
    uint32_t err_code = NRF_SUCCESS;

    NRF_CLOCK->LFCLKSRC            = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_LFCLKSTART    = 1;

    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0) {
        // Do nothing.
    }

    APP_TIMER_INIT(APP_TIMER_PRESCALER, /*APP_TIMER_MAX_TIMERS,*/ APP_TIMER_OP_QUEUE_SIZE, NULL);

    err_code = bsp_init(BSP_INIT_LED, APP_TIMER_TICKS(100, APP_TIMER_PRESCALER), NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for application main entry. Does not return. */
int main(void)
{
    // Setup bsp module.
    bsp_configuration();

    uart_init();
    spi_init();

    nrf_delay_ms(1000);
    puts("--- START ---");

    nrf_delay_ms(1000);
    // ds3234_set_datetime(9, 12, 15, 16, 52, 22);
    // ds3234_settime_string((char*)"2015-12-09 16:52:22:");
    // nrf_delay_ms(1000);

    for (;;) {
        if (change_rtc) {
            change_rtc = false;
            ds3234_settime_string((char*)(time_buffer + 1));
        }

        ds3234_gettime();
        nrf_delay_ms(DELAY_MS);
    }
}


/** @} */
