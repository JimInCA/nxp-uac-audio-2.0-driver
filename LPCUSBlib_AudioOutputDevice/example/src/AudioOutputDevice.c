/*
 * @brief Make your board becomes a USB speaker
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * Copyright(C) Dean Camera, 2011, 2012
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

#include "AudioOutputDevice.h"
#include "max98357a.h"

#if defined(USB_DEVICE_ROM_DRIVER)
#include "usbd_adcuser.h"
#endif
/** LPCUSBlib Audio Class driver interface configuration and state information. This structure is
 *  passed to all Audio Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_Audio_Device_t Speaker_Audio_Interface = {
	.Config = {
		.StreamingInterfaceNumber = 1,

		.DataOUTEndpointNumber    = AUDIO_STREAM_EPNUM,
		.DataOUTEndpointSize      = AUDIO_STREAM_EPSIZE,
		.PortNumber = 0,
	},
};

/** Max Sample Frequency. */
#define AUDIO_MAX_SAMPLE_FREQ   48000
/** if audio buffer count is over this value, i2s will run in higher speed */
#define AUDIO_SPEEP_UP_TRIGGER			(audio_buffer_size*5/8)
/** if audio buffer count is under this value, i2s will run in normal speed */
#define AUDIO_NORMAL_SPEED_TRIGGER	(audio_buffer_size*3/8)
/** Current audio sampling frequency of the streaming audio endpoint. */
uint32_t CurrentAudioSampleFrequency = AUDIO_MAX_SAMPLE_FREQ;

/**
 * Audio API
 */
/** Audio max packet count. */
#define AUDIO_MAX_PC    10
PRAGMA_ALIGN_4
uint8_t audio_buffer[2048*2] ATTR_ALIGNED(4);
uint32_t audio_buffer_size = 0;
uint32_t audio_buffer_rd_index = 0;
uint32_t audio_buffer_wr_index = 0;
uint32_t audio_buffer_count = 0;

typedef struct {
	uint8_t BITRATE;
	struct {
		uint8_t X,Y;
	} RATEUP;
	struct {
		uint8_t X,Y;
	} RATEDOWN;
} I2S_RATE_CONFIG;

static uint32_t I2S_SpeedConfig_index;
#if defined(__LPC18XX__ )
/** USB Audio Speed config table if I2S peripheral speed = 180Mhz 
 *	Manual find and set for closest match freq */
I2S_RATE_CONFIG I2S_SpeedConfig[] = 
{
	/* BITRATE	RATE_Up(x,y)	RATE_Down(x,y) */
	/* 8000 */
	{0x3F,	{0x2D,	0xF7},	{0x02,	0x0B}},
	/* 11025 */
	{0x3F,	{0x40,	0xFF},	{0x01,	0x04}},
	/* 16000 unsupport */
	{0,	{0,	0},	{0,	0}},
	/* 22050 */
	{0x3F,	{0x80,	0xFF},	{0x01,	0x02}},
	/* 32000 unsupport */
	{0,	{0,	0},	{0,	0}},
	/* 44100 */
	{0x3E,	{0xF4,	0xF7},	{0x51,	0x52}},
	/* 48000 */
	{0x39,	{0x62,	0x63},	{0xC3,	0xC5}}
	
};
#elif defined(__LPC43XX__ )
/** USB Audio Speed config table if I2S peripheral speed = 204Mhz 
 *	Manual find and set for closest match freq */
