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

#ifndef __BOARD_LCD_H_
#define __BOARD_LCD_H_

#include "chip.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup BOARD_LCD_MANLEY_11U68 LPC11U68 Manley board LCD driver
 * @ingroup BOARD_MANLEY_11U68
 * The Manley board LCD driver provides a set of functions that can be
 * used to initialize and use the LCD. The Manley board's LCD interface is
 * via the SPI, so using this driver requires setting up and using the
 * SSP controller and driver.<br>
 * <b>This driver has been adopted from code included on the CDROM with the
 * Manley board. All LCD functions are blocking.</b>
 * @{
 */

/**
 * @brief	Initialize LCD controller
 * @param	portrait	: true for portrait orientation, false for landscape
 * @return	Nothing
 */
void Board_LCD_Init(bool portrait);

/**
 * @brief	Return size of display in pixels
 * @param	pWidth	: Pointer to data entry to fill with horizontal resolution
 * @param	pHeight	: Pointer to data entry to fill with vertical resolution
 * @return	Nothing
 * @note	Returns the full size of the display in pixels regardless of
 *			active window size.
 */
void Board_LCD_GetLCDSize(uint16_t *pWidth, uint16_t *pHeight);

/**
 * @brief	Sets an active window on the display
 * @param	xPos	: Horizontal coordinate of the window
 * @param	yPos	: Vertical coordinate of the window
 * @param	width	: Width of the window
 * @param	height	: Height of the window
 * @return	Nothing
 * @note	If @a height is less than 0, then height will be the
 * entire height of the LCD, if @a width is less than 0 then width
 * will be the entire width of the LCD. If height or width is 0 then
 * the function does nothing.
 */
void Board_LCD_SetWindowSize(uint16_t xPos, uint16_t yPos, int width, int height);

/**
 * @brief	Returns current size and left/top coordinates of active window
 * @param	pXPos	: Pointer to horizontal coordinate of the window to set
 * @param	pYPos	: Pointer to vertical coordinate of the window to set
 * @param	pWidth	: Pointer to width of the window to set
 * @param	pHeight	: Pointer to height of the window to set
 * @return	Nothing
 */
void Board_LCD_GetWindowSize(uint16_t *pXPos, uint16_t *pYPos, uint16_t *pWidth, uint16_t *pHeight);

/**
 * @brief	Sets the active window to the passed color
 * @param	color	: 16-bit RGB565 color to set the display to
 * @return	Nothing
 */
void Board_LCD_Clear(uint16_t color);

/**
 * @brief	Copies an image with the passed size at the display coordinates
 * @param	buff	: Pointer to 16-bit RGB565 image data (byte array)
 * @param	hsize	: Horizontal size of the image data
 * @param	vsize	: Vertical size of the image data
 * @param	xPos	: X coordinate on display to start drawing image
 * @param	yPos	: Y coordinate on display to start drawing image
 * @return	Nothing
 * @note	Supports RGB565 only. Images data should be organized from top to
 *			bottom and left to right.
 */
void Board_LCD_CopyImage(const uint8_t *buff, uint16_t hsize, uint16_t vsize, uint16_t xPos, uint16_t yPos);

/**
 * @brief	Places a pixel at the display coordinates
 * @param	xPos	: X coordinate on display to start drawing image
 * @param	yPos	: Y coordinate on display to start drawing image
 * @param	color	: 16-bit RGB565 color for the pixel
 * @return	Nothing
 * @note	Supports RGB565 only.
 */
void Board_LCD_PutPixel(uint16_t xPos, uint16_t yPos, uint16_t color);

/**
 * @brief	Sets color current pixel and increments to next pixel
 * @param	color	: Color to set (RGB565 format)
 * @return	Nothing
 */
void Board_LCD_SetPixel(uint16_t color);

/**
 * @brief	Gets the pixel color at the display coordinates
 * @param	xPos	: X coordinate on display to start drawing image
 * @param	yPos	: Y coordinate on display to start drawing image
 * @return	16-bit RGB565 color for the pixel
 * @note	Supports RGB565 only. This function may not be working correctly.
 */
uint16_t Board_LCD_GetPixel(uint16_t xPos, uint16_t yPos);

/**
 * @brief	Draws a filled rectangle
 * @param	xPos	: X coordinate top left
 * @param	yPos	: Y coordinate top left
 * @param	width	: Width of rectangle
 * @param	height	: Height of rectangle
 * @param	color	: Color to fill rectangle with
 * @return	None
 */
void Board_LCD_FillRect(uint16_t xPos, uint16_t yPos, int width, int height, uint16_t color);

/**
 * @brief	Writes a command and data to LCD module multiple times
 * @param	cmd		: Command to be sent
 * @param	data	: Data associated with command @a cmd
 * @param	length	: Length of @a data
 * @param	repeat	: Number of time the data should repeat
 * @return	Nothing
 */
void Board_LCD_WriteDataRaw(uint8_t cmd, const uint8_t *data, int length, int repeat);

/**
 * @brief	Writes a command and data to LCD module
 * @param	cmd		: Command to be sent
 * @param	data	: Data associated with command @a cmd
 * @param	length	: Length of @a data
 * @return	Nothing
 */
STATIC INLINE void Board_LCD_WriteData(uint8_t cmd, const uint8_t *data, int length)
{
	Board_LCD_WriteDataRaw(cmd, data, length, 1);
}

/**
 * @brief	Sets the current pixel position
 * @param	xPos	: x-position (in pixels)
 * @param	yPos	: y-position (in pixels)
 * @return	Nothing
 */
void Board_LCD_GotoXY(uint16_t xPos, uint16_t yPos);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_LCD_H_ */
