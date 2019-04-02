#include <onlp/platformi/psui.h>

// #include "i2c_chips.h"
#include "platform.h"

static onlp_psu_info_t psu_info[] =
    {
        {}, /* Not used */
        {
            {ONLP_PSU_ID_CREATE(PSU1_ID), "PSU-1-1", 0},
        },
        {
            {ONLP_PSU_ID_CREATE(PSU2_ID), "PSU-1-2", 0},
        },
        {
            {ONLP_PSU_ID_CREATE(PSU3_ID), "PSU-2-1", 0},
        },
        {
            {ONLP_PSU_ID_CREATE(PSU4_ID), "PSU-2-2", 0},
        }};

struct psuInfo_p temp_info[] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

int onlp_psui_init(void)
{
    return ONLP_STATUS_OK;
}
/*EVT*/
/*dps1100-i2c-25-58 PSU1
dps1100-i2c-26-58 PSU2
dps1100-i2c-28-58 PSU3
dps1100-i2c-29-58 PSU4*/

/* DVT */
/*dps1100-i2c-27-58 PSU1
dps1100-i2c-26-58 PSU2
dps1100-i2c-25-58 PSU3
dps1100-i2c-24-58 PSU4*/

#define PSU1_SENSOR_MAPPING_NAME "dps1100-i2c-27-58"
#define PSU2_SENSOR_MAPPING_NAME "dps1100-i2c-26-58"
#define PSU3_SENSOR_MAPPING_NAME "dps1100-i2c-25-58"
#define PSU4_SENSOR_MAPPING_NAME "dps1100-i2c-24-58"

int onlp_psui_info_get(onlp_oid_t id, onlp_psu_info_t *info_p)
{
    int psu_id;
    char cont_buf[64];
    //struct psuInfo psu;
    //bool psu_status;
    psu_id = ONLP_OID_ID_GET(id);
    *info_p = psu_info[psu_id];
    char psu_name[32] = {0};
    uint8_t psu_stat;
    int absent_status = 1;

    psu_stat = get_psu_status(psu_id);

    if (psu_stat != 0xFF)
    {

        // if(psu_id == PSUL_ID){
        //     absent_status = (psu_stat >> 5) &0x1;
        //     if(!absent_status)
        //         info_p->status |= ONLP_PSU_STATUS_PRESENT;
        // }else{
        //     absent_status = (psu_stat >> 4) &0x1;
        //     if(!absent_status)
        //         info_p->status |= ONLP_PSU_STATUS_PRESENT;
        // }
        absent_status = (psu_stat >> (psu_id - 1)) &0x1;
        if (!absent_status)
            info_p->status |= ONLP_PSU_STATUS_PRESENT;
    }
    else
    {
        return ONLP_STATUS_E_MISSING;
    }
    
    switch(psu_id)
    {
	case PSU1_ID: 
	    (void)strncpy(psu_name, PSU1_SENSOR_MAPPING_NAME, strlen(PSU1_SENSOR_MAPPING_NAME));
	    break;
        case PSU2_ID:
	    (void)strncpy(psu_name, PSU2_SENSOR_MAPPING_NAME, strlen(PSU2_SENSOR_MAPPING_NAME));
	    break;
	case PSU3_ID:
	    (void)strncpy(psu_name, PSU3_SENSOR_MAPPING_NAME, strlen(PSU3_SENSOR_MAPPING_NAME));
	    break;
	case PSU4_ID:
	    (void)strncpy(psu_name, PSU4_SENSOR_MAPPING_NAME, strlen(PSU4_SENSOR_MAPPING_NAME));
	    break;
        default:
	    return ONLP_STATUS_E_MISSING;
    }

    memset(cont_buf, 0, 64);
    if(psu_id == PSU3_ID){
        psu_stat = get_psu_item_content(psu_id, "Serial Number", cont_buf);
        if(psu_stat != 0xFF){
	    (void)strncpy(info_p->serial, cont_buf, strlen(cont_buf));
        }
    }
    else
    {
        psu_stat = get_psu_item_content(psu_id, "Product Serial", cont_buf);
        if(psu_stat != 0xFF){
            (void)strncpy(info_p->serial, cont_buf, strlen(cont_buf));
        }

    }
    memset(cont_buf, 0, 64);
    psu_stat = get_psu_item_content(psu_id, "Product Name", cont_buf);
    if(psu_stat != 0xFF){
        (void)strncpy(info_p->model, cont_buf, strlen(cont_buf));
    }

    memset(cont_buf, 0, sizeof(cont_buf));
    psu_stat = read_psu_inout(NULL, psu_name, "iin", cont_buf);
    if(psu_stat != 0xFF){
	int curr = 0;
	psu_stat = search_current_val(cont_buf, &curr);
        if(psu_stat != 0xFF){
	    info_p->miin = curr;
   	}
    }

    memset(cont_buf, 0, sizeof(cont_buf));
    psu_stat = read_psu_inout(NULL, psu_name, "iout1", cont_buf);
    if(psu_stat != 0xFF){
        int curr = 0;
        psu_stat = search_current_val(cont_buf, &curr);
	if(psu_stat != 0xFF){
	    info_p->miout = curr;
	}
    }
    memset(cont_buf, 0, sizeof(cont_buf));
    psu_stat = read_psu_inout(NULL, psu_name, "pin", cont_buf);
    if(psu_stat != 0xFF){
        int watt = 0;
	psu_stat = search_power_val(cont_buf, &watt);
	if(psu_stat != 0xFF){
            info_p->mpin = watt;
        }
    }

    memset(cont_buf, 0, sizeof(cont_buf));
    psu_stat = read_psu_inout(NULL, psu_name, "pout1", cont_buf);
    if(psu_stat != 0xFF){
        int watt = 0;
        psu_stat = search_power_val(cont_buf, &watt);
	if(psu_stat != 0xFF){
            info_p->mpout = watt;
        }
    }
    memset(cont_buf, 0, sizeof(cont_buf));
    psu_stat = read_psu_inout(NULL, psu_name, "vin", cont_buf);
    if(psu_stat != 0xFF){
        int volt = 0;
        psu_stat = search_voltage_val(cont_buf, &volt);
	 if(psu_stat != 0xFF){
            info_p->mvin = volt;
        }
    }

    memset(cont_buf, 0, sizeof(cont_buf));
    psu_stat = read_psu_inout(NULL, psu_name, "vout1", cont_buf);
     if(psu_stat != 0xFF){
        int volt = 0;
        psu_stat = search_voltage_val(cont_buf, &volt);
         if(psu_stat != 0xFF){
            info_p->mvout = volt;
        }
    }
    //psu_get_model_sn(psu_id,info_p->model,info_p->serial);

    //psu_get_info(psu_id,&(info_p->mvin),&(info_p->mvout),&(info_p->mpin),&(info_p->mpout),&(info_p->miin),&(info_p->miout));

    // if ((info_p->mpin) < 0 || (info_p->mvin)<0)
    //     info_p->status |= ONLP_PSU_STATUS_UNPLUGGED;

    info_p->caps = ONLP_PSU_CAPS_DC12;
    info_p->caps |= ONLP_PSU_CAPS_VIN;
    info_p->caps |= ONLP_PSU_CAPS_VOUT;
    info_p->caps |= ONLP_PSU_CAPS_IIN;
    info_p->caps |= ONLP_PSU_CAPS_IOUT;
    info_p->caps |= ONLP_PSU_CAPS_PIN;
    info_p->caps |= ONLP_PSU_CAPS_POUT;

    return ONLP_STATUS_OK;
}
