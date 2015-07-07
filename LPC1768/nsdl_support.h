// Support functions for the NSDL library

#ifndef NSDL_SUPPORT_H
#define NSDL_SUPPORT_H

#include "mbed.h"
#include <stdint.h>
#include "sn_nsdl.h"
#include "sn_coap_header.h"
#include "sn_coap_protocol.h"
#include "sn_nsdl_lib.h"

typedef uint8_t (*sn_grs_dyn_res_callback_t)(sn_coap_hdr_s *, sn_nsdl_addr_s *, sn_proto_info_s *);

extern "C" void *nsdl_alloc(uint16_t size);
extern "C" void nsdl_free(void* ptr_to_free);
void nsdl_create_static_resource(sn_nsdl_resource_info_s *resource_structure, uint16_t pt_len, uint8_t *pt, uint16_t rpp_len, uint8_t *rpp_ptr, uint8_t *rsc, uint16_t rsc_len);
void nsdl_create_dynamic_resource(sn_nsdl_resource_info_s *resource_structure, uint16_t pt_len, uint8_t *pt, uint16_t rpp_len, uint8_t *rpp_ptr, uint8_t is_observable, sn_grs_dyn_res_callback_t callback_ptr, int access_right);
sn_nsdl_ep_parameters_s* nsdl_init_register_endpoint(sn_nsdl_ep_parameters_s *endpoint_structure, uint8_t* name, uint8_t* ypename_ptr, uint8_t *lifetime_ptr);
void nsdl_clean_register_endpoint(sn_nsdl_ep_parameters_s **endpoint_structure);
void nsdl_init();
void nsdl_event_loop();

#endif // NSDL_SUPPORT_H
