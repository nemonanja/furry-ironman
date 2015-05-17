#include "mbed.h"
#include "rtos.h"
#include "nsdl_support.h"
#include "weight.h"

#define RES_ID     "sen/weight"


static char temp_val[5];
float resistors;
AnalogIn sensors(p15);



static uint8_t weight_resource_cb(sn_coap_hdr_s *received_coap_ptr, sn_nsdl_addr_s *address, sn_proto_info_s * proto){
    sn_coap_hdr_s *coap_res_ptr = 0;
    resistors = 0;
    for(int i = 0; i < 500; i++){
        resistors = resistors + sensors.read();
    }
    resistors = resistors / 500; 
    coap_res_ptr = sn_coap_build_response(received_coap_ptr, COAP_MSG_CODE_RESPONSE_CONTENT);
        
    sprintf(temp_val, "%1.3f", resistors);
    coap_res_ptr->payload_len = 5;
    coap_res_ptr->payload_ptr = (uint8_t *)temp_val;
    sn_nsdl_send_coap_message(address, coap_res_ptr);


    sn_coap_parser_release_allocated_coap_msg_mem(coap_res_ptr);
    return 0;

}


int create_weight_resource(sn_nsdl_resource_info_s *resource_ptr){
    
    nsdl_create_dynamic_resource(resource_ptr, sizeof(RES_ID)-1, (uint8_t*)RES_ID, 0, 0, 0, &weight_resource_cb, SN_GRS_GET_ALLOWED);
    return 0;
}