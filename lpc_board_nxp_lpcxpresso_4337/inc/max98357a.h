/*
 * @brief  MAX98357A Audio PCM Class D Amplifier
 */

#ifndef _MAX98357A_H
#define _MAX98357A_H

/** @defgroup BOARD_COMMON_WM8904 BOARD: WM8904 Audio codec interface module
 * @ingroup BOARD_Common
 * WM8904 Audio codec interface module, the module registers are accessed
 * using I2C. The board which uses this module must define WM8904_I2C_BUS to I2C0,
 * I2C1, etc, based on which I2C bus is connected to WM8904. All the
 * functions in this modules assumes that the I2C interrupt for WM8904_I2C_BUS
 * is enabled and Chip_I2C_MasterStateHandler(WM8904_I2C_BUS) is called from the
 * ISR. If the functions are to be used in polling mode the caller must replace
 * the event handler to Chip_I2C_EventHandlerPolling(), by using API
 * Chip_I2C_SetMasterEventHandler(). A macro I2CDEV_WM8904_ADDR must be defined
 * to the appropriate slave address of WM8904 audio codec.
 * @{
 */



#ifdef __cplusplus
extern "C" {
#endif


int MAX98357A_Init(void);
int MAX98357A_Enable (void);
int MAX98357A_Disable (void);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* _MAX98357A_H */

