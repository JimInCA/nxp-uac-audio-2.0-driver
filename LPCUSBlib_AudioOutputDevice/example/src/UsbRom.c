/*
 * @brief Audio device class ROM based application's specific functions supporting core layer
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "UsbRom.h"

#if defined(USB_DEVICE_ROM_DRIVER)

uint8_t StringDescriptor[] = {
	USB_STRING_LEN(1),	/* LanguageString */
	DTYPE_String,
	WBVAL(LANGUAGE_ID_ENG),

	USB_STRING_LEN(9),	/* ManufacturerString */
	DTYPE_String,
	WBVAL('U'),
	WBVAL('l'),
	WBVAL('t'),
	WBVAL('r'),
	WBVAL('a'),
	WBVAL('l'),
	WBVAL('e'),
	WBVAL('a'),
	WBVAL('p'),

	USB_STRING_LEN(27),	/* ProductString */
	DTYPE_String,
	WBVAL('U'),
	WBVAL('l'),
	WBVAL('t'),
	WBVAL('r'),
	WBVAL('a'),
	WBVAL('l'),
	WBVAL('e'),
	WBVAL('a'),
	WBVAL('p'),
	WBVAL(' '),
	WBVAL('A'),
	WBVAL('u'),
	WBVAL('d'),
	WBVAL('i'),
	WBVAL('o'),
	WBVAL(' '),
	WBVAL('O'),
	WBVAL('u'),
	WBVAL('t'),
	WBVAL('p'),
	WBVAL('u'),
	WBVAL('t'),
	WBVAL(' '),
	WBVAL('D'),
	WBVAL('e'),
	WBVAL('m'),
	WBVAL('o'),

	USB_STRING_LEN(6),	/* SerialNumberString */
	DTYPE_String,
	WBVAL('1'),
	WBVAL('2'),
	WBVAL('3'),
	WBVAL('4'),
	WBVAL('5'),
	WBVAL('6')
};
extern USB_Descriptor_Device_t DeviceDescriptor;
extern USB_Descriptor_Configuration_t ConfigurationDescriptor;

uint32_t CALLBACK_UsbdRom_Register_DeviceDescriptor(void)
{
	return (uint32_t) &DeviceDescriptor;
}

uint32_t CALLBACK_UsbdRom_Register_ConfigurationDescriptor(void)
{
	return (uint32_t) &ConfigurationDescriptor;
}

uint32_t CALLBACK_UsbdRom_Register_StringDescriptor(void)
{
	return (uint32_t) StringDescriptor;
}

uint32_t CALLBACK_UsbdRom_Register_DeviceQualifierDescriptor(void)
{
	return (uint32_t) NULL;
}

uint8_t CALLBACK_UsbdRom_Register_ConfigureEndpoint(void)
{
	return 0;
}

#endif
