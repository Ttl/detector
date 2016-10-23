/*
 * @brief Manley 11U68 board LCD driver
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
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

#include "board.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/* LCD (SPI_CS) port and pin */
#define LCDCS_PORT          0
#define LCDCS_PIN           2

/* 100nS SPI clock */
#define LCDSSP_BITRATE      (25000000)

/* SSP device mapped to LCD */
#define LCD_SSP             LPC_SSP0

/* Horizontal and vertical sizes of the LCD */
#define LCD_HORIZ_SIZE      320
#define LCD_VERT_SIZE       240

/* Flag indicating portrait or landscape orientation */
static bool orientationPortrait;
static uint8_t cont_mem;

/* Window data */
static const struct {
	uint16_t width;
	uint16_t height;
} lcd_size[] = {
	{LCD_HORIZ_SIZE, LCD_VERT_SIZE},/* Landscape mode */
	{LCD_VERT_SIZE, LCD_HORIZ_SIZE},/* Portrait mode */
};
static uint16_t line_buffer[LCD_HORIZ_SIZE + 1];
static const uint8_t InitLCD_dat[] = {
	/* Data length , Command, Data */

	/* Power control B */
	3,  0xCF, 0x00, 0xC1, 0x30,
	/* Power on sequence control */
	4,  0xED, 0x64, 0x03, 0x12, 0x81,
	/* Driver timing control A */
	3,  0xE8, 0x85, 0x10, 0x7A,
	/* Power control A */
	5,  0xCB, 0x39, 0x2C, 0x00, 0x34, 0x02,
	/* Pump ratio control */
	1,  0xF7, 0x20,
	/* Driver timing control B */
	2,  0xEA, 0x00, 0x00,
	/* Power control */
	1,  0xC0, 0x1B,
	/* Power control */
	1,  0xC1, 0x01,
	/* VCM control */
	2,  0xC5, 0x45, 0x25,
	/* VCM control2 */
	1,  0xC7, 0xB7,
	/* Memory Access Control */
	1,  0x36, 0x28,
	/*Pixel Format Set */
	1,  0x3A, 0x55,
	/*Frame Rate Control */
	2,  0xB1, 0x00, 0x1A,
	/* Display Function Control */
	2,  0xB6, 0x0A, 0x82,
	/* 3Gamma Function Disable */
	1,  0xF2, 0x00,
	/* Gamma curve selected */
	1,  0x26, 0x01,
	/* Set Gamma */
	15, 0xE0, 0x0F, 0x2A, 0x28, 0x08, 0x0E, 0x08, 0x54, 0xA9, 0x43, 0x0A, 0x0F, 0x00, 0x00, 0x00, 0x00,
	/* Set Gamma */
	15, 0xE1, 0x00, 0x15, 0x17, 0x07, 0x11, 0x06, 0x2B, 0x56, 0x3C, 0x05, 0x10, 0x0F, 0x3F, 0x3F, 0x0F,
	/* Address Column set */
	4,  0x2A, 0x00, 0x00, 0x01, 0x3F,
	/* Address Page set */
	4,  0x2B, 0x00, 0x00, 0x00, 0xEF,
	/* Sleep OUT */
	0,  0x11,
	/* Display ON */
	0,  0x29,
};

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Read data to LCD controller via SPI */
static uint16_t Board_LCD_ReadData(void)
{
	/* FIXME: Implement later */
	return 0;
}

/* Initialize SPI interface for board */
static void Board_LCDSSP_Init(void)
{
	/* Enable SSP clocking and setup SSP defaults */
	Chip_SSP_Init(LCD_SSP);

	/* 8 bits data, SPI0 mode */
	Chip_SSP_Set_Mode(LCD_SSP, SSP_MODE_MASTER);
	Chip_SSP_SetFormat(LCD_SSP, SSP_BITS_9, SSP_FRAMEFORMAT_SPI, SSP_CLOCK_CPHA0_CPOL0);

	/* Set SPI clock rate for LCD controller */
	Chip_SSP_SetBitRate(LCD_SSP, LCDSSP_BITRATE);

	/* Enable SSP */
	Chip_SSP_Enable(LCD_SSP);
}

/* Initialize the LCD Panel with startup values */
static void Board_LCDPanel_Init(void)
{
	int i, len = 0;
	for (i = 0; i < sizeof(InitLCD_dat) / sizeof(uint8_t); i += 2 + len) {
		len = InitLCD_dat[i];
		Board_LCD_WriteData(InitLCD_dat[i + 1], &InitLCD_dat[i + 2], len);
	}
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Initialize LCD controller */
void Board_LCD_Init(bool portrait)
{
	volatile int i;
	/* LCD_CS (SPI_CS) mapped to PIO0_2, use GPIO to control frame */
	Chip_IOCON_PinMuxSet(LPC_IOCON, LCDCS_PORT, LCDCS_PIN,
						 (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGMODE_EN));
	/* BLC mapped to PIO1_20, not used */

	/* Setup GPIO for LCD CS to output, initial LCD CS state is high */
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, LCDCS_PORT, LCDCS_PIN);

	/* Initialize SPI for board LCD controller */
	Board_LCDSSP_Init();

	/* Save orientation */
	orientationPortrait = portrait;

	/* Initial LCD */
	Board_LCDPanel_Init();

	if (portrait) {	/* set to portrait mode */
		uint8_t ch = 0x8;
		Board_LCD_WriteData(0x36, &ch, 1);
	}
	for (i = 0; i < 0xFFFFF; i++) {}
}

