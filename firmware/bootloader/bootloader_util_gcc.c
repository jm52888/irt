/***********************************************************************************************************************
  GCC-spezifier Code für den Bootloader
------------------------------------------------------------------------------------------------------------------------
  \file       bootloader_util_gcc.c
  \creation   2014-02-21, Joe Merten
***********************************************************************************************************************/

#include "bootloader_util.h"
#include <stdint.h>
#include "nordic_common.h"
#include "bootloader_types.h"
#include "dfu_types.h"
#include "irt_common.h"
#include "debug.h"

/**@brief Debug logging for main module.
 *
 */
#ifdef ENABLE_DEBUG_LOG
#define BL_LOG debug_log
#else
#define BL_LOG(...)
#endif // ENABLE_DEBUG_LOG

bootloader_settings_t  m_boot_settings __attribute__((section(".bootloader_settings_sect"))) __attribute__((used)) = {BANK_VALID_APP};
uint16_t  m_irt_features __attribute__((section(".bootloader_settings_sect"))) __attribute__((used)) = IRT_FEATURES;

uint32_t m_uicr_bootloader_start_address __attribute__((section(".NRF_UICR_BOOT_START_SECT"))) __attribute__((used)) = BOOTLOADER_REGION_START;

void bootloader_util_app_start(uint32_t start_addr)
{
#if 0
    { /* First approach to start the application */
        typedef void (*application_main_t)();
        application_main_t application_main = *(application_main_t*)(start_addr+4);
        application_main();
    }
#else
    { /* Second approach to start the application */
        asm volatile(" LDR   R0, =0x20000          \n\t" // Assign app code address
                     " LDR   R2, [R0]              \n\t" // Get App MSP
                     " MSR   MSP, R2               \n\t" // Set the main stack pointer to the applications MSP
                     " LDR   R3, [R0, #0x00000004] \n\t" // Get application reset vector address
                     " BX    R3                    \n\t" // No return - stack code is now activated only through SVC and plain interrupts
                     " .ALIGN                      ");
    }
#endif
}

void bootloader_util_settings_get(const bootloader_settings_t** pp_bootloader_settings)
{
    // Read only pointer to bootloader settings in flash.
    //*pp_bootloader_settings = (bootloader_settings_t*)BOOTLOADER_SETTINGS_ADDRESS;
	*pp_bootloader_settings = &m_boot_settings;
}
