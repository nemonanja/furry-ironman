#include "mbed.h"
#include "rtos.h"
#include "mlx90614.h"
#include "nsdl_support.h"
#include "temperature.h"

#define TEMP_RES_ID     "sen/temp"



I2C i2c(p28,p27);   //sda,scl
MLX90614 therm(&i2c);
DigitalOut waitLed(LED1); //displays I2C wait
static char temp_val[5];
float temp;



static uint8_t temp_resource_cb(sn_coap_hdr_s *received_coap_ptr, sn_nsdl_addr_s *address, sn_proto_info_s * proto){
    sn_coap_hdr_s *coap_res_ptr = 0;
    waitLed = 1;
    while (1) {
        
        waitLed=1; // if led1 on - waiting on I2C
        if (therm.getTemp(&temp)) {
            //gets temperature from sensor via I2C bus
            waitLed=0;
            break;
            //print temperature on PC
        }
    }
    coap_res_ptr = sn_coap_build_response(received_coap_ptr, COAP_MSG_CODE_RESPONSE_CONTENT);
        
    sprintf(temp_val, "%2.2f", temp);
    coap_res_ptr->payload_len = 5;
    coap_res_ptr->payload_ptr = (uint8_t *)temp_val;
    sn_nsdl_send_coap_message(address, coap_res_ptr);


    sn_coap_parser_release_allocated_coap_msg_mem(coap_res_ptr);
    return 0;

}


int create_temperature_resource(sn_nsdl_resource_info_s *resource_ptr){
    
    nsdl_create_dynamic_resource(resource_ptr, sizeof(TEMP_RES_ID)-1, (uint8_t*)TEMP_RES_ID, 0, 0, 0, &temp_resource_cb, SN_GRS_GET_ALLOWED);
    return 0;
}




