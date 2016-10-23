/*
 * @brief Manley 11U68 Sysinit file
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
 #include "string.h"

/* The System initialization code is called prior to the application and
   initializes the board for run-time operation. Board initialization
   includes clock setup and default pin muxing configuration. */

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/* Pin muxing table, only items that need changing from their default pin
   state are in this table. Not every pin is mapped. */
STATIC const PINMUX_GRP_T pinmuxing[] = {
	/* Main oscillator */
	{2, 0,  (IOCON_FUNC1 | IOCON_MODE_INACT)},										/* XTALIN */
	{2, 1,  (IOCON_FUNC1 | IOCON_MODE_INACT)},										/* XTALOUT */

	/* Board LEDs */
	{2, 5,  (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)},	/* LED0 */
	{2, 10, (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)},	/* LED1 */
	{2, 2,  (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)},	/* LED2 */

	/* UART0 and UART1 */
	{0, 18, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)},	/* U0_RXD */
	{0, 19, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)},	/* U0_TXD */
	{0, 13, (IOCON_FUNC4 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)},	/* RXD1 */
	{0, 14, (IOCON_FUNC4 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)},	/* TXD1 */

	/* Joystick button inputs */
	{0, 22, (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)},	/* UP */
	{0, 7,  (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)},	/* LEFT */
	{0, 16, (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)},	/* SELECT */
	{0, 23, (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)},	/* DOWN */
	{1, 23, (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)},	/* RIGHT */

	/* I2C and SPI */
	{0, 4,  (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_STDI2C_EN)},	/* SCL */
	{0, 5,  (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_STDI2C_EN)},	/* SDA */
	{0, 8,  (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)},	/* MISO */
	{0, 9,  (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)},	/* MOSI */
	{2, 7,  (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)},	/* SCLK */
	{1, 13, (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)},	/* MEM_CS (SPI_CS) */

	/* Misc */
	//	{?, ?,  (IOCON_FUNCX | IOCON_MODE_INACT | IOCON_DIGMODE_EN)},	/* Button B1?? */
	{0, 20, (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)},	/* Button B2 */
	{0, 21, (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGMODE_EN)},	/* Button B3 */
	{0, 12, (IOCON_FUNC2 | IOCON_MODE_INACT )},						/* POS (ADC8 input from pot) */
	{0, 3, (IOCON_FUNC1 | IOCON_MODE_INACT )},						/* VBUS */
};

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Sets up system pin muxing */
void Board_SetupMuxing(void)
{
	/* Enable IOCON clock */
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_IOCON);

	Chip_IOCON_SetPinMuxing(LPC_IOCON, pinmuxing, sizeof(pinmuxing) / sizeof(PINMUX_GRP_T));
}

/* Set up and initialize clocking prior to call to main */
void Board_SetupClocking(void)
{
	Chip_SetupXtalClocking();

	/* Select the CLKOUT clocking source */
	Chip_Clock_SetCLKOUTSource(SYSCTL_CLKOUTSRC_MAINSYSCLK, 1);
}

/* Set up and initialize hardware prior to call to main */
void Board_SystemInit(void)
{
	/* Setup system clocking and muxing */
	Board_SetupMuxing();/* Muxing first as it sets up ext oscillator pins */
	Board_SetupClocking();
}
