#include "mbed.h"
#include "WiflyInterface.h"
#include "nsdl_support.h"
#include "temperature.h"
#include "alarm_led.h"
#include "alarm_buzz.h"
#include "weight.h"
#include "C12832_lcd.h"
/*
*Main program for the SW project 2015 IoT coffee monitor application. 
*HW is LPC1768 with mbed application board, a IR thermometer mlx90614 and an analog input mass measurement circuit. 
*Heavily influenced by the NSDL_HelloWorld_WiFi program
*-Joonas
*/

// WiFi configuration (edit this)

#define WIFI_SSID               "OTiT"
#define WIFI_SECURITY           WPA    // NONE, WEP_128 or WPA
#define WIFI_PASS               "oh8taoh8ta"

//DHCP definition
#define DHCP                    1

//Operating mode definitions
#define ALARM                   1
#define MONITOR                 0

#if MONITOR == 1 & ALARM == 0
    char endpoint_name[16] = "monitor-";

#elif ALARM == 1 && MONITOR == 0
    char endpoint_name[16] = "alarm-";
#else
    char endpoint_name[16] = "multifunc-";
#endif


/* Manual IP configurations, if DHCP not defined */
#define IP      "000.000.000.000"
#define MASK    "255.255.255.0"
#define GW      "000.000.000.000"

// NSP configuration
/* Change this IP address to that of your NanoService Platform installation */
static const char* NSP_ADDRESS = "teemupa.dy.fi";
static const int NSP_PORT = 5683;

uint8_t ep_type[] = {"IoT-coffee"};

uint8_t lifetime_ptr[] = {"1200"};


/* wifly interface:
*     - p9 and p10 are for the serial communication
*     - p30 is for the reset pin
*     - p29 is for the connection status
*     - "mbed" is the ssid of the network
*     - "password" is the password
*     - WPA is the security
*/
static WiflyInterface wifly(p9, p10, p30, p29, WIFI_SSID, WIFI_PASS, WIFI_SECURITY);
static C12832_LCD lcd;
Serial pc(USBTX, USBRX); // tx, rx


// ****************************************************************************
// WiFi initialization


static void wifi_init()
{
    char mbed_uid[33]; // for creating unique name for the board

    /* Initialize network */
#if DHCP == 1
    //NSDL_DEBUG("DHCP in use\r\n");
    wifly.init();
#else
    wifly.init(IP, MASK, GW);
#endif
    if(wifly.connect() == 0)
        pc.printf("Connect OK\n\r");

    mbed_interface_uid(mbed_uid);
    mbed_uid[32] = '\0';
    strncat(endpoint_name, mbed_uid + 27, 15 - strlen(endpoint_name));

    lcd.locate(0,11);
    lcd.printf("IP:%s", wifly.getIPAddress());

    //NSDL_DEBUG("IP Address:%s ", wifly.getIPAddress());
}

// ****************************************************************************
// ****************************************************************************
// NSP initialization

UDPSocket server;
Endpoint nsp;

static void nsp_init()
{
    server.init();
    server.bind(NSP_PORT);

    nsp.set_address(NSP_ADDRESS, NSP_PORT);
    
    //NSDL_DEBUG("name: %s", endpoint_name);
    //NSDL_DEBUG("NSP=%s - port %d\n", NSP_ADDRESS, NSP_PORT);

    lcd.locate(0,22);
    lcd.printf("EP name:%s\n", endpoint_name);
}

// ****************************************************************************
// ****************************************************************************
// Resource creation

static int create_resources()
{
    sn_nsdl_resource_info_s *resource_ptr = NULL;
    sn_nsdl_ep_parameters_s *endpoint_ptr = NULL;
    
    //NSDL_DEBUG("Creating resources");

    /* Create resources */
    resource_ptr = (sn_nsdl_resource_info_s*)nsdl_alloc(sizeof(sn_nsdl_resource_info_s));
    if(!resource_ptr)
        return 0;
    memset(resource_ptr, 0, sizeof(sn_nsdl_resource_info_s));

    resource_ptr->resource_parameters_ptr = (sn_nsdl_resource_parameters_s*)nsdl_alloc(sizeof(sn_nsdl_resource_parameters_s));
    if(!resource_ptr->resource_parameters_ptr)
    {
        nsdl_free(resource_ptr);
        return 0;
    }
    memset(resource_ptr->resource_parameters_ptr, 0, sizeof(sn_nsdl_resource_parameters_s));

    //Static resources
    nsdl_create_static_resource(resource_ptr, sizeof("dev/mfg")-1, (uint8_t*) "dev/mfg", 0, 0,  (uint8_t*) "Team Purple", sizeof("Team Purple")-1);
    nsdl_create_static_resource(resource_ptr, sizeof("dev/mdl")-1, (uint8_t*) "dev/mdl", 0, 0,  (uint8_t*) "Coffee monitoring system", sizeof("Coffee monitoring system")-1);
    
    //Create dynamic resources

    #if MONITOR == 1
        create_temperature_resource(resource_ptr);
        create_weight_resource(resource_ptr);
    #endif
    #if ALARM == 1 
        create_alarm_led_resource(resource_ptr);
        create_alarm_buzz_resource(resource_ptr);
    #endif
    

        /* Register with NSP */
    /*char domain_name[7] = "coffee";
    uint8_t * domain = (uint8_t*)malloc(8);
    memset(domain, 0, 8);
    memcpy(0, domain_name, 8);

    endpoint_ptr->domain_name_ptr = (uint8_t *)domain_name;
    endpoint_ptr->domain_name_len = sizeof(domain_name);*/
    endpoint_ptr = nsdl_init_register_endpoint(endpoint_ptr, (uint8_t*)endpoint_name, ep_type, lifetime_ptr);
    if(sn_nsdl_register_endpoint(endpoint_ptr) != 0)
        pc.printf("NSP registering failed\r\n");
    else
        pc.printf("NSP registering OK\r\n");
    nsdl_clean_register_endpoint(&endpoint_ptr);

    nsdl_free(resource_ptr->resource_parameters_ptr);
    nsdl_free(resource_ptr);
    //free(domain);
    return 1;
}

// ****************************************************************************
// ****************************************************************************
// Program entry point

int main()
{
    lcd.cls();
    lcd.locate(0,0);
    lcd.printf("mbed coffee monitor");
    //NSDL_DEBUG("mbed NanoService Example App 0.1\n");
    
    // Initialize WiFi interface first
    wifi_init();
    
    // Initialize NSP node
    nsp_init();
    
    // Initialize NSDL stack
    nsdl_init();
    
    // Create NSDL resources
    create_resources();
    
    // Run the NSDL event loop (never returns)
    nsdl_event_loop();
}