I2S_RATE_CONFIG I2S_SpeedConfig[] = 
{
	/* BITRATE	RATE_Up(x,y)	RATE_Down(x,y) */
	/* 8000 */
	{0x3F,	{0x20,	0xC1},	{0x16,	0x89}},
	/* 11025 */
	{0x3F,	{0x1D,	0x83},	{0x38,	0xFD}},
	/* 16000 */
	{0x3f,	{0x3f,	0xc1},	{0x35,	0xA5}},
	/* 22050 */
	{0x3F,	{0x3A,	0x83},	{0x55,	0xC0}},
	/* 32000 */
	{0x3f,	{0x85,	0xCF},	{0x7C,	0xC1}},
	/* 44100 */
	{0x3F,	{0x74,	0x83},	{0xC9,	0xE3}},
	/* 48000 */
	{0x3F,	{0xD5,	0xDD},	{0x85,	0x8A}}
	
};
#endif
Status I2S_RateFind(LPC_I2S_T *I2Sx, I2S_AUDIO_FORMAT_T *audio_format, I2S_RATE_CONFIG *I2S_Config)
{
	uint32_t pClk;
	uint32_t x, y;
	uint64_t divider;
	uint16_t dif;
	uint16_t x_divide = 0, y_divide_down = 0, y_divide_up = 0;
	uint32_t N;
	uint32_t err, ErrorOptimal_down = 0xFFFF, ErrorOptimal_up = 0xFFFF;

	pClk = (uint64_t)Chip_Clock_GetRate(CLK_APB1_I2S);

	/* find N that make x/y <= 1 -> divider <= 2^16 */
	for (N = 64; N > 0; N--) {
		/* divider is a fixed point number with 16 fractional bits */
		divider = (((uint64_t)(audio_format->SampleRate) * 2 * (audio_format->WordWidth) * 2) << 16) * N / pClk;
		if (divider < (1 << 16)) {
			break;
		}
	}
	if (N == 0) {
		return ERROR;
	}
	//divider *= N;
	for (y = 255; y > 0; y--) {
		x = y * divider;
		if (x & (0xFF000000)) {
			continue;
		}
		dif = x & 0xFFFF;
		if (dif > 0x8000) {
			/* mark this case to plus 1 to x value */
			err = (0x10000 - dif);
			/* in this case x = 255 + 1 */
			if((x & 0x00FF0000) == 0x00FF0000) continue;
			if (err < ErrorOptimal_up ) {
				ErrorOptimal_up = err;
				y_divide_up = y;
			}
		}
		else {
			err = dif;
			if (err < ErrorOptimal_down) {
				ErrorOptimal_down = err;
				y_divide_down = y;
			}
		}
	}
	I2S_Config->BITRATE = N - 1;
	x_divide = ((uint64_t)y_divide_up * (audio_format->SampleRate) * 2 * (audio_format->WordWidth) * N * 2) / pClk;
	x_divide += 1;
	if (x_divide >= 256) {
		x_divide = 0xFF;
	}
	if (x_divide == 0) {
		x_divide = 1;
	}
	I2S_Config->RATEUP.X = x_divide;
	I2S_Config->RATEUP.Y = y_divide_up;
	
	x_divide = ((uint64_t)y_divide_down * (audio_format->SampleRate) * 2 * (audio_format->WordWidth) * N * 2) / pClk;
	if (x_divide >= 256) {
		x_divide = 0xFF;
	}
	if (x_divide == 0) {
		x_divide = 1;
	}
	I2S_Config->RATEDOWN.X = x_divide;
	I2S_Config->RATEDOWN.Y = y_divide_down;

	//printf("audio_format->SampleRate: %d\r\n", audio_format->SampleRate);
	//printf("I2S_Config->BITRATE: 0x%02x\r\n", I2S_Config->BITRATE);
	//printf("I2S_Config->RATEUP.X: 0x%02x\r\n", I2S_Config->RATEUP.X);
	//printf("I2S_Config->RATEUP.Y: 0x%02x\r\n", I2S_Config->RATEUP.Y);
	//printf("I2S_Config->RATEDOWN.X: 0x%02x\r\n", I2S_Config->RATEDOWN.X);
	//printf("I2S_Config->RATEDOWN.Y: 0x%02x\r\n", I2S_Config->RATEDOWN.Y);

	return SUCCESS;
}

void Audio_Reset_Data_Buffer(void)
{
	//printf("0x%02x 0x%02x 0x%02x 0x%02x\r\n", audio_buffer[0], audio_buffer[1], audio_buffer[2], audio_buffer[3]);
	audio_buffer_count = 0;
	audio_buffer_wr_index = 0;
	audio_buffer_rd_index = 0;
}

uint32_t Audio_Get_ISO_Buffer_Address(uint32_t last_packet_size)
{
	audio_buffer_wr_index += last_packet_size;
	audio_buffer_count += last_packet_size;
	if (audio_buffer_count > audio_buffer_size) {
		Audio_Reset_Data_Buffer();
	}
	if (audio_buffer_wr_index >= audio_buffer_size) {
		memcpy(audio_buffer, &audio_buffer[audio_buffer_size], audio_buffer_wr_index - audio_buffer_size);
		audio_buffer_wr_index -= audio_buffer_size;
	}
	return (uint32_t) &audio_buffer[audio_buffer_wr_index];
}

