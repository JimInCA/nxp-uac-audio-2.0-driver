/*
 * @brief USB Device Descriptors, for library use when in USB device mode. Descriptors are special
 *        computer-readable structures which the host requests upon device enumeration, to determine
 *        the device's capabilities and functions
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

#include "Descriptors.h"

#ifndef USB_AUDIO_2DOT0
#define AUDIO_CONTROL_INPUT_TERMINAL_ID    0x01
#define AUDIO_CONTROL_OUTPUT_TERMINAL_ID   0x02
#define POLLING_INTERVAL                   0x01
#else
#define AUDIO_CONTROL_CLOCK_SOURCE_UNIT_ID 0x10
#define AUDIO_CONTROL_INPUT_TERMINAL_ID    0x20
#define AUDIO_CONTROL_FEATURE_UNIT_ID      0x30
#define AUDIO_CONTROL_OUTPUT_TERMINAL_ID   0x40
#define POLLING_INTERVAL                   0x02
#endif

/** Device descriptor structure. This descriptor, located in FLASH memory, describes the overall
 *  device characteristics, including the supported USB version, control endpoint size and the
 *  number of device configurations. The descriptor is read out by the USB host when the enumeration
 *  process begins.
 */
USB_Descriptor_Device_t DeviceDescriptor = {
	.Header                 = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

	.USBSpecification       = VERSION_BCD(02.00),
#ifndef USB_AUDIO_2DOT0
	.Class                  = USB_CSCP_NoDeviceClass,
	.SubClass               = USB_CSCP_NoDeviceSubclass,
	.Protocol               = USB_CSCP_NoDeviceProtocol,
#else
	.Class                  = USB_CSCP_IADDeviceClass,
	.SubClass               = USB_CSCP_IADDeviceSubclass,
	.Protocol               = USB_CSCP_IADDeviceProtocol,
#endif

	.Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,

	.VendorID               = 0x1fc9,	/* NXP */
	.ProductID              = 0x2046,
	.ReleaseNumber          = VERSION_BCD(00.01),

	.ManufacturerStrIndex   = 0x01,
	.ProductStrIndex        = 0x02,
	.SerialNumStrIndex      = 0x03,

	.NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

/** Configuration descriptor structure. This descriptor, located in FLASH memory, describes the usage
 *  of the device in one of its supported configurations, including information about any device interfaces
 *  and endpoints. The descriptor is read out by the USB host during the enumeration process when selecting
 *  a configuration so that the host may correctly communicate with the USB device.
 */
USB_Descriptor_Configuration_t ConfigurationDescriptor = {
	.Config = {
		.Header                   = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},

		.TotalConfigurationSize   = sizeof(USB_Descriptor_Configuration_t) - 1,		// termination byte not included in size
		.TotalInterfaces          = 2,

		.ConfigurationNumber      = 1,
		.ConfigurationStrIndex    = NO_DESCRIPTOR,

		.ConfigAttributes         = (USB_CONFIG_ATTR_BUSPOWERED | USB_CONFIG_ATTR_SELFPOWERED),

		.MaxPowerConsumption      = USB_CONFIG_POWER_MA(100)
	},

#ifdef USB_AUDIO_2DOT0
	.InterfaceAssociation = {
		.bLength                  = sizeof(USB_StdDescriptor_Interface_Association_t), /*  Size of the descriptor, in bytes. */
		.bDescriptorType          = DTYPE_InterfaceAssociation,                        /*  Type of the descriptor, either a value in
		                                                                                *  @ref USB_DescriptorTypes_t or a value
			                                                                            *  given by the specific class.
			                                                                            */
		.bFirstInterface          = 0,                                                 /*  Index of the first associated interface. */
		.bInterfaceCount          = 2,                                                 /*  Total number of associated interfaces. */
		.bFunctionClass           = 1,                                                 /*  Interface class ID. */
		.bFunctionSubClass        = 0,                                                 /*  Interface subclass ID. */
		.bFunctionProtocol        = AUDIO_CSCP_StreamingProtocol,                      /*  Interface protocol ID. */
		.iFunction                = 0,                                                 /*  Index of the string descriptor describing the
			                                                                            *  interface association.
			                                                                            */
	},
#endif

	.Audio_ControlInterface = {
		.Header                   = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

		.InterfaceNumber          = 0,
		.AlternateSetting         = 0,

		.TotalEndpoints           = 0,

		.Class                    = AUDIO_CSCP_AudioClass,
		.SubClass                 = AUDIO_CSCP_ControlSubclass,
		.Protocol                 = AUDIO_CSCP_ControlProtocol,

		.InterfaceStrIndex        = NO_DESCRIPTOR
	},

	.Audio_ControlInterface_SPC = {
		.Header                   = {.Size = sizeof(USB_Audio_Descriptor_Interface_AC_t), .Type = DTYPE_CSInterface},
		.Subtype                  = AUDIO_DSUBTYPE_CSInterface_Header,

#ifndef USB_AUDIO_2DOT0
		.ACSpecification          = VERSION_BCD(01.00),
		.TotalLength              = (sizeof(USB_Audio_Descriptor_Interface_AC_t) +
									 sizeof(USB_Audio_Descriptor_InputTerminal_t) +
									 //sizeof(USB_Audio_StdDescriptor_FeatureUnit_t) +
									 sizeof(USB_Audio_Descriptor_OutputTerminal_t)),

		.InCollection             = 1,
		.InterfaceNumber          = 1,
#else
		// JTN Need to determine following values.
		.ACSpecification          = VERSION_BCD(02.00),
		.bCategory                = 1,  /** Indicating the primary use of this audio function, as intended by the manufacturer. */
        .wTotalLength             = (sizeof(USB_Audio_Descriptor_Interface_AC_t) +
        						     sizeof(USB_Audio_StdDescriptor_Source_Clock_t) +
				                     sizeof(USB_Audio_Descriptor_InputTerminal_t) +
				                     sizeof(USB_Audio_StdDescriptor_FeatureUnit_t) +
				                     sizeof(USB_Audio_Descriptor_OutputTerminal_t)),
									 /** Total number of bytes returned for the class-specific AudioControl interface descriptor.
                                       * Includes the combined length of this descriptor header and all Clock Source, Unit and
                                       * Terminal descriptors.
                                       */
        .bmControls               = 0,  /** Bitmap */
#endif
	},

#ifdef USB_AUDIO_2DOT0
	.Audio_SourceClock = {
			.bLength                  = sizeof(USB_Audio_StdDescriptor_Source_Clock_t), /*  Size of the descriptor, in bytes. */
			.bDescriptorType          = DTYPE_CSInterface,                              /*  Type of the descriptor, either a value in
			                                                                             *  @ref USB_DescriptorTypes_t or a value
				                                                                         *  given by the specific class.
				                                                                         */
			.bDescriptorSubtype      = AUDIO_DSUBTYPE_CSInterface_ClockSource,  /*  Sub type value used to distinguish between audio class-specific descriptors,
			                                                                     *  a value from the @ref Audio_CSInterface_AS_SubTypes_t enum.
			                                                                     */
			.bClockID                = AUDIO_CONTROL_CLOCK_SOURCE_UNIT_ID,  /* Constant uniquely identifying the Clock Source Entity within the audio function.
			                                                                 * This value is used in all requests to address this Entity.
			                                                                 */

			.bmAttributes            = 1,  /* Bitmap: D1..0: Clock Type: 00: External Clock
			                                *                            01: Internal fixed Clock
			                                *                            10: Internal variable Clock
			                                *                            11: Internal programmable Clock
			                                *            D2: Clock synchronized to SOF
			                                *         D7..3: Reserved. Must be set to 0.
			                                */

			.bmControls              = 7,   /* Bitmap: D1..0: Clock Frequency Control
			                                 *         D3..2: Clock Validity Control
			                                 *         D7..4: Reserved. Must be set to 0.
			                                 */

			.bAssocTerminal          = 0,   /* Terminal ID of the Terminal that is associated with this Clock Source. */

			.iClockSource            = 0,   /* Index of a string descriptor, describing the Clock Source Entity. */
	},
#endif

	.Audio_InputTerminal = {
		.Header                   = {.Size = sizeof(USB_Audio_Descriptor_InputTerminal_t), .Type = DTYPE_CSInterface},
		.Subtype                  = AUDIO_DSUBTYPE_CSInterface_InputTerminal,

		.TerminalID               = AUDIO_CONTROL_INPUT_TERMINAL_ID,
		.TerminalType             = AUDIO_TERMINAL_STREAMING,
		.AssociatedOutputTerminal = 0x00,
#ifdef USB_AUDIO_2DOT0
        .bCSourceID               = AUDIO_CONTROL_CLOCK_SOURCE_UNIT_ID, /* ID of Clock Enity to which this Input Terminal is connected. */
#endif
		.TotalChannels            = 2,
		.ChannelConfig            = (AUDIO_CHANNEL_LEFT_FRONT | AUDIO_CHANNEL_RIGHT_FRONT),

		.ChannelStrIndex          = NO_DESCRIPTOR,
#ifdef USB_AUDIO_2DOT0
        .bmControls               = 0,  /* Bitmap controls */
#endif
		.TerminalStrIndex         = NO_DESCRIPTOR
	},

#ifdef USB_AUDIO_2DOT0
	.Audio_FeatureUnit = {
		.bLength				  = sizeof(USB_Audio_StdDescriptor_FeatureUnit_t),
		.bDescriptorType          = DTYPE_CSInterface,
		.bDescriptorSubtype       = AUDIO_DSUBTYPE_CSInterface_Feature,
		.bUnitID                  = AUDIO_CONTROL_FEATURE_UNIT_ID,
		.bSourceID                = AUDIO_CONTROL_INPUT_TERMINAL_ID,
#ifndef USB_AUDIO_2DOT0
		.bControlSize             = 1,
		.bmaControls[0]           = 1,
		.bmaControls[1]           = 2,
		.bmaControls[2]           = 2,
#else
		// JTN: Need to be verified.
		.bmaControls[0]           = 0,
		.bmaControls[1]           = 0,
		.bmaControls[2]           = 0,
		.bmaControls[3]           = 0,
		.bmaControls[4]           = 0,
		.bmaControls[5]           = 0,
		.bmaControls[6]           = 0,
		.bmaControls[7]           = 0,
#endif
		.iFeature                 = 0
	},
#endif

	.Audio_OutputTerminal = {
		.Header                   = {.Size = sizeof(USB_Audio_Descriptor_OutputTerminal_t), .Type = DTYPE_CSInterface},
		.Subtype                  = AUDIO_DSUBTYPE_CSInterface_OutputTerminal,

		.TerminalID               = AUDIO_CONTROL_OUTPUT_TERMINAL_ID,
		.TerminalType             = AUDIO_TERMINAL_OUT_SPEAKER,
		.AssociatedInputTerminal  = 0x00,

#ifndef USB_AUDIO_2DOT0
		.SourceID                 = AUDIO_CONTROL_INPUT_TERMINAL_ID,
#else
		.SourceID                 = AUDIO_CONTROL_FEATURE_UNIT_ID,

        .bCSourceID               = AUDIO_CONTROL_CLOCK_SOURCE_UNIT_ID, /* ID of the Clock Enity to which this output Terminal is connected. */
        .bmControls               = 0,    /* Bitmap Controls */
#endif

		.TerminalStrIndex         = NO_DESCRIPTOR
	},

	.Audio_StreamInterface_Alt0 = {
		.Header                   = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

		.InterfaceNumber          = 1,
		.AlternateSetting         = 0,

		.TotalEndpoints           = 0,

		.Class                    = AUDIO_CSCP_AudioClass,
		.SubClass                 = AUDIO_CSCP_AudioStreamingSubclass,
		.Protocol                 = AUDIO_CSCP_StreamingProtocol,

		.InterfaceStrIndex        = NO_DESCRIPTOR
	},

	.Audio_StreamInterface_Alt1 = {
		.Header                   = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

		.InterfaceNumber          = 1,
		.AlternateSetting         = 1,

#ifndef USB_AUDIO_2DOT0
		.TotalEndpoints           = 1,
#else
		.TotalEndpoints           = 2,
#endif

		.Class                    = AUDIO_CSCP_AudioClass,
		.SubClass                 = AUDIO_CSCP_AudioStreamingSubclass,
		.Protocol                 = AUDIO_CSCP_StreamingProtocol,

		.InterfaceStrIndex        = NO_DESCRIPTOR
	},

	.Audio_StreamInterface_SPC = {
		.Header                   = {.Size = sizeof(USB_Audio_Descriptor_Interface_AS_t), .Type = DTYPE_CSInterface},
		.Subtype                  = AUDIO_DSUBTYPE_CSInterface_General,

		.TerminalLink             = AUDIO_CONTROL_INPUT_TERMINAL_ID,

#ifndef USB_AUDIO_2DOT0
		.FrameDelay               = 1,
		.AudioFormat              = 0x0001
#else
		// JTN Need to determine following values.
		.bmControls               = 0, /** Bitmap  D1..0: Active Alternate Setting Control
		                                 *         D3..2: Valid Alternate Settings Control
		                                 *         D7..4: Reserved. Must be set to 0.
		                                 */
		.bFormatType              = 1,  /** Constant identifying the Format Type the AudioStreaming interface is using. */
		.bmFormats                = 1,  /** The Audio Data Format(s) that can be used to communicate with this interface.
		                                  * See the USB Audio Data Formats document for further details.
		                                  */
		.bNrChannels              = 2,  /* Number of physical channels in the AS Interface audio channel cluster. */
		.bmChannelConfig          = 3,  /* Describes the spatial location of the physical channels. */
		.iChannelNames            = 0,  /* Index of a string descriptor, describing the name of the first physical channel. */
#endif
	},
// Need to start here on Monday!!!
	.Audio_AudioFormat = {
#ifndef USB_AUDIO_2DOT0
		.Header                   = {.Size = sizeof(USB_Audio_Descriptor_Format_t) +
											 sizeof(ConfigurationDescriptor.Audio_AudioFormatSampleRates),
									 .Type = DTYPE_CSInterface},
#else
	    // JTN Need to determine following value.
		.Header                   = {.Size = sizeof(USB_Audio_Descriptor_Format_t),
				                     .Type = DTYPE_CSInterface},
#endif
		.Subtype                  = AUDIO_DSUBTYPE_CSInterface_FormatType,

		.FormatType               = 0x01,
#ifndef USB_AUDIO_2DOT0
		.Channels                 = 0x02,

		.SubFrameSize             = 0x02,
		.BitResolution            = 16,

		.TotalDiscreteSampleRates =
			(sizeof(ConfigurationDescriptor.Audio_AudioFormatSampleRates) / sizeof(USB_Audio_SampleFreq_t)),
#else
		// JTN Need to determine following values.
        .bSubslotSize             = 0x02,  /** The number of bytes occupied by one audio subslot. Can be 1, 2, 3 or 4. */
        .bBitResolution           = 0x10,  /** The number of effectively used bits from the available bits in an audio subslot. */
#endif
	},

#ifndef USB_AUDIO_2DOT0
	.Audio_AudioFormatSampleRates = {
		AUDIO_SAMPLE_FREQ(8000),
		AUDIO_SAMPLE_FREQ(11025),
		AUDIO_SAMPLE_FREQ(16000),
		AUDIO_SAMPLE_FREQ(22050),
		AUDIO_SAMPLE_FREQ(32000),
		AUDIO_SAMPLE_FREQ(44100),
		AUDIO_SAMPLE_FREQ(48000),
	},
#endif

	.Audio_StreamEndpointOut = {
		.Endpoint = {
			.Header              = {.Size = sizeof(USB_Audio_Descriptor_StreamEndpoint_Std_t), .Type = DTYPE_Endpoint},

			.EndpointAddress     = (ENDPOINT_DIR_OUT | AUDIO_STREAM_EPNUM),
			.Attributes          = (EP_TYPE_ISOCHRONOUS | ENDPOINT_ATTR_SYNC | ENDPOINT_USAGE_DATA),
			.EndpointSize        = AUDIO_STREAM_EPSIZE,
			.PollingIntervalMS   = POLLING_INTERVAL
		},

#ifndef USB_AUDIO_2DOT0
		.Refresh                  = 0,
		.SyncEndpointNumber       = 0
#endif
	},

	.Audio_StreamEndpoint_SPC = {
		.Header                   =
		{.Size = sizeof(USB_Audio_Descriptor_StreamEndpoint_Spc_t), .Type = DTYPE_CSEndpoint},
		.Subtype                  = AUDIO_DSUBTYPE_CSEndpoint_General,

		//.Attributes               = (AUDIO_EP_ACCEPTS_SMALL_PACKETS | AUDIO_EP_SAMPLE_FREQ_CONTROL),
		.Attributes               = AUDIO_EP_ACCEPTS_SMALL_PACKETS,

#ifdef USB_AUDIO_2DOT0
		.bmControls               = 0x00,
#endif

		.LockDelayUnits           = 0x00,
		.LockDelay                = 0x0000
	},

#ifdef USB_AUDIO_2DOT0
	.Audio_StreamEndpointIn = {
		.Endpoint = {
			.Header              = {.Size = sizeof(USB_Audio_Descriptor_StreamEndpoint_Std_t), .Type = DTYPE_Endpoint},

			.EndpointAddress     = (ENDPOINT_DIR_IN | 0x01),
			.Attributes          = (0x10 | EP_TYPE_ISOCHRONOUS),
			.EndpointSize        = 0x0004,
			.PollingIntervalMS   = POLLING_INTERVAL
		},
	},
#endif

/*
	.my_bytes = {0x09, 0x04, 0x02, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00,
	             0x09, 0x21, 0x10, 0x01, 0x00, 0x01, 0x02, 0x1f, 0x00,
	             0x07, 0x05, 0x82, 0x03, 0x01, 0x00, 0x0a},
*/
	.Audio_Termination = 0x00
};

