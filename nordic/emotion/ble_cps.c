#include "ble_cps.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_l2cap.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include "led.h"

// https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicsHome.aspx
#define BLE_UUID_SENSOR_LOCATION_CHAR										0x2A5D 		/**< Sensor Location UUID.  */
#define BLE_UUID_CYCLING_POWER_MEASUREMENT_CHAR					0x2A63		/**< Cycling Power Measurement UUID. */
#define BLE_UUID_CYCLING_POWER_FEATURE_CHAR							0x2A65 		/**< Cycling Power Feature UUID. */
#define BLE_UUID_CYCLING_POWER_CONTROL_POINT_CHAR				0x2A66		/**< Cycling Power Control Point UUID. */

// Cycling Power Measurement flag bits: https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.cycling_power_measurement.xml
#define CPS_MEAS_FLAG_PEDAL_POWER_PRESENT        				(0x01 << 0)		
#define CPS_MEAS_FLAG_PEDAL_POWER_REFERENCE        			(0x01 << 1) 
#define CPS_MEAS_FLAG_ACCUM_TORQUE_PRESENT							(0x01 << 2)
#define CPS_MEAS_FLAG_ACCUM_TORQUE_SOURCE								(0x01 << 3)
#define CPS_MEAS_FLAG_WHEEL_REV_PRESENT									(0x01 << 4)
#define CPS_MEAS_FLAG_CRANK_REV_PRESENT									(0x01 << 5)
#define CPS_MEAS_FLAG_EXTREME_FORCE_PRESENT							(0x01 << 6)
#define CPS_MEAS_FLAG_EXTREME_TORQUE_PRESENT						(0x01 << 7)
#define CPS_MEAS_FLAG_EXTREME_ANGLES_PRESENT						(0x01 << 8)
#define CPS_MEAS_FLAG_TOP_DEAD_SPOT_PRESENT							(0x01 << 9)
#define CPS_MEAS_FLAG_BOTTOM_DEAD_SPOT_PRESENT					(0x01 << 10)
#define CPS_MEAS_FLAG_ACCUM_ENERGY_PRESENT							(0x01 << 11)
#define CPS_MEAS_FLAG_OFFSET_COMP_INDICATOR							(0x01 << 12)

#define MAX_CPM_LEN																			34	// This is unscientific? I'm just added up all the bytes of all possible fields in the structure.

/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_hrs       Cycling Power Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_cps_t * p_cps, ble_evt_t * p_ble_evt)
{
    p_cps->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}


/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_hrs       Cycling Power Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_cps_t * p_cps, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_cps->conn_handle = BLE_CONN_HANDLE_INVALID;
}

uint8_t 	m_counter = 1;
uint8_t 	data[3];
uint16_t 	len = 0;

