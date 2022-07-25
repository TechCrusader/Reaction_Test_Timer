/*
 * Assignment Program
 * Reaction Timer
 * PRID : JOHNJ41406
 *
 */

//----------------------------------------------------------------------------------------------------------


/*
 * Header files giving access to addition ARM main board functionality
 */

#include <config/AT91SAM7S256.h>			// Hardware description header
#include <board/SevenSegmentDisplay.h>		// Provide access to 7 segment display functions
#include <board/LED-lib.h>					// Provide access to LEDs
#include <board/Button-lib.h>				// Provide access to buttons

#include <atmel/pit.h>						// Provide access to Interrupt functions
#include <atmel/aic.h>
#include <board/Button-lib.h>				// Header files giving access to addition ARM main board functionality

//----------------------------------------------------------------------------------------------------------


/*
 * The digits on the Seven Segment LED Display are numbered right to left. DISPLAY1 is the right most digit.
 * The defines below give more sensible (useful) names to the display digits for use in this program.If needed.
 */

#define RIGHT_DISPLAY			DISPLAY1
#define MIDDLE_RIGHT_DISPLAY	DISPLAY2
#define MIDDLE_LEFT_DISPLAY		DISPLAY3
#define LEFT_DISPLAY			DISPLAY4



//----------------------------------------------------------------------------------------------------------

/*
 * The array Digit contains the binary patterns to display the digits 0 - 9 to the Seven Segment LED Display.
 * Digit[0] is the pattern for zero, Digit[1] is the pattern for one, etc.
 */

short Digit[] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7c, 0x07, 0x7f, 0x67 };

//----------------------------------------------------------------------------------------------------------

/*
 * Declare a variable to store the number of an LED and other led related functions
 */

LEDnumber LEDpos, LeftEnd, RightEnd;
short LeftRight,Game_Start_status =0,j,TARGET = 5, hertz;

short target[] = {7,0,5,4,2,1,7,3,6,4};                                         // Array for the randomising the target button and led
//----------------------------------------------------------------------------------------------------------

/*
 * Global variables for PIT and counter related functions
 */

int seconds_3,seconds_2,seconds_1,seconds_0;
int COUNTER_END =1;

//----------------------------------------------------------------------------------------------------------

/*
 * Software routine to delay a number of milliseconds
 * All timings are approximate.
 */
void delay_ms(short ms)
{
	volatile short loop;

	while (ms-- > 0)
		for (loop = 0; loop < 2100; loop++);
}

//----------------------------------------------------------------------------------------------------------
/*
 * Function to  run at the start of the game this loop will be called repeatedly until
 * BUTTON1 is pressed and update the value of  Game_Start_status
 */

void Game_Waiting(void)
{
	if(IsButtonReleased(BUTTON1)==0)
		{
			Game_Start_status = 1;
		}
	else
		{
			delay_ms(500);
			SetLEDcolor(0, RED);
			delay_ms(500);
			SetLEDcolor(0, GREEN);
		}
}

//----------------------------------------------------------------------------------------------------------
/*
 * Count down starts starts tuning of from LED8 to LED1
 */

void Game_Start(void)
{
	int i;

	/*
	 * Turn on all LED
	 */
	SetAllLEDs(LEDsAllRed);
	delay_ms(100);

	/*
	 * First set the LED position variable to the last LED.
	 */

	LEDpos = LED8;



	/*
	 * Turning On all the LED'S in Red colour
	 */

	SetAllLEDs(LEDsAllRed);

	/*
	 * Turning On all the LED'S in Red colour
	 */

	for(i=7;i>=0;i--)
		{
			while(IsButtonPressed(BUTTON8)||IsButtonPressed(BUTTON7)||IsButtonPressed(BUTTON6)||IsButtonPressed(BUTTON5)||IsButtonPressed(BUTTON4)||IsButtonPressed(BUTTON3)||IsButtonPressed(BUTTON2)||IsButtonPressed(BUTTON1)==1)
				{
					LEDpos = LEDpos + 0;
				}

			SetLEDcolor(LEDpos, OFF);
			LEDpos --;
			delay_ms(500);



		}
	/*
	 * Setting up all the time related variables to zero
	 */

	seconds_0 = 0;
	seconds_1 = 0;
	seconds_2 = 0;
	seconds_3 = 0;
	hertz     = 0;

}

//----------------------------------------------------------------------------------------------------------

/*
 * Interrupt Routine
 *
 * This routine is called for every PIT interrupt
 */

void ISR_System(void)
{

	/*
	 * Read the PIT status register to clear the pending interrupt.
	 * We are not interested in the value so the result does not have to be assigned to anything.
	 */

   	 PIT_GetPIVR();

   	/*
   	 * Now Process the interrupt.
   	 *
	 * 5000 interrupts equals one second
     */

    if (++hertz == 5000)
		{
    		hertz = 0;
    		if (++seconds_0 == 10)
				{
    			seconds_0 = 0;

    			}
		}

   	if ((hertz % 5) == 0)
		{
    		if (++seconds_3 == 10)                                              // Counts One millisecond increment
    			{
    			seconds_3 = 0;
    			if (++seconds_2 == 10)                                          // Counts Ten millisecond increment
    				{
    					seconds_2 = 0;
 	  					if (++seconds_1 == 10)                                  // Counts Hundred millisecond increment
    						{
    							seconds_1 = 0;
    						}

    				}
    			}
		}
}


