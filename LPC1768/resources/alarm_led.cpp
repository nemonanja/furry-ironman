// GPS resource implementation

#include "mbed.h"
#include "nsdl_support.h"
#include "alarm_led.h"
#include "C12832_lcd.h"

#define ALARM_LED_RES_ID    "alarm_led/on"

extern Serial pc;
static C12832_LCD lcd;

/* Only GET and PUT method allowed */
static uint8_t alarm_led_resource_cb(sn_coap_hdr_s *received_coap_ptr, sn_nsdl_addr_s *address, sn_proto_info_s * proto)
{
    sn_coap_hdr_s *coap_res_ptr = 0;
    static uint8_t alarm_led_state = '0';
    PwmOut r (p23);
    pc.printf("relay callback\r\n");

    if(received_coap_ptr->msg_code == COAP_MSG_CODE_REQUEST_GET)
    {
        coap_res_ptr = sn_coap_build_response(received_coap_ptr, COAP_MSG_CODE_RESPONSE_CONTENT);

        coap_res_ptr->payload_len = 1;
        coap_res_ptr->payload_ptr = &alarm_led_state;
        sn_nsdl_send_coap_message(address, coap_res_ptr);
        
        coap_res_ptr->payload_ptr = &alarm_led_state;
        sn_nsdl_send_coap_message(address, coap_res_ptr);
                
    }
    else if(received_coap_ptr->msg_code == COAP_MSG_CODE_REQUEST_PUT)
    {
        if(received_coap_ptr->payload_len)
        {
            if(*(received_coap_ptr->payload_ptr) == '1')
            {
                alarm_led_state = '1';
                /*lcd.locate(0,11);
                lcd.printf("OUT OF COFFEE!");*/
                r.write(0.0);
                
            }
            else if(*(received_coap_ptr->payload_ptr) == '0')
            {
                alarm_led_state = '0';
                //lcd.cls();
                r.write(1.0);
            }
            coap_res_ptr = sn_coap_build_response(received_coap_ptr, COAP_MSG_CODE_RESPONSE_CHANGED);
            sn_nsdl_send_coap_message(address, coap_res_ptr);
        }
    }

    sn_coap_parser_release_allocated_coap_msg_mem(coap_res_ptr);

    return 0;
}

int create_alarm_led_resource(sn_nsdl_resource_info_s *resource_ptr)
{
    nsdl_create_dynamic_resource(resource_ptr, sizeof(ALARM_LED_RES_ID)-1, (uint8_t*)ALARM_LED_RES_ID, 0, 0, 0, &alarm_led_resource_cb, (SN_GRS_GET_ALLOWED | SN_GRS_PUT_ALLOWED));
    return 0;
}