void Audio_Init(uint32_t samplefreq)
{
	I2S_AUDIO_FORMAT_T audio_Confg;

	//printf("%s()\r\n", __FUNCTION__);

	audio_Confg.SampleRate = samplefreq;
	audio_Confg.ChannelNumber = 2;	// 1 is mono, 2 is stereo
	audio_Confg.WordWidth = 16;	// 8, 16 or 32 bits
	Board_Audio_Init(LPC_I2S0);

	Chip_I2S_Init(LPC_I2S0);
	Chip_I2S_TxConfig(LPC_I2S0, &audio_Confg);
	Chip_I2S_TxStop(LPC_I2S0);
	Chip_I2S_DisableMute(LPC_I2S0);
	Chip_I2S_TxStart(LPC_I2S0);
	Chip_I2S_Int_TxCmd(LPC_I2S0, ENABLE, 4);
	NVIC_EnableIRQ(I2S0_IRQn);

	Audio_Reset_Data_Buffer();
	switch (samplefreq) {
	case 11025:
		I2S_SpeedConfig_index = 1;
		audio_buffer_size = 1764;
		break;
	case 22050:
		I2S_SpeedConfig_index = 3;
		audio_buffer_size = 1764;
		break;
	case 44100:
		I2S_SpeedConfig_index = 5;
		audio_buffer_size = 1764;
		break;

	case 8000:
		I2S_SpeedConfig_index = 0;
	audio_buffer_size = samplefreq * 4 * AUDIO_MAX_PC / 1000;
		break;
	case 16000:
		I2S_SpeedConfig_index = 2;
		audio_buffer_size = samplefreq * 4 * AUDIO_MAX_PC / 1000;
		break;
	case 32000:
		I2S_SpeedConfig_index = 4;
		audio_buffer_size = samplefreq * 4 * AUDIO_MAX_PC / 1000;
		break;
	case 48000:
		I2S_SpeedConfig_index = 6;
	default:
		audio_buffer_size = samplefreq * 4 * AUDIO_MAX_PC / 1000;
		break;
	}
	Chip_I2S_SetTxBitRate(LPC_I2S0, I2S_SpeedConfig[I2S_SpeedConfig_index].BITRATE);
	Chip_I2S_SetTxXYDivider(LPC_I2S0,
							I2S_SpeedConfig[I2S_SpeedConfig_index].RATEDOWN.X,
							I2S_SpeedConfig[I2S_SpeedConfig_index].RATEDOWN.Y);
	audio_buffer_size*=2;

	//printf("Sample Frequency: %d\r\n", samplefreq);
	//printf("I2S_SpeedConfig[%d].BITRATE: 0x%02x\r\n", I2S_SpeedConfig_index, I2S_SpeedConfig[I2S_SpeedConfig_index].BITRATE);
	//printf("I2S_SpeedConfig[%d].RATEUP.X: 0x%02x\r\n", I2S_SpeedConfig_index, I2S_SpeedConfig[I2S_SpeedConfig_index].RATEUP.X);
	//printf("I2S_SpeedConfig[%d].RATEUP.Y: 0x%02x\r\n", I2S_SpeedConfig_index, I2S_SpeedConfig[I2S_SpeedConfig_index].RATEUP.Y);
	//printf("I2S_SpeedConfig[%d].RATEDOWN.X: 0x%02x\r\n", I2S_SpeedConfig_index, I2S_SpeedConfig[I2S_SpeedConfig_index].RATEDOWN.X);
	//printf("I2S_SpeedConfig[%d].RATEDOWN.Y: 0x%02x\r\n", I2S_SpeedConfig_index, I2S_SpeedConfig[I2S_SpeedConfig_index].RATEDOWN.Y);
}

void Audio_DeInit(void)
{
	Chip_I2S_DeInit(LPC_I2S0);
	Chip_I2S_Int_TxCmd(LPC_I2S0, DISABLE, 4);
	NVIC_DisableIRQ(I2S0_IRQn);
}

