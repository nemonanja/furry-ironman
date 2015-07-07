#include "WiflyInterface.h"

WiflyInterface::WiflyInterface( PinName tx, PinName rx, PinName reset, PinName tcp_status,
                                const char * ssid, const char * phrase, Security sec) :
    Wifly(tx, rx, reset, tcp_status, ssid, phrase, sec)
{
    ip_set = false;
}

int WiflyInterface::init()
{
    state.dhcp = true;
    reset();
    return 0;
}

int WiflyInterface::init(const char* ip, const char* mask, const char* gateway)
{
    state.dhcp = false;
    this->ip = ip;
    strcpy(ip_string, ip);
    ip_set = true;
    this->netmask = mask;
    this->gateway = gateway;
    reset();

    return 0;
}

int WiflyInterface::connect()
{
    return join();
}

int WiflyInterface::disconnect()
{
    return Wifly::disconnect();
}

char * WiflyInterface::getIPAddress()
{
    char * match = 0;
    if (!ip_set) {
        if (!sendCommand("get ip a\r", NULL, ip_string))
            return NULL;
        exit();
        flush();
        match = strstr(ip_string, "<");
        if (match != NULL) {
            *match = '\0';
        }
        if (strlen(ip_string) < 6) {
            match = strstr(ip_string, ">");
            if (match != NULL) {
                int len = strlen(match + 1);
                memcpy(ip_string, match + 1, len);
            }
        }
        ip_set = true;
    }
    return ip_string;
}