#define PIT_PERIOD          200 // interrupt at 0.020 milliseconds (5000Hz)

//----------------------------------------------------------------------------------------------------------

/*
 * Main function
 */

int main()

{
	/*=====================================================================================================*/

	/*
	 * First we need to configure the additional functionality required.
	 */

	Configure7SegmentDisplay();

	/*=====================================================================================================*/

	/*
	 * Reset the Seven Segment Display.
	 */

	Set7SegmentDisplayValue(DISPLAY1, Digit[0]); // Displays the digit 0
	Set7SegmentDisplayValue(DISPLAY2, Digit[0]); // Displays the digit 0
	Set7SegmentDisplayValue(DISPLAY3, Digit[0]); // Displays the digit 0
	Set7SegmentDisplayValue(DISPLAY4, Digit[0]); // Displays the digit 0

	/*=====================================================================================================*/

	/*
	 * Initialize and enable the PIT
	 *
	 * First argument (PIT_PERIOD) is number of milliseconds between timer interrupts (maximum 20 bit number)
	 * Second argument is clock rate in MHz (BOARD_MCK is in Hz so divide by 1000000)
	 *
	 */

	PIT_Init(PIT_PERIOD, BOARD_MCK / 1000000);

	/*
	 * Disable the interrupt on the interrupt controller
	 */

	AIC_DisableIT(AT91C_ID_SYS);

	/*
	 * Configure the AIC for PIT interrupts
	 *
	 * The PIT is a System interrupt, so we need to associate the ID of the System interrupt with the interrupt
	 * routine ISR_System.
	 */

	AIC_ConfigureIT(AT91C_ID_SYS, 0, ISR_System);

	/*
	 * Enable the interrupt on the interrupt controller
	 */

	AIC_EnableIT(AT91C_ID_SYS);

	/*
	 * Enable the PIT interrupt and start the PIT
	 *
	 * The interrupt handler is always assigned before the interrupt is enabled.
	 * Using the library function the PIT interrupt is enabled separately from the PIT itself. the interrupt
	 * is enabled first so none are missed when the timer is enabled.
	 */

	 PIT_EnableIT();
	 PIT_Enable();


/*=====================================================================================================*/

	/*
 	 * Running the whole system in loop
 	 */

 	while (1)                                       							// This loop runs repeatedly so at the end game keeps on running until device is powered off
 		{

 			Game_Waiting();														// Function : Waiting for the user to start the game

 			while(Game_Start_status)											// If the BUTTON1 is pressed the Game_Start_Status gets value one and the loop run's until the condition fails
 				{

 					/*
 					 * Setting up all the displays to zero
 					 */

 					Set7SegmentDisplayValue(DISPLAY1, Digit[0]); // Displays the digit 0
 					Set7SegmentDisplayValue(DISPLAY2, Digit[0]); // Displays the digit 0
 					Set7SegmentDisplayValue(DISPLAY3, Digit[0]); // Displays the digit 0
 					Set7SegmentDisplayValue(DISPLAY4, Digit[0] + 0x80); // Displays the digit 0

 					Game_Start();                                               // Count down Starts

					/*
 					 * Turning on the led above the target button
 					 */

 					SetLEDcolor(TARGET,	GREEN);                                 // Turning On the LED above the target button

 					while(IsButtonReleased(TARGET)&&COUNTER_END)				// The counter will be counting up until the user click on target button or count reaches 9.999
 						{

 							if(seconds_0 + seconds_1 + seconds_2 + seconds_3 == 36)
 							 	  	{
 							 	  		COUNTER_END = 0;						//Checking if the counter hits 9.999s
 							 	  	}
 					/*
 					 * Updating the display for each  millisecond
 					 */

 							Set7SegmentDisplayValue(DISPLAY1, Digit[seconds_3]);
 							Set7SegmentDisplayValue(DISPLAY2, Digit[seconds_2]);
 							Set7SegmentDisplayValue(DISPLAY3, Digit[seconds_1]);
 							Set7SegmentDisplayValue(DISPLAY4, Digit[seconds_0] + 0x80);
 						}

 					SetLEDcolor(TARGET,	OFF);									// Turning off the target LED when the user presses on the TARGET button or fails to click it within 10 seconds

 					TARGET = target[seconds_2];									// Randomise the target switch for the reaction testing based on variable second_2

 					if(TARGET >> 7)
 						{
 							TARGET = TARGET - 3;								// This if condition is just avoid any error values passed to the target button
 						}


 					if(COUNTER_END == 0)										// If counter ends up before the the person presses on the target
 						{
 							COUNTER_END = 1;
 							TARGET = 4;
 						}


 					Game_Start_status = 0;										// Exit the whole loop and go to initial starting point

 				}
 		}
/*=====================================================================================================*/

}