/** Language descriptor structure. This descriptor, located in FLASH memory, is returned when the host requests
 *  the string descriptor with index 0 (the first index). It is actually an array of 16-bit integers, which indicate
 *  via the language ID table available at USB.org what languages the device supports for its string descriptors.
 */
uint8_t LanguageString[] = {
	USB_STRING_LEN(1),
	DTYPE_String,
	WBVAL(LANGUAGE_ID_ENG),
};
USB_Descriptor_String_t *LanguageStringPtr = (USB_Descriptor_String_t *) LanguageString;

/** Manufacturer descriptor string. This is a Unicode string containing the manufacturer's details in human readable
 *  form, and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
uint8_t ManufacturerString[] = {
	USB_STRING_LEN(8),
	DTYPE_String,
	WBVAL('A'),
	WBVAL('c'),
	WBVAL('m'),
	WBVAL('e'),
	WBVAL(' '),
	WBVAL('C'),
	WBVAL('o'),
	WBVAL('.'),
};
USB_Descriptor_String_t *ManufacturerStringPtr = (USB_Descriptor_String_t *) ManufacturerString;

/** Product descriptor string. This is a Unicode string containing the product's details in human readable form,
 *  and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
uint8_t ProductString[] = {
	USB_STRING_LEN(17),
	DTYPE_String,
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
};
USB_Descriptor_String_t *ProductStringPtr = (USB_Descriptor_String_t *) ProductString;

/** Serial Number descriptor string. This is a Unicode string containing the device's serial number in human
 *  readable from, and is read out upon request by the host when the appropriate string ID is requested,
 *  listed in the Device Descriptor.
 */
