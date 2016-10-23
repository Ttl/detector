/*
 * @brief LPC11U6X System Control functions
 *
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
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
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "chip.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/* PDSLEEPCFG register mask */
#define PDSLEEPUSEMASK 0x00000037
#define PDSLEEPMASKTMP (SYSCTL_DEEPSLP_BOD_PD | SYSCTL_DEEPSLP_WDTOSC_PD)
#define PDSLEEPMASK ((PDSLEEPUSEMASK) &~(PDSLEEPMASKTMP))

/* PDWAKECFG register mask */
#define PDWAKEUPUSEMASK 0x00000800
#define PDWAKEUPMASKTMP 0x000025FF

/* PDRUNCFG register mask */
#define PDRUNCFGUSEMASK 0x0000C800
#define PDRUNCFGMASKTMP 0x000025FF

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Setup a pin source for the pin interrupts (0-7) */
void Chip_SYSCTL_SetPinInterrupt(uint32_t intno, uint8_t port, uint8_t pin)
{
	if (port == 0) {
		/* Pins P0.1 to P0.23 only */
		LPC_SYSCTL->PINTSEL[intno] = (uint32_t) pin;
	}
	else {
		/* P1.1 to P1.31 and P2.0 to P2.7 */
		LPC_SYSCTL->PINTSEL[intno] = (uint32_t) ((port - 1) * 32 + pin);
	}
}

/* Setup deep sleep behaviour for power down */
void Chip_SYSCTL_SetDeepSleepPD(uint32_t sleepmask)
{
	/* Update new value */
	LPC_SYSCTL->PDSLEEPCFG = PDSLEEPMASK | (sleepmask & PDSLEEPMASKTMP);
}

/* Setup wakeup behaviour from deep sleep */
void Chip_SYSCTL_SetWakeup(uint32_t wakeupmask)
{
	/* Update new value */
	LPC_SYSCTL->PDWAKECFG = PDWAKEUPUSEMASK | (wakeupmask & PDWAKEUPMASKTMP);
}

/* Power down one or more blocks or peripherals */
void Chip_SYSCTL_PowerDown(uint32_t powerdownmask)
{
	uint32_t pdrun;

	pdrun = LPC_SYSCTL->PDRUNCFG & PDRUNCFGMASKTMP;
	pdrun |= (powerdownmask & PDRUNCFGMASKTMP);

	LPC_SYSCTL->PDRUNCFG = (pdrun | PDRUNCFGUSEMASK);
}

/* Power up one or more blocks or peripherals */
void Chip_SYSCTL_PowerUp(uint32_t powerupmask)
{
	uint32_t pdrun;

	pdrun = LPC_SYSCTL->PDRUNCFG & PDRUNCFGMASKTMP;
	pdrun &= ~(powerupmask & PDRUNCFGMASKTMP);

	LPC_SYSCTL->PDRUNCFG = (pdrun | PDRUNCFGUSEMASK);
}