/**@brief Function for handling the Write event.
 *
 * @param[in]   p_cps       Cycling Power Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_cps_t * p_cps, ble_evt_t * p_ble_evt)
{
	ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
	
	len = p_evt_write->len;
	if (p_evt_write->handle == p_cps->cprc_handles.value_handle)
	{
		// TODO: THIS STUFF BELONGS OUTSIDE OF BLE AND PROBABLY IN THE MAIN CONTROLLER.
		// should call a specific event handler for writes to this handle.
		// All the header declarations related to resistance should be moved to the 
		// resistance module.
		// We'll do this after we see how the events show up in ANT+ and make sure that
		// it's consistent. 
		ble_cps_rc_evt_t evt;
		evt.resistance_mode = (ble_cps_resistance_mode_t)p_evt_write->data[0];
		evt.p_value = (uint16_t*)&p_evt_write->data[1];
		
		// TODO: REMOVE DEBUG STUFF:
		data[0] = evt.resistance_mode; 
		data[1] = evt.p_value[0]; 
		data[2] = evt.p_value[1]; 

		// Propogate the set resistance control.
		if (p_cps->evt_handler != NULL)
		{
			p_cps->evt_handler(p_cps, &evt);
		}
		// TODO: blink just so I know it's getting called.
		blink_led2();		
	}
}

/**@brief Function for encoding a Cycling Power Measurement.
 *
 * @param[in]   p_cps              Cycling Power Service structure.
 * @param[in]   p_cps_measurement  Measurement to be encoded.
 * @param[out]  p_encoded_buffer   Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t cps_measurement_encode(ble_cps_t *      p_cps,
                                      ble_cps_meas_t * p_cps_measurement,
                                      uint8_t *        p_encoded_buffer)
{
    uint16_t flags = 0;
    uint8_t len    = 2;

		// TODO: could this encoding be a problem? The actual value should be a *signed* int16.
		// length should be the same, but we don't care about the most significant bit do we?
		// Instantaneous power field
		len += uint16_encode(p_cps_measurement->instant_power, &p_encoded_buffer[len]);

		// NOTE: Skipping Pedal Power Balance, NOT USED
		
		// Accumulated torque field
		if (p_cps->feature & BLE_CPS_FEATURE_ACCUMULATED_TORQUE_BIT)
		{
			if (p_cps_measurement->accum_torque != NULL)
			{
				flags |= CPS_MEAS_FLAG_ACCUM_TORQUE_PRESENT;
				// TODO: If reporting from crank, this flag bit should be 1: CPS_MEAS_FLAG_ACCUM_TORQUE_SOURCE
				len += uint16_encode(p_cps_measurement->accum_torque, &p_encoded_buffer[len]);
			}
		}

		// Wheel revolution data fields (sent as a pair)
		if (p_cps->feature & BLE_CPS_FEATURE_WHEEL_REV_BIT)
		{
			if (p_cps_measurement->accum_wheel_revs != NULL)
			{
				flags |= CPS_MEAS_FLAG_WHEEL_REV_PRESENT;
				len += uint32_encode(p_cps_measurement->accum_wheel_revs, &p_encoded_buffer[len]);
				len += uint16_encode(p_cps_measurement->last_wheel_event_time, &p_encoded_buffer[len]);
			}
		}
		
		// NOTE: Skipping Crank revolution, NOT USED.
		// NOTE: Skipping Extreme force, torque and angles, top and bottom dead spot, NONE USED.
		
		if (p_cps->feature & BLE_CPS_FEATURE_ACCUM_ENERGY_BIT)
		{
			if (p_cps_measurement->accum_energy != NULL)
			{
				flags |= CPS_MEAS_FLAG_ACCUM_ENERGY_PRESENT;
				len += uint16_encode(p_cps_measurement->accum_energy, &p_encoded_buffer[len]);
			}
		}
		    
    // Take 16 bit flags field and seperate into 2 octets.
		p_encoded_buffer[0] = (uint8_t) ((flags & 0x00FF) >> 0);
    p_encoded_buffer[1] = (uint8_t) ((flags & 0xFF00) >> 8);
		
    return len;
}

/*@brief 	This adds a vendor specific characteristic.  Namely, a write property
 *				used by the Wahoo KICKR to set resistance.
 *
 *@note		
 */
