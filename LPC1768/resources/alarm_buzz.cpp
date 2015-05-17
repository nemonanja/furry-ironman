// GPS resource implementation

#include "mbed.h"
#include "nsdl_support.h"
#include "alarm_buzz.h"
#include "Beep.h"

#define ALARM_BUZZ_RES_ID    "alarm_buzz/on"

extern Serial pc;
static Beep buzzer(p26);

/* Only GET and PUT method allowed */
static uint8_t alarm_buzz_resource_cb(sn_coap_hdr_s *received_coap_ptr, sn_nsdl_addr_s *address, sn_proto_info_s * proto)
{
    sn_coap_hdr_s *coap_res_ptr = 0;
    static uint8_t alarm_buzz_state = '0';

    if(received_coap_ptr->msg_code == COAP_MSG_CODE_REQUEST_GET)
    {
        coap_res_ptr = sn_coap_build_response(received_coap_ptr, COAP_MSG_CODE_RESPONSE_CONTENT);

        coap_res_ptr->payload_len = 1;
        coap_res_ptr->payload_ptr = &alarm_buzz_state;
        sn_nsdl_send_coap_message(address, coap_res_ptr);
        coap_res_ptr->payload_ptr = &alarm_buzz_state;
        sn_nsdl_send_coap_message(address, coap_res_ptr);
                
    }
    else if(received_coap_ptr->msg_code == COAP_MSG_CODE_REQUEST_PUT)
    {
        if(received_coap_ptr->payload_len)
        {
            if(*(received_coap_ptr->payload_ptr) == '1')
            {
                buzzer.beep(700,5);
                alarm_buzz_state = '1';
                
            }
            else if(*(received_coap_ptr->payload_ptr) == '0')
            {
                buzzer.nobeep();
                alarm_buzz_state = '0';
            }
            coap_res_ptr = sn_coap_build_response(received_coap_ptr, COAP_MSG_CODE_RESPONSE_CHANGED);
            sn_nsdl_send_coap_message(address, coap_res_ptr);
        }
    }

    sn_coap_parser_release_allocated_coap_msg_mem(coap_res_ptr);

    return 0;
}

int create_alarm_buzz_resource(sn_nsdl_resource_info_s *resource_ptr)
{
    nsdl_create_dynamic_resource(resource_ptr, sizeof(ALARM_BUZZ_RES_ID)-1, (uint8_t*)ALARM_BUZZ_RES_ID, 0, 0, 0, &alarm_buzz_resource_cb, (SN_GRS_GET_ALLOWED | SN_GRS_PUT_ALLOWED));
    return 0;
}