void I2S0_IRQHandler(void)
{
	uint32_t txlevel, i;
	static bool double_speed = false;
	static uint32_t sample = 0;
	txlevel = Chip_I2S_GetTxLevel(LPC_I2S0);
	if (txlevel <= 4)
	{
		for (i = 0; i < 8 - txlevel; i++)
		{
			if (audio_buffer_count >= 4)
			{	/*has enough data */
				audio_buffer_count -= 4;
				sample = *(uint32_t *) (audio_buffer + audio_buffer_rd_index);
				audio_buffer_rd_index += 4;
				if (audio_buffer_rd_index >= audio_buffer_size)
				{
					audio_buffer_rd_index -= audio_buffer_size;
				}
			}
			Chip_I2S_Send(LPC_I2S0, sample);
		}
		/*Skip some samples if buffer run writing too fast. */
		if(audio_buffer_size != 0)
 		{
 			if(audio_buffer_count >= AUDIO_SPEEP_UP_TRIGGER)
 			{
 				if(!double_speed)
 				{
					Chip_I2S_SetTxXYDivider(LPC_I2S0,
										  I2S_SpeedConfig[I2S_SpeedConfig_index].RATEUP.X,
										  I2S_SpeedConfig[I2S_SpeedConfig_index].RATEUP.Y);
					double_speed = true;
				}
 			}
 			else if(audio_buffer_count < AUDIO_NORMAL_SPEED_TRIGGER)
 			{
 				if(double_speed)
 				{
					Chip_I2S_SetTxXYDivider(LPC_I2S0,
											I2S_SpeedConfig[I2S_SpeedConfig_index].RATEDOWN.X,
											I2S_SpeedConfig[I2S_SpeedConfig_index].RATEDOWN.Y);
					double_speed = false;
				}
 			}
 		}
	}
}

/** This callback function provides iso buffer address for HAL iso transfer processing.
 */
uint32_t CALLBACK_HAL_GetISOBufferAddress(const uint32_t EPNum, uint32_t *last_packet_size)
{
    static uint16_t counter = 0;
	/* Check if this is audio stream endpoint */
	if (EPNum == AUDIO_STREAM_EPNUM) {
	    Board_LED_Set(counter % 4, false);
	    counter++;
	    Board_LED_Set(counter % 4, true);
		return Audio_Get_ISO_Buffer_Address(*last_packet_size);
	}
	else {return 0; }
}