static uint32_t resistance_control_char_add(ble_cps_t * p_cps, const ble_cps_init_t * p_cps_init)
{
    uint32_t						err_code;
		ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t        	ble_uuid;
    ble_gatts_attr_md_t attr_md;
		
		memset(&char_md, 0, sizeof(char_md));
		
		char_md.char_props.write = 1;
		char_md.char_props.indicate = 1;
    char_md.p_char_user_desc = NULL;
    char_md.p_char_pf        = NULL;
    char_md.p_user_desc_md   = NULL;
    char_md.p_cccd_md        = NULL;
    char_md.p_sccd_md        = NULL;		

		// 
		// Set vendor specific UUID (A026E005-0A75-4AB3-97FA-F1500F9FEB8B)
		//
		const ble_uuid128_t WAHOO_UUID = { 0x8B, 0xEB, 0x9F, 0x0F, 0x50, 0xF1, 0xFA, 0x97, 0xB3, 0x4A, 0x7D, 0x0A, 0x00, 0x00, 0x26, 0xA0 };
		const uint16_t WAHOO_CHAR = 0xE005;
		uint8_t uuid_type; // = BLE_UUID_TYPE_VENDOR_BEGIN;
	
		err_code = sd_ble_uuid_vs_add(&WAHOO_UUID, &uuid_type);
		if (err_code != NRF_SUCCESS)
		{
			APP_ERROR_HANDLER(err_code);
		}

		ble_uuid.type = uuid_type;
		ble_uuid.uuid = WAHOO_CHAR;

		// Set attribute metadata.
		BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = 0;
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = 4;
    attr_char_value.p_value      = NULL;
    
    return sd_ble_gatts_characteristic_add(p_cps->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_cps->cprc_handles);				
}

static uint32_t cycling_power_feature_char_add(ble_cps_t * p_cps, const ble_cps_init_t * p_cps_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
		uint32_t						init_value_feature;
		uint8_t							init_value_encoded[4];	// 4 bytes to the array (feature total size is 32 = 4*8).
    
    memset(&char_md, 0, sizeof(char_md));
    
    char_md.char_props.read  = 1;
    char_md.p_char_user_desc = NULL;
    char_md.p_char_pf        = NULL;
    char_md.p_user_desc_md   = NULL;
    char_md.p_cccd_md        = NULL;
    char_md.p_sccd_md        = NULL;
    
    BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_CYCLING_POWER_FEATURE_CHAR);
    
    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_cps_init->cps_cpf_attr_md.read_perm;
    attr_md.write_perm = p_cps_init->cps_cpf_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;
		
    memset(&attr_char_value, 0, sizeof(attr_char_value));

		// Encode the feature bits.
		init_value_feature    = p_cps_init->feature;
    init_value_encoded[0] = init_value_feature & 0xFF;
    init_value_encoded[1] = (init_value_feature >> 8) & 0xFF;
		init_value_encoded[2] = (init_value_feature >> 16) & 0xFF;
		init_value_encoded[3] = (init_value_feature >> 24) & 0xFF;

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(uint32_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(uint32_t);
    attr_char_value.p_value      = init_value_encoded;
    
    return sd_ble_gatts_characteristic_add(p_cps->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_cps->cpf_handles);		
}

static uint32_t cycling_power_measurement_char_add(ble_cps_t * p_cps, const ble_cps_init_t * p_cps_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;		
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
		ble_cps_meas_t			initial_cpm;
		uint8_t							encoded_cpm[MAX_CPM_LEN];
    
    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
		BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;
		
    memset(&char_md, 0, sizeof(char_md));
    
    char_md.char_props.notify = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;
    
    BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_CYCLING_POWER_MEASUREMENT_CHAR);
    
    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_cps_init->cps_cpm_attr_md.read_perm;
    attr_md.write_perm = p_cps_init->cps_cpm_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;
    
    memset(&attr_char_value, 0, sizeof(attr_char_value));
	
		memset(&encoded_cpm, 0, sizeof(encoded_cpm));
	
    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = cps_measurement_encode(p_cps, &initial_cpm, encoded_cpm);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = MAX_CPM_LEN;
    attr_char_value.p_value      = encoded_cpm;
    
    return sd_ble_gatts_characteristic_add(p_cps->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_cps->cpm_handles);	
}

