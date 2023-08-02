#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "ImageData.h"
#include "Debug.h"
#include <stdlib.h> // malloc() free()

#include "LCD_1in28.h"
#include "QMI8658.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#include <string.h>

// UART0
#define UART0_ID        uart0
#define UART0_BAUD_RATE 115200
#define UART0_TX_GP    0
#define UART0_RX_GP    1

int main(void)
{
    stdio_init_all();

    // -- Setup the UART0 port as a seperate item
    // uart_init(UART0_ID, UART0_BAUD_RATE);
    // gpio_set_function(UART0_TX_GP, GPIO_FUNC_UART);
    // gpio_set_function(UART0_RX_GP, GPIO_FUNC_UART);

    DEV_Delay_ms(2000);    // Give time for the USB to be connected by the user clicking things

    // puts("USB with debug");
    // uart_puts(UART0_ID, "UART0 with debug\n");
    // DEV_Delay_ms(1000);    // Give time for the USB to be setup

    // ------------------------------------------
    //Initialize & settings LCD
    if (DEV_Module_Init() != 0)
    {
        return -1;
    }

    printf("LCD_1in28_test Demo\r\n");
    adc_init();
    adc_gpio_init(29);
    adc_select_input(3);
    /* LCD Init */
    printf("1.28inch LCD demo...\r\n");
    LCD_1IN28_Init(HORIZONTAL);
    LCD_1IN28_Clear(BLACK);
    DEV_SET_PWM(60);
    // LCD_SetBacklight(1023);
    UDOUBLE Imagesize = LCD_1IN28_HEIGHT * LCD_1IN28_WIDTH * 2;
    UWORD *BlackImage;

    if ((BlackImage = (UWORD *)malloc(Imagesize)) == NULL)
    {
        printf("Failed to apply for black memory...\r\n");
        exit(0);
    }

    // /*1.Create a new image cache named IMAGE_RGB and fill it with white*/
    Paint_NewImage((UBYTE *)BlackImage, LCD_1IN28.WIDTH, LCD_1IN28.HEIGHT, 0, BLACK);
    Paint_SetScale(65);
    Paint_Clear(BLACK);
    Paint_SetRotate(ROTATE_270);
    Paint_Clear(BLACK);
    // ------------------------------------------
    
    // ------------------------------------------
    // /* GUI */
    printf("drawing...\r\n");
    
    // ------------------------------------------
    float acc[3], gyro[3];
    unsigned int tim_count = 0;
    QMI8658_init();
    printf("QMI8658_init\r\n");

    char buff[30] = {};
    char rx_buff[30] = {};

    while(1)
    {
        // --------------------------------------
        // -- USB serial to LCD
        int16_t ch = getchar_timeout_us(100);
        
        char ch_buff[30] = {};
        int ch_i = 0;
        int state = 0;

        while (ch != PICO_ERROR_TIMEOUT)
        {
            printf("%c", ch); // Echo back so you can see what you've done
            
            if(ch == 'I')
            {
                state = 1;
            }

            if(ch == '\n'){
                for(int i = 0; i<sizeof(ch_buff); i++)
                    rx_buff[i] = ch_buff[i];
                ch_i = 0;
                state = 0;
            }
            
            if(state)
            {
                ch_buff[ch_i] = ch;
                ch_i++;
            }
            // sprintf(rx_buff, "%c", ch);
            ch = getchar_timeout_us(100);
        }
        // --------------------------------------

        // --------------------------------------
        // -- UART0 to LCD
        // char ch_buff[30] = {};
        // int ch_i = 0;
        // int state = 0;

        // while (uart_is_readable_within_us(UART0_ID, 100))
        // {
        //     uint8_t ch = uart_getc(UART0_ID);
        //     // uart_putc_raw(UART1_ID, ch); // Send to UART1
        //     printf("%c", ch); // Echo to USB
        //                       // If you are using this port to communicate with the module, then
        //                       // uncomment this to echo back so you can see what you've sent
        //                       // uart_putc_raw(UART0_ID, ch);
        //     if (ch == 'I')
        //     {
        //         state = 1;
        //     }

        //     if (ch == '\n')
        //     {
        //         for (int i = 0; i < sizeof(ch_buff); i++)
        //             rx_buff[i] = ch_buff[i];
        //         ch_i = 0;
        //         state = 0;
        //     }

        //     if (state)
        //     {
        //         ch_buff[ch_i] = ch;
        //         ch_i++;
        //     }
        // }
        // --------------------------------------

        const float conversion_factor = 3.3f / (1 << 12) * 2;
        uint16_t result = adc_read();
        printf("Raw value: 0x%03x, voltage: %f V\n", result, result * conversion_factor);
        Paint_Clear(BLACK);
        QMI8658_read_xyz(acc, gyro, &tim_count);
        printf("acc_x   = %4.3fmg , acc_y  = %4.3fmg , acc_z  = %4.3fmg\r\n", acc[0], acc[1], acc[2]);
        printf("gyro_x  = %4.3fdps, gyro_y = %4.3fdps, gyro_z = %4.3fdps\r\n", gyro[0], gyro[1], gyro[2]);

        printf("tim_count = %d\r\n", tim_count);

        // Accelerometer
        Paint_DrawString_EN(75, 50, "Accelerometer:", &Font12, WHITE, BLACK);
        sprintf(buff, " x: %4.1fmg", acc[0]);
        Paint_DrawString_EN(75, 65, buff, &Font12, WHITE, BLACK);
        sprintf(buff, " y: %4.1fmg", acc[1]);
        Paint_DrawString_EN(75, 80, buff, &Font12, WHITE, BLACK);
        sprintf(buff, " z: %4.1fmg", acc[2]);
        Paint_DrawString_EN(75, 95, buff, &Font12, WHITE, BLACK);
        //Serial - Jetson IP address
        Paint_DrawString_EN(65, 115, rx_buff, &Font12, WHITE, BLACK);
        //Gyroscope
        Paint_DrawString_EN(75, 150, "Gyroscope:", &Font12, WHITE, BLACK);
        sprintf(buff, " x: %4.1fdps", gyro[0]);
        Paint_DrawString_EN(75, 165, buff, &Font12, WHITE, BLACK);
        sprintf(buff, " y: %4.1fdps", gyro[1]);
        Paint_DrawString_EN(75, 180, buff, &Font12, WHITE, BLACK);
        sprintf(buff, " z: %4.1fdps", gyro[2]);
        Paint_DrawString_EN(75, 195, buff, &Font12, WHITE, BLACK);
        sprintf(buff, " BAT: %1.2fV", result, result * conversion_factor);
        Paint_DrawString_EN(75, 210, buff, &Font12, WHITE, BLACK);
        LCD_1IN28_Display(BlackImage);
        DEV_Delay_ms(100);
    }
    // ------------------------------------------

    /* Module Exit */
    free(BlackImage);
    BlackImage = NULL;

    DEV_Module_Exit();
    return 0;
}