/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
int main(void)
{
	I2S_AUDIO_FORMAT_T audio_Confg;

	SetupHardware();
	printf("\r\nAudio Output Device\r\n");
	//Board_UARTPutChar('*');

/*
	printf("USB_Descriptor_Configuration_t: %d\r\n", sizeof(USB_Descriptor_Configuration_t));
	printf("USB_Descriptor_Configuration_Header_t: %d\r\n", sizeof(USB_Descriptor_Configuration_Header_t));
	printf("USB_StdDescriptor_Interface_Association_t: %d\r\n", sizeof(USB_StdDescriptor_Interface_Association_t));
	printf("USB_Descriptor_Interface_t: %d\r\n", sizeof(USB_Descriptor_Interface_t));
	printf("USB_Audio_Descriptor_Interface_AC_t: %d\r\n", sizeof(USB_Audio_Descriptor_Interface_AC_t));
#ifdef USB_AUDIO_2DOT0
	printf("USB_Audio_StdDescriptor_Source_Clock_t: %d\r\n", sizeof(USB_Audio_StdDescriptor_Source_Clock_t));
#endif
	printf("USB_Audio_Descriptor_InputTerminal_t: %d\r\n", sizeof(USB_Audio_Descriptor_InputTerminal_t));
	printf("USB_Audio_StdDescriptor_FeatureUnit_t: %d\r\n", sizeof(USB_Audio_StdDescriptor_FeatureUnit_t));
	printf("USB_Audio_Descriptor_OutputTerminal_t: %d\r\n", sizeof(USB_Audio_Descriptor_OutputTerminal_t));
	printf("USB_Descriptor_Interface_t: %d\r\n", sizeof(USB_Descriptor_Interface_t));
	printf("USB_Descriptor_Interface_t: %d\r\n", sizeof(USB_Descriptor_Interface_t));
	printf("USB_Audio_Descriptor_Interface_AS_t: %d\r\n", sizeof(USB_Audio_Descriptor_Interface_AS_t));
	printf("USB_Audio_Descriptor_Format_t: %d\r\n", sizeof(USB_Audio_Descriptor_Format_t));
	printf("USB_Audio_Descriptor_StreamEndpoint_Std_t: %d\r\n", sizeof(USB_Audio_Descriptor_StreamEndpoint_Std_t));
	printf("USB_Audio_Descriptor_StreamEndpoint_Spc_t: %d\r\n", sizeof(USB_Audio_Descriptor_StreamEndpoint_Spc_t));
#ifdef USB_AUDIO_2DOT0
	printf("USB_Audio_Descriptor_StreamEndpoint_Std_t: %d\r\n", sizeof(USB_Audio_Descriptor_StreamEndpoint_Std_t));
#endif

	int32_t i, j;
	char * dev_des = (char*)&DeviceDescriptor;
	for (i = 0; i < sizeof(USB_Descriptor_Device_t); i++)
	{
		printf("0x%02x ", dev_des[i]);
	}
	printf("\r\n");

	char * conf_des = (char*)&ConfigurationDescriptor;
	for (i = 0, j = conf_des[0]; i < sizeof(USB_Descriptor_Configuration_t); i++)
	{
		printf("0x%02x ", conf_des[i]);
		if (--j == 0)
		{
			j = conf_des[i+1];
			printf("\r\n");
		}
	}
	printf("\r\n");
*/

	audio_Confg.ChannelNumber = 2;	//stereo
	audio_Confg.WordWidth = 16;	//16 bits
	/* Build I2S config table */
	audio_Confg.SampleRate = 8000;
	I2S_RateFind(LPC_I2S0, &audio_Confg, &I2S_SpeedConfig[0]);
	audio_Confg.SampleRate = 11025;
	I2S_RateFind(LPC_I2S0, &audio_Confg, &I2S_SpeedConfig[1]);
	audio_Confg.SampleRate = 16000;
	I2S_RateFind(LPC_I2S0, &audio_Confg, &I2S_SpeedConfig[2]);
	audio_Confg.SampleRate = 22050;
	I2S_RateFind(LPC_I2S0, &audio_Confg, &I2S_SpeedConfig[3]);
	audio_Confg.SampleRate = 32000;
	I2S_RateFind(LPC_I2S0, &audio_Confg, &I2S_SpeedConfig[4]);
	/* Although the founded rate is very accuracy,
	the XTAL doesn't always match its frequency.
	Increase the sample rate a little bit will really meet the USB speed. */
#if defined(__LPC43XX__ )
	audio_Confg.SampleRate = 44101; /* a bit higher for 204Mhz device */
#else
	audio_Confg.SampleRate = 44100;
#endif
	I2S_RateFind(LPC_I2S0, &audio_Confg, &I2S_SpeedConfig[5]);
#if defined(__LPC43XX__ )
	audio_Confg.SampleRate = 48001; /* a bit higher for 204Mhz device */
#else
	audio_Confg.SampleRate = 48000;
#endif
	I2S_RateFind(LPC_I2S0, &audio_Confg, &I2S_SpeedConfig[6]);

#if defined(USB_DEVICE_ROM_DRIVER)
	UsbdAdc_Init(&Speaker_Audio_Interface);
#endif

	Audio_Init(CurrentAudioSampleFrequency);

	for (;;)
	{
#if !defined(USB_DEVICE_ROM_DRIVER)
		Audio_Device_USBTask(&Speaker_Audio_Interface);
		USB_USBTask(Speaker_Audio_Interface.Config.PortNumber,USB_MODE_Device);
#endif
	}
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
	Board_Init();
	USB_Init(Speaker_Audio_Interface.Config.PortNumber, USB_MODE_Device);
}

#if !defined(USB_DEVICE_ROM_DRIVER)
/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	//printf("%s()\r\n", __FUNCTION__);
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	//printf("%s()\r\n", __FUNCTION__);
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	//printf("%s()\r\n", __FUNCTION__);

	bool ConfigSuccess = true;

	ConfigSuccess &= Audio_Device_ConfigureEndpoints(&Speaker_Audio_Interface);

	//	LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	//printf("%s()\r\n", __FUNCTION__);
	Audio_Device_ProcessControlRequest(&Speaker_Audio_Interface);
}

void EVENT_Audio_Device_StreamStartStop(USB_ClassInfo_Audio_Device_t *const AudioInterfaceInfo)
{
	//printf("%s(%s)\r\n", __FUNCTION__, AudioInterfaceInfo->State.InterfaceEnabled == true ? "Start":"Stop");
	/* reset audio buffer */
	Audio_Reset_Data_Buffer();
	if (AudioInterfaceInfo->State.InterfaceEnabled == true)
		MAX98357A_Enable();
	else
		MAX98357A_Disable();
}

#ifndef USB_AUDIO_2DOT0
/** Audio class driver callback for the setting and retrieval of streaming endpoint properties. This callback must be implemented
 *  in the user application to handle property manipulations on streaming audio endpoints.
 */