static uint32_t sensor_location_char_add(ble_cps_t * p_cps, const ble_cps_init_t * p_cps_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    
    memset(&char_md, 0, sizeof(char_md));
    
    char_md.char_props.read  = 1;
    char_md.p_char_user_desc = NULL;
    char_md.p_char_pf        = NULL;
    char_md.p_user_desc_md   = NULL;
    char_md.p_cccd_md        = NULL;
    char_md.p_sccd_md        = NULL;
    
    BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_SENSOR_LOCATION_CHAR);
    
    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_cps_init->cps_sl_attr_md.read_perm;
    attr_md.write_perm = p_cps_init->cps_sl_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;
    
    memset(&attr_char_value, 0, sizeof(attr_char_value));
    
    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(uint8_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(uint8_t);
    attr_char_value.p_value      = p_cps_init->p_sensor_location;
    
    return sd_ble_gatts_characteristic_add(p_cps->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_cps->sl_handles);	
}

static uint32_t cycling_power_control_point_char_add(ble_cps_t * p_cps, const ble_cps_init_t * p_cps_init)
{
	//ble_cs_ctrlpt_init_t 
	return 0;
}

static uint32_t cycling_power_vector_char_add(ble_cps_t * p_cps, const ble_cps_init_t * p_cps_init)
{
	return 0;
}



void ble_cps_on_ble_evt(ble_cps_t * p_cps, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_cps, p_ble_evt);
            break;
            
        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_cps, p_ble_evt);
            break;
            
        case BLE_GATTS_EVT_WRITE:
            on_write(p_cps, p_ble_evt);
            break;
            
        default:
            break;
    }	
}

uint32_t ble_cps_init(ble_cps_t * p_cps, const ble_cps_init_t * p_cps_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;

    // Initialize service structure
    p_cps->evt_handler                 = p_cps_init->evt_handler;
    p_cps->conn_handle                 = BLE_CONN_HANDLE_INVALID;
		p_cps->feature										 = p_cps_init->feature;
    
    // Add service
    BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_CYCLING_POWER_SERVICE);

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_cps->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

		// Add the resistance control characteristic
		err_code = resistance_control_char_add(p_cps, p_cps_init);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add cycling power feature characteristic
    err_code = cycling_power_feature_char_add(p_cps, p_cps_init);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add cycling power measurement characteristic
    err_code = cycling_power_measurement_char_add(p_cps, p_cps_init);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add sensor location characteristic
    err_code = sensor_location_char_add(p_cps, p_cps_init);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    if (p_cps_init->use_cycling_power_control_point)
    {
        // Add cycling power control point characteristic
        err_code = cycling_power_control_point_char_add(p_cps, p_cps_init);
        if (err_code != NRF_SUCCESS)
        {
            return err_code;
        }
    }
    
    if (p_cps_init->use_cycling_power_vector)
    {
        // Add cycling power control point characteristic
        err_code = cycling_power_vector_char_add(p_cps, p_cps_init);
        if (err_code != NRF_SUCCESS)
        {
            return err_code;
        }
    }
		
    return NRF_SUCCESS;	
}

uint32_t ble_cps_cycling_power_measurement_send(ble_cps_t * p_cps, ble_cps_meas_t * p_cps_meas)
{
    uint32_t err_code;
    
    // Send value if connected and notifying
    if (p_cps->conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        uint8_t								 encoded_cpm[MAX_CPM_LEN];
        uint16_t               len;
        uint16_t               hvx_len;
        ble_gatts_hvx_params_t hvx_params;
        
				memset(&encoded_cpm, 0, sizeof(encoded_cpm));
				
        len     = cps_measurement_encode(p_cps, p_cps_meas, encoded_cpm);
        hvx_len = len;

        memset(&hvx_params, 0, sizeof(hvx_params));
        
        hvx_params.handle   = p_cps->cpm_handles.value_handle;
        hvx_params.type     = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset   = 0;
        hvx_params.p_len    = &hvx_len;
        hvx_params.p_data   = encoded_cpm;
        
        err_code = sd_ble_gatts_hvx(p_cps->conn_handle, &hvx_params);
        if ((err_code == NRF_SUCCESS) && (hvx_len != len))
        {
            err_code = NRF_ERROR_DATA_SIZE;
        }
    }
    else
    {
        err_code = NRF_ERROR_INVALID_STATE;
    }

    return err_code;
}