uint8_t SerialNumberString[] = {
	USB_STRING_LEN(6),
	DTYPE_String,
	WBVAL('0'),
	WBVAL('0'),
	WBVAL('0'),
	WBVAL('0'),
	WBVAL('0'),
	WBVAL('1'),
};
USB_Descriptor_String_t *SerialNumberStringPtr = (USB_Descriptor_String_t *) SerialNumberString;

/** This function is called by the library when in device mode, and must be overridden (see library "USB Descriptors"
 *  documentation) by the application code so that the address and size of a requested descriptor can be given
 *  to the USB library. When the device receives a Get Descriptor request on the control endpoint, this function
 *  is called so that the descriptor details can be passed back and the appropriate descriptor sent back to the
 *  USB host.
 */
uint16_t CALLBACK_USB_GetDescriptor(uint8_t corenum,
									const uint16_t wValue,
									const uint8_t wIndex,
									const void * *const DescriptorAddress)
{
	const uint8_t  DescriptorType   = (wValue >> 8);
	const uint8_t  DescriptorNumber = (wValue & 0xFF);

	const void *Address = NULL;
	uint16_t    Size    = NO_DESCRIPTOR;

	switch (DescriptorType) {
	case DTYPE_Device:
		Address = &DeviceDescriptor;
		Size    = sizeof(USB_Descriptor_Device_t);
		break;

	case DTYPE_Configuration:
		Address = &ConfigurationDescriptor;
		Size    = sizeof(USB_Descriptor_Configuration_t);
		break;

	case DTYPE_String:
		switch (DescriptorNumber) {
		case 0x00:
			Address = LanguageStringPtr;
			Size    = pgm_read_byte(&LanguageStringPtr->Header.Size);
			break;

		case 0x01:
			Address = ManufacturerStringPtr;
			Size    = pgm_read_byte(&ManufacturerStringPtr->Header.Size);
			break;

		case 0x02:
			Address = ProductStringPtr;
			Size    = pgm_read_byte(&ProductStringPtr->Header.Size);
			break;

		case 0x03:
			Address = SerialNumberStringPtr;
			Size    = pgm_read_byte(&SerialNumberStringPtr->Header.Size);
			break;

		default:
			printf("ERROR: Unknown DescriptorType %d\r\n", DescriptorType);
			break;
		}

		break;
	}

	*DescriptorAddress = Address;
	return Size;
}

#ifdef USB_AUDIO_2DOT0
static const USB_Cntrl_Ranges ctrl_range = { .numranges = 3, .ranges[0] = {16000, 16000, 0},
															 .ranges[1] = {32000, 32000, 0},
															 .ranges[2] = {48000, 48000, 0} };

void Get_USB_Control_Ranges(uint16_t *DataLength, uint8_t *Data)
{
	if ( (DataLength != NULL) & (Data != NULL) )
	{
		if (*DataLength >= sizeof(USB_Cntrl_Ranges))
			*DataLength = sizeof(USB_Cntrl_Ranges);
		memcpy (Data, &ctrl_range, *DataLength);
	}
}
#endif
