/*
 * Copyright
 */
#include <unistd.h>
#include <fcntl.h>

#include <onlplib/file.h>
#include <onlp/platformi/thermali.h>
#include <onlp/platformi/fani.h>
#include <onlp/platformi/sysi.h>
#include <x86_64_cel_ivystone/x86_64_cel_ivystone_config.h>

#include "x86_64_cel_ivystone_int.h"
#include "x86_64_cel_ivystone_log.h"
#include "platform.h"
#include <sys/stat.h>
#include <time.h>

static int is_cache_exist(){
    const char *path="/tmp/onlp-sensor-cache.txt";
    const char *time_setting_path="/var/opt/interval_time.txt";
    time_t current_time;
    double diff_time;
    int interval_time = 30; //set default to 30 sec
    struct stat fst;
    bzero(&fst,sizeof(fst));

    //Read setting
    if(access(time_setting_path, F_OK) == -1){ //Setting not exist
        return -1;
    }else{
        FILE *fp;
        
        fp = fopen(time_setting_path, "r"); // read setting
        
        if (fp == NULL)
        {
            perror("Error while opening the file.\n");
            exit(EXIT_FAILURE);
        }

        fscanf(fp,"%d", &interval_time);

        fclose(fp);
    }

    if (access(path, F_OK) == -1){ //Cache not exist
        return -1;
    }else{ //Cache exist
        current_time = time(NULL);
        if (stat(path,&fst) != 0) { printf("stat() failed"); exit(-1); }

        diff_time = difftime(current_time,fst.st_mtime);

        if(diff_time > interval_time){
            return -1;
        }
        return 1;
    }
}

static int create_cache(){
    //curl -g http://240.1.1.1:8080/api/sys/fruid/status |python -m json.tool Present status PSU,FAN
    //curl -g http://240.1.1.1:8080/api/sys/fruid/psu |python -m json.tool PSU FRU data
    //curl -g http://240.1.1.1:8080/api/sys/fruid/fan |python -m json.tool FAN FRU data
    //curl -g http://240.1.1.1:8080/api/sys/fruid/sys |python -m json.tool Board FRU
    system("curl -g http://240.1.1.1:8080/api/sys/sensors |python -m json.tool > /tmp/onlp-sensor-cache.txt");
    system("curl -g http://240.1.1.1:8080/api/sys/fruid/psu | python -m json.tool > /tmp/onlp-psu-fru-cache.txt");
    system("curl -g http://240.1.1.1:8080/api/sys/fruid/fan | python -m json.tool > /tmp/onlp-fan-fru-cache.txt");
    system("curl -g http://240.1.1.1:8080/api/sys/fruid/sys | python -m json.tool > /tmp/onlp-sys-fru-cache.txt");
    system("curl -g http://240.1.1.1:8080/api/sys/fruid/status | python -m json.tool > /tmp/onlp-status-fru-cache.txt");
    return 1;
}

const char*
onlp_sysi_platform_get(void)
{
    return "x86-64-cel-ivystone-r0";
}

int
onlp_sysi_init(void)
{
    return ONLP_STATUS_OK;
}

int
onlp_sysi_onie_data_get(uint8_t** data, int* size)
{
    uint8_t* rdata = aim_zmalloc(256);
    if(onlp_file_read(rdata, 256, size, PREFIX_PATH_ON_SYS_EEPROM) == ONLP_STATUS_OK) {
        if(*size == 256) {
            *data = rdata;
            return ONLP_STATUS_OK;
        }
    }
    aim_free(rdata);
    rdata = NULL;
    DEBUG_PRINT("[Debug][%s][%d][Can't get onie data]\n", __FUNCTION__, __LINE__);
    return ONLP_STATUS_E_INTERNAL;
}

void
onlp_sysi_onie_data_free(uint8_t* data)
{
    aim_free(data);
}

int onlp_sysi_platform_manage_init(void)
{
    //printf("Check the sequence from onlp_sysi_platform_manage_init\n");
    if(is_cache_exist()<1){
        create_cache();
    }
    return ONLP_STATUS_OK;
}

int onlp_sysi_platform_manage_fans(void){
    //printf("Check the sequence from onlp_sysi_platform_manage_fans\n");
    if(is_cache_exist()<1){
        create_cache();
    }
    return ONLP_STATUS_OK;
}

int onlp_sysi_platform_manage_leds(void){
    //printf("Check the sequence from onlp_sysi_platform_manage_leds\n");
    if(is_cache_exist()<1){
        create_cache();
    }
    return ONLP_STATUS_OK;
}

int
onlp_sysi_oids_get(onlp_oid_t* table, int max)
{
    int i;
    onlp_oid_t* e = table;

    memset(table, 0, max*sizeof(onlp_oid_t));

    /* 4 PSUs */
    for (i=1; i<=PSU_COUNT; i++)
        *e++ = ONLP_PSU_ID_CREATE(i);

    // // /* LEDs Item */
    // for (i=1; i<=LED_COUNT; i++)
    //     *e++ = ONLP_LED_ID_CREATE(i);

    // // /* THERMALs Item */
    // for (i=1; i<=THERMAL_COUNT; i++)
    //     *e++ = ONLP_THERMAL_ID_CREATE(i);

    /* Fans Item */
    for (i=1; i<=FAN_COUNT; i++)
        *e++ = ONLP_FAN_ID_CREATE(i);

    return ONLP_STATUS_OK;
}
