/*
 * crp.c
 * Source file to create CRP word expected by Code Red linker
 */

#ifdef __CODE_RED
#include <NXP/crp.h>

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;

#endif
