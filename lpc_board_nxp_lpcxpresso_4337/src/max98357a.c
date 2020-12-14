/*
 * @brief MAX98357A Audio PCM Class D Amplifier
 */

#include "board.h"
#include "max98357a.h"

#define SD_MODE_PORT 3
#define SD_MODE_PIN 1

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/


/* MAX98357A initialize function */
int MAX98357A_Init (void)
{
	//printf("%s()\r\n", __FUNCTION__);

	// setup output for SD_MODE
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, SD_MODE_PORT, SD_MODE_PIN);
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, SD_MODE_PORT, SD_MODE_PIN);

    return SUCCESS;
}

int MAX98357A_Enable (void)
{
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, SD_MODE_PORT, SD_MODE_PIN);
	return 0;
}

int MAX98357A_Disable (void)
{
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, SD_MODE_PORT, SD_MODE_PIN);
	return 0;
}

/**
 * @}
 */
