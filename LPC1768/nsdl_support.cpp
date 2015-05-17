// NSDL library support functions

#include "mbed.h"
#include "nsdl_support.h"
#include "mbed.h"
#include "rtos.h"
#include "Endpoint.h"
#include "UDPSocket.h"

extern Serial pc;
extern Endpoint nsp;
extern UDPSocket server;
extern char endpoint_name[16];
extern uint8_t ep_type[];
extern uint8_t lifetime_ptr[];

/* The number of seconds between NSP registration messages */
#define RD_UPDATE_PERIOD  60

void *nsdl_alloc(uint16_t size)
{
    return malloc(size);
}

void nsdl_free(void* ptr_to_free)
{
    free(ptr_to_free);
}

/*
 * Create a static resoure
 * Only get is allowed
 */
void nsdl_create_static_resource(sn_nsdl_resource_info_s *resource_structure, uint16_t pt_len, uint8_t *pt, uint16_t rpp_len, uint8_t *rpp_ptr, uint8_t *rsc, uint16_t rsc_len)
{
    resource_structure->access = SN_GRS_GET_ALLOWED;
    resource_structure->mode = SN_GRS_STATIC;
    resource_structure->pathlen = pt_len;
    resource_structure->path = pt;
    resource_structure->resource_parameters_ptr->resource_type_len = rpp_len;
    resource_structure->resource_parameters_ptr->resource_type_ptr = rpp_ptr;
    resource_structure->resource = rsc;
    resource_structure->resourcelen = rsc_len;
    sn_nsdl_create_resource(resource_structure);
}

void nsdl_create_dynamic_resource(sn_nsdl_resource_info_s *resource_structure, uint16_t pt_len, uint8_t *pt, uint16_t rpp_len, uint8_t *rpp_ptr, uint8_t is_observable, sn_grs_dyn_res_callback_t callback_ptr, int access_right)
{
    resource_structure->access = (sn_grs_resource_acl_e)access_right;
    resource_structure->resource = 0;
    resource_structure->resourcelen = 0;
    resource_structure->sn_grs_dyn_res_callback = callback_ptr;
    resource_structure->mode = SN_GRS_DYNAMIC;
    resource_structure->pathlen = pt_len;
    resource_structure->path = pt;
    resource_structure->resource_parameters_ptr->resource_type_len = rpp_len;
    resource_structure->resource_parameters_ptr->resource_type_ptr = rpp_ptr;
    resource_structure->resource_parameters_ptr->observable = is_observable;
    sn_nsdl_create_resource(resource_structure);
}

sn_nsdl_ep_parameters_s* nsdl_init_register_endpoint(sn_nsdl_ep_parameters_s *endpoint_structure, uint8_t* name, uint8_t* typename_ptr, uint8_t *lifetime_ptr)
{
    if (NULL == endpoint_structure)
    {   
        endpoint_structure = (sn_nsdl_ep_parameters_s*)nsdl_alloc(sizeof(sn_nsdl_ep_parameters_s));
    }
    if (endpoint_structure)
    {
        memset(endpoint_structure, 0, sizeof(sn_nsdl_ep_parameters_s));
        endpoint_structure->endpoint_name_ptr = name;
        endpoint_structure->endpoint_name_len = strlen((char*)name);
        endpoint_structure->type_ptr = typename_ptr;
        endpoint_structure->type_len =  strlen((char*)typename_ptr);
        endpoint_structure->lifetime_ptr = lifetime_ptr;
        endpoint_structure->lifetime_len =  strlen((char*)lifetime_ptr);
    }
    return endpoint_structure;
}

void nsdl_clean_register_endpoint(sn_nsdl_ep_parameters_s **endpoint_structure)
{
    if (*endpoint_structure)
    {
        nsdl_free(*endpoint_structure);
        *endpoint_structure = NULL;
    }
}

static uint8_t tx_cb(sn_nsdl_capab_e protocol, uint8_t *data_ptr, uint16_t data_len, sn_nsdl_addr_s *address_ptr)
{
    pc.printf("TX callback!\n\rSending %d bytes\r\n", data_len);

    if(server.sendTo(nsp, (char*)data_ptr, data_len) != data_len)
        pc.printf("sending failed\n\r");

    return 1;
}

static uint8_t rx_cb(sn_coap_hdr_s *coap_packet_ptr, sn_nsdl_addr_s *address_ptr)
{
    pc.printf("RX callback!\r\n");
    return 0;
}

static void registration_update_thread(void const *args)
{
    sn_nsdl_ep_parameters_s *endpoint_ptr = NULL;

    while(true)
    {
        wait(RD_UPDATE_PERIOD);
        endpoint_ptr = nsdl_init_register_endpoint(endpoint_ptr, (uint8_t*)endpoint_name, ep_type, lifetime_ptr);
        if(sn_nsdl_register_endpoint(endpoint_ptr) != 0)
            pc.printf("NSP re-registering failed\r\n");
        else
            pc.printf("NSP re-registering OK\r\n");
        nsdl_clean_register_endpoint(&endpoint_ptr);
    }
}

void nsdl_init()
{
    uint8_t nsp_addr[4];
    sn_nsdl_mem_s memory_cbs;

    /* Initialize libNsdl */
    memory_cbs.sn_nsdl_alloc = &nsdl_alloc;
    memory_cbs.sn_nsdl_free = &nsdl_free;
    if(sn_nsdl_init(&tx_cb, &rx_cb, &memory_cbs) == -1)
        pc.printf("libNsdl init failed\r\n");
    else
        pc.printf("libNsdl init done\r\n");

    /* Set nsp address for library */
    set_NSP_address(nsp_addr, 5683, SN_NSDL_ADDRESS_TYPE_IPV4);
}

void nsdl_event_loop()
{
    Thread registration_thread(registration_update_thread);
    
    sn_nsdl_addr_s received_packet_address;
    uint8_t received_address[4];
    char buffer[1024];
    Endpoint from;

    memset(&received_packet_address, 0, sizeof(sn_nsdl_addr_s));
    received_packet_address.addr_ptr = received_address;

    while(1)
    {
        int n = server.receiveFrom(from, buffer, sizeof(buffer));
        if (n < 0)
        {
            pc.printf("Socket error\n\r");
        }
        else
        {   
            pc.printf("Received %d bytes\r\n", n);
            sn_nsdl_process_coap((uint8_t*)buffer, n, &received_packet_address);
        }
    }
}

            