/* Writes a command and data to LCD module multiple times */
void Board_LCD_WriteDataRaw(uint8_t cmd, const uint8_t *data, int length, int repeat)
{
	int i, len = length * repeat, ind = 0;
	Chip_SSP_DATA_SETUP_T sfr = {0};

	cont_mem = (cmd & ~0x10) == 0x2C;
	sfr.tx_data = line_buffer;
	line_buffer[0] = (uint16_t) cmd;
	sfr.length = 2;
	do {
		for (i = 0; i < len && i < (sizeof(line_buffer) / sizeof(line_buffer[0])) - 1; i++) {
			line_buffer[i + 1] = 0x100 | (uint16_t) data[ind++];
			if (ind >= length) {
				ind -= length;
			}
		}
		sfr.length += i << 1;
		len -= i;

		/* Enable LCD chip select */
		Chip_GPIO_SetPinState(LPC_GPIO, LCDCS_PORT, LCDCS_PIN, false);
		while (sfr.tx_cnt < sfr.length) {
			Chip_SSP_Int_RWFrames16Bits(LCD_SSP, &sfr);
		}
		sfr.tx_data = &line_buffer[1];
		sfr.length = sfr.tx_cnt = sfr.rx_cnt = 0;
	} while (len);

	/* Diable Chip Select after xfer done */
	while (Chip_SSP_GetStatus(LCD_SSP, SSP_STAT_BSY)) {}
	Chip_GPIO_SetPinState(LPC_GPIO, LCDCS_PORT, LCDCS_PIN, true);
}

/* Sets the current pixel position */
void Board_LCD_GotoXY(uint16_t x, uint16_t y)
{
	uint8_t tmp[4];

	/* Set X */
	tmp[0] = tmp[2] = x >> 8;
	tmp[1] = tmp[3] = x & 0xFF;
	Board_LCD_WriteData(0x2A, tmp, 4);

	/* Set Y */
	tmp[0] = tmp[2] = y >> 8;
	tmp[1] = tmp[3] = y & 0xFF;
	Board_LCD_WriteData(0x2B, tmp, 4);
}

/* Return size of display in pixels */
void Board_LCD_GetLCDSize(uint16_t *pWidth, uint16_t *pHeight)
{
	*pWidth = lcd_size[orientationPortrait].width;
	*pHeight = lcd_size[orientationPortrait].height;
}

/* Sets an active window on the display */
void Board_LCD_SetWindowSize(uint16_t xPos, uint16_t yPos, int width, int height)
{
	uint8_t tmp[4];

	if (!width || !height) {
		return;
	}

	if (width < 0) {
		width = lcd_size[orientationPortrait].width;
	}
	if (height < 0) {
		height = lcd_size[orientationPortrait].height;
	}

	tmp[0] = xPos >> 8;
	tmp[1] = xPos & 0xFF;
	tmp[2] = (xPos + width - 1) >> 8;
	tmp[3] = (xPos + width - 1) & 0xFF;
	Board_LCD_WriteData(0x2A, tmp, 4);

	tmp[0] = yPos >> 8;
	tmp[1] = yPos & 0xFF;
	tmp[2] = (yPos + height - 1) >> 8;
	tmp[3] = (yPos + height - 1) & 0xFF;
	Board_LCD_WriteData(0x2B, tmp, 4);
}

/* Fills a rectangle area with a given color */
void Board_LCD_FillRect(uint16_t xPos, uint16_t yPos, int width, int height, uint16_t color)
{
	uint8_t tmp[2];

	if (width < 0) {
		width = lcd_size[orientationPortrait].width;
	}
	if (height < 0) {
		height = lcd_size[orientationPortrait].height;
	}

	Board_LCD_SetWindowSize(xPos, yPos, width, height);
	tmp[0] = color >> 8;
	tmp[1] = color & 0xFF;
	Board_LCD_WriteDataRaw(0x2C, tmp, 2, width * height);
}

/* Sets the active window to the passed color */
void Board_LCD_Clear(uint16_t color)
{
	Board_LCD_FillRect(0, 0, -1, -1, color);
}

/* Copies an image with the passed size at the display coordinates */
void Board_LCD_CopyImage(const uint8_t *buff, uint16_t xPos, uint16_t yPos, uint16_t hsize, uint16_t vsize)
{
	Board_LCD_SetWindowSize(xPos, yPos, hsize, vsize);
	Board_LCD_WriteData(0x2C, (const uint8_t *) buff, (hsize * vsize) << 1);
}

/* Places a pixel at the display coordinates */
void Board_LCD_PutPixel(uint16_t xPos, uint16_t yPos, uint16_t color)
{
	Board_LCD_GotoXY(xPos, yPos);
	Board_LCD_SetPixel(color);
}

/* Gets the pixel color at the display coordinates */
uint16_t Board_LCD_GetPixel(uint16_t xPos, uint16_t yPos)
{
	/* FIXME: Implement later */
	return 0;
}

/* Sets a pixel location at the display coordinates */
void Board_LCD_SetPixel(uint16_t color)
{
	uint8_t tmp[2];
	tmp[0] = color >> 8;
	tmp[1] = color & 0xFF;
	Board_LCD_WriteData(0x2C | (cont_mem << 4), tmp, 2);
}
