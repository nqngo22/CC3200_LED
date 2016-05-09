//Nguyen Ngo
//Section A03

// Standard includes
#include <stdio.h>

// Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "interrupt.h"
#include "hw_apps_rcm.h"
#include "prcm.h"
#include "rom.h"
#include "rom_map.h"
#include "prcm.h"
#include "gpio.h"
#include "utils.h"

#include "uart.h"
#include "uart_if.h"


// Common interface includes
#include "gpio_if.h"

#include "pin_mux_config.h"

#define APPLICATION_VERSION     "1.1.1"

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif

int g_bCount = 0;
//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************


//*****************************************************************************
//                      LOCAL FUNCTION PROTOTYPES                           
//*****************************************************************************
void LEDBlinkyRoutine();
static void BoardInit(void);

//*****************************************************************************
//                      LOCAL FUNCTION DEFINITIONS                         
//*****************************************************************************

//*****************************************************************************
//
//! Configures the pins as GPIOs and peroidically toggles the lines
//!
//! \param None
//! 
//! This function  
//!    1. Configures 3 lines connected to LEDs as GPIO
//!    2. Sets up the GPIO pins as output
//!    3. Periodically toggles each LED one by one by toggling the GPIO line
//!
//! \return None
//
//*****************************************************************************
void LEDBlinkyRoutine()
{
    //
    // Toggle the lines initially to turn off the LEDs.
    // The values driven are as required by the LEDs on the LP.
    //
    GPIO_IF_LedOff(MCU_ALL_LED_IND);

    //setting initial state
    int state = 0;

    while(1){
        	switch(state){
        	case 0:
        		//set Pin 18 output low
        		GPIOPinWrite(GPIOA3_BASE, 0x10, 0x00);
        		//turn on green led
    			GPIO_IF_LedOn(MCU_GREEN_LED_GPIO);
        		if(!GPIOPinRead(GPIOA1_BASE, 0x20) && GPIOPinRead(GPIOA2_BASE, 0x40)){
        			//sw3 not pressed, sw2 pressed
        			state = 1;
        		}else if(GPIOPinRead(GPIOA1_BASE, 0x20) && GPIOPinRead(GPIOA2_BASE, 0x40)){
        			//sw3 pressed and sw2 pressed
					state = 4;
        		}else if(GPIOPinRead(GPIOA1_BASE, 0x20) && !GPIOPinRead(GPIOA2_BASE, 0x40)){
					//sw3 pressed and sw2 not pressed
					state = 4;
				}
        		break;
        	case 1:
        		//set Pin 18 output low
        		GPIOPinWrite(GPIOA3_BASE, 0x10, 0x00);
        		//turn off green led
        		GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);
        		//turn on orange led
        		GPIO_IF_LedOn(MCU_ORANGE_LED_GPIO);
        		if(GPIOPinRead(GPIOA1_BASE, 0x20) && GPIOPinRead(GPIOA2_BASE, 0x40)) {
        			//sw3 pressed and sw2 pressed
        			state = 2;
        		}else if(!GPIOPinRead(GPIOA1_BASE, 0x20) && !GPIOPinRead(GPIOA2_BASE, 0x40)){
        			//sw3 not pressed and sw2 not pressed
        			state = 4;
        		}else if(GPIOPinRead(GPIOA1_BASE, 0x20) && !GPIOPinRead(GPIOA2_BASE, 0x40)){
        			//sw3 pressed and sw2 not pressed
        			state = 4;
        		}
        		break;
        	case 2:
        		//set Pin 18 output low
        		GPIOPinWrite(GPIOA3_BASE, 0x10, 0x00);
        		//turn on green led
        		GPIO_IF_LedOn(MCU_GREEN_LED_GPIO);
        		if(GPIOPinRead(GPIOA1_BASE, 0x20) && !GPIOPinRead(GPIOA2_BASE, 0x40)){
        			//sw3 pressed, sw2 not pressed
        			state = 3;
        		}else if(!GPIOPinRead(GPIOA1_BASE, 0x20) && !GPIOPinRead(GPIOA2_BASE, 0x40)){
        			//sw3 not pressed, sw2 not presed
        			state = 4;
        		}else if(!GPIOPinRead(GPIOA1_BASE, 0x20) && GPIOPinRead(GPIOA2_BASE, 0x40)){
        			//sw3 not pressed, sw2 pressed
        			state = 4;
        		}
        		break;
        	case 3:
        		//turn on red led
        		GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        		//toggles pin 18 at rate of about 5 hz
        		//MAP_UtilsDelay
        		//param ulCount in MAP_UtilsDelay is the number of delay loop iterations to perform.
        		//The loop takes 3 cycles/loop from ROM, and 5 cycles/loop from RAM
        		//rate of 5 hz = 5 blinks per sec
        		//clock tick is at 80MHz
        		//1 sec = 16000000. So .2 sec = 3200000
        		//dividing by 2 for on and off = 1600000
        		while(1){
        			MAP_UtilsDelay(1600000);
					GPIOPinWrite(GPIOA3_BASE, 0x10, 0xFF);
					MAP_UtilsDelay(1600000);
					GPIOPinWrite(GPIOA3_BASE, 0x10, 0x00);
				}
        		//break;

        	case 4:
        		//set Pin 18 output low
        		GPIOPinWrite(GPIOA3_BASE, 0x10, 0x00);
        		//set all led lights on
    			GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);
    			GPIO_IF_LedOff(MCU_ORANGE_LED_GPIO);
				GPIO_IF_LedOn(MCU_RED_LED_GPIO);

				break;
        	}
        }

}
//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void
BoardInit(void)
{
/* In case of TI-RTOS vector table is initialize by OS itself */
#ifndef USE_TIRTOS
    //
    // Set vector table base
    //
#if defined(ccs)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif
    
    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}

DisplayBanner(char * AppName)
{

    Report("\n\n\n\r");
    Report("\t\t *************************************************\n\r");
    Report("\t\t        CC3200 %s Application       \n\r", AppName);
    Report("\t\t *************************************************\n\r");
    Report("\n\n\n\r");
}


//****************************************************************************
//
//! Main function
//!
//! \param none
//! 
//! This function  
//!    1. Invokes the LEDBlinkyTask
//!
//! \return None.
//
//****************************************************************************
int
main()
{
    //
    // Initialize Board configurations
    //
    BoardInit();
    
    //
    // Power on the corresponding GPIO port B for 9,10,11.
    // Set up the GPIO lines to mode 0 (GPIO)
    //
    PinMuxConfig();

	#ifndef NOTERM
		//
		// Configuring UART
		//
		InitTerm();
	#endif

	DisplayBanner("Lab 0");
    Message("\t\t****************************************************\n\r");
    Message("\t\t\t        CC3200 UART Echo Usage        \n\r");
    Message("\t\t To get to state 3, the user must press and hold SW2  \n\r");
    Message("\t\t then press SW3 without releasing SW2   \n\r");
    Message("\t\t and then release SW2 while continuing to press SW3\n\r");
    Message("\n\n\n\r");

    GPIO_IF_LedConfigure(LED1|LED2|LED3);

    GPIO_IF_LedOff(MCU_ALL_LED_IND);
    
    //
    // Start the LEDBlinkyRoutine
    //
    LEDBlinkyRoutine();
    return 0;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