bool CALLBACK_Audio_Device_GetSetEndpointProperty(USB_ClassInfo_Audio_Device_t *const AudioInterfaceInfo,
												  const uint8_t EndpointProperty,
												  const uint8_t EndpointAddress,
												  const uint8_t EndpointControl,
												  uint16_t *const DataLength,
												  uint8_t *Data)
{
	//printf("%s()\r\n", __FUNCTION__);

	/* Check the requested endpoint to see if a supported endpoint is being manipulated */
	if (EndpointAddress == (ENDPOINT_DIR_OUT | Speaker_Audio_Interface.Config.DataOUTEndpointNumber)) {
		/* Check the requested control to see if a supported control is being manipulated */
		if (EndpointControl == AUDIO_EPCONTROL_SamplingFreq) {
			switch (EndpointProperty) {
			case AUDIO_REQ_SetCurrent:
				/* Check if we are just testing for a valid property, or actually adjusting it */
				if (DataLength != NULL) {
					/* Set the new sampling frequency to the value given by the host */
					CurrentAudioSampleFrequency =
						(((uint32_t) Data[2] << 16) | ((uint32_t) Data[1] << 8) | (uint32_t) Data[0]);
					if (CurrentAudioSampleFrequency > AUDIO_MAX_SAMPLE_FREQ) {
						return false;
					}
					Audio_DeInit();
					Audio_Init(CurrentAudioSampleFrequency);
				}

				return true;

			case AUDIO_REQ_GetCurrent:
				/* Check if we are just testing for a valid property, or actually reading it */
				if (DataLength != NULL) {
					*DataLength = 3;

					Data[2] = (CurrentAudioSampleFrequency >> 16);
					Data[1] = (CurrentAudioSampleFrequency >> 8);
					Data[0] = (CurrentAudioSampleFrequency &  0xFF);
				}

				return true;
			}
		}
	}

	return false;
}
#else
/** Audio class driver callback for the setting and retrieval of streaming properties. This callback must be implemented
 *  in the user application to handle property manipulations on streaming audio properties.
 */
bool CALLBACK_Audio_Device_GetSetProperty(USB_ClassInfo_Audio_Device_t *const AudioInterfaceInfo,
										  const uint8_t RequestType,
										  const uint8_t Request,
										  const uint8_t Control,
										  uint16_t *const DataLength,
										  uint8_t *Data)
{
	//printf("%s(0x%02x, 0x%02x, 0x%02x)\r\n", __FUNCTION__, RequestType, Request, Control);

	if ((RequestType & CONTROL_REQTYPE_TYPE) == REQTYPE_CLASS)
	{
		switch (Control)
		{
		case AUDIO_CS_SAM_FREQ_CONTROL:
			switch (Request)
			{
			case AUDIO_REQ_Cur:
				if (RequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
				{
					if ( (DataLength != NULL) && (Data != NULL) )
					{
						CurrentAudioSampleFrequency =
							( ((uint32_t) Data[3] << 24) | ((uint32_t) Data[2] << 16) | ((uint32_t) Data[1] << 8) | (uint32_t) Data[0] );
						if (CurrentAudioSampleFrequency > AUDIO_MAX_SAMPLE_FREQ) {
							return false;
						}
						Audio_DeInit();
						Audio_Init(CurrentAudioSampleFrequency);
						//printf("Audio Sample Frequency: %dHz\r\n", CurrentAudioSampleFrequency);
					}
					return true;
				}
				else if (RequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
				{
					if ( (DataLength != NULL) && (Data != NULL) )
					{
						*DataLength = sizeof(CurrentAudioSampleFrequency);
						Data[0] = (uint8_t) ((CurrentAudioSampleFrequency & 0x000000ff) >>  0);
						Data[1] = (uint8_t) ((CurrentAudioSampleFrequency & 0x0000ff00) >>  8);
						Data[2] = (uint8_t) ((CurrentAudioSampleFrequency & 0x00ff0000) >> 16);
						Data[3] = (uint8_t) ((CurrentAudioSampleFrequency & 0xff000000) >> 24);
						return true;
					}
				}
				break;
			case AUDIO_REQ_Range:
				if (RequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
				{
					Get_USB_Control_Ranges(DataLength, Data);
					return true;
				}
				break;
			case AUDIO_REQ_Mem:
				break;
			}
			break;
		case AUDIO_CS_CLOCK_VALID:
			switch (Request)
			{
			case AUDIO_REQ_Cur:
				if (RequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
				{
					*DataLength = 1;
					Data[0] = 0x01;
					return true;
				}
				break;
			case AUDIO_REQ_Range:
			case AUDIO_REQ_Mem:
				break;
			}
			break;
		}
	}

	return false;
}
#endif

#else

#endif
