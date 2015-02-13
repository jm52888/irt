/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
  
/** @file
 *
 * @defgroup nrf_dfu_ble_svc DFU BLE SVC 
 * @{
 *
 * @brief DFU BLE SVC in bootloader. The DFU BLE SuperVisor Calls allow an application to execute
 *        functions in the installed bootloader. 
 *
 * @details This module implements handling of SuperVisor Calls in the bootloader. 
 *          SuperVisor Calls allow for an application to execute calls into the bootloader.
 *          Currently, it is possible to exchange bonding information (like keys) from the 
 *          application to a bootloader supporting DFU OTA using BLE, so the update process can be 
 *          done through an already existing bond.
 *
 * @note The application must make sure that all SuperVisor Calls (SVC) are forwarded to the 
 *       bootloader to ensure correct behavior. Forwarding of SVCs to the bootloader is 
 *       done using the SoftDevice SVC \ref sd_softdevice_vector_table_base_set with the value 
 *       present in \c NRF_UICR->BOOTLOADERADDR.
 */
 
#ifndef DFU_BLE_SVC_H__
#define DFU_BLE_SVC_H__

#include "nrf_svc.h"
#include <stdint.h>
#include "ble_gap.h"
#include "nrf51.h"
#include "nrf_soc.h"
#include "nrf_error_sdm.h"

#define BOOTLOADER_SVC_BASE 0x0  /**< The number of the lowest SVC number reserved for the bootloader. */

/**@brief The SVC numbers used by the SVC functions in the SoC library. */
enum BOOTLOADER_SVCS
{
    DFU_BLE_SVC_SET_PEER_DATA = BOOTLOADER_SVC_BASE,
    BOOTLOADER_SVC_LAST
};

/**@brief   DFU Peer data structure.
 *
 * @details This structure contains peer data needed for connection to a bonded device during DFU.
 *          The peer data must be provided by the application to the bootloader during buttonless
 *          update. See @ref dfu_ble_svc_set_peer_data. It contains bond information about the
 *          desired DFU peer.
 */
typedef struct
{
    ble_gap_enc_info_t enc_info;
    ble_gap_irk_t      irk;
    ble_gap_addr_t     addr;
} dfu_ble_peer_data_t;

/**@brief   SVC Function for setting peer data containing address, IRK, and LTK to establish bonded
 *          connection in DFU mode.
 *
 * @param[in] p_peer_data  Pointer to the peer data containing keys for the connection.
 *
 * @retval NRF_ERROR_NULL If a NULL pointer was provided as argument.
 * @retval NRF_SUCCESS  If the function completed successfully.
 */
SVCALL(DFU_BLE_SVC_SET_PEER_DATA, uint32_t, dfu_ble_svc_set_peer_data(dfu_ble_peer_data_t * p_peer_data));

#endif // DFU_BLE_SVC_H__

/** @} */
