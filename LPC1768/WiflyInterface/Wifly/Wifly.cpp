/* Copyright (C) 2012 mbed.org, MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "mbed.h"
#include "Wifly.h"
#include <string>
#include <algorithm>

//Debug is disabled by default
#if (0 && !defined(TARGET_LPC11U24))
#define DBG(x, ...) std::printf("[Wifly : DBG]"x"\r\n", ##__VA_ARGS__);
#define WARN(x, ...) std::printf("[Wifly : WARN]"x"\r\n", ##__VA_ARGS__);
#define ERR(x, ...) std::printf("[Wifly : ERR]"x"\r\n", ##__VA_ARGS__);
#else
#define DBG(x, ...)
#define WARN(x, ...)
#define ERR(x, ...)
#endif

#if !defined(TARGET_LPC11U24)
#define INFO(x, ...) printf("[Wifly : INFO]"x"\r\n", ##__VA_ARGS__);
#else
#define INFO(x, ...)
#endif

#define MAX_TRY_JOIN 3

Wifly * Wifly::inst;

Wifly::Wifly(   PinName tx, PinName rx, PinName _reset, PinName tcp_status, const char * ssid, const char * phrase, Security sec):
    wifi(tx, rx), reset_pin(_reset), tcp_status(tcp_status), buf_wifly(256)
{
    memset(&state, 0, sizeof(state));
    state.sec = sec;

    // change all ' ' in '$' in the ssid and the passphrase
    strcpy(this->ssid, ssid);
    for (int i = 0; i < strlen(ssid); i++) {
        if (this->ssid[i] == ' ')
            this->ssid[i] = '$';
    }
    strcpy(this->phrase, phrase);
    for (int i = 0; i < strlen(phrase); i++) {
        if (this->phrase[i] == ' ')
            this->phrase[i] = '$';
    }

    inst = this;
    attach_rx(false);
    state.cmd_mode = false;
}

bool Wifly::join()
{
    char cmd[75];

    for (int i= 0; i < MAX_TRY_JOIN; i++) {

        // no auto join
        if (!sendCommand("set w j 0\r", "AOK"))
            continue;

        //no echo
        if (!sendCommand("set u m 1\r", "AOK"))
            continue;

        // set time
        if (!sendCommand("set c t 30\r", "AOK"))
            continue;

        // set size
        if (!sendCommand("set c s 1024\r", "AOK"))
            continue;

        // red led on when tcp connection active
        if (!sendCommand("set s i 0x40\r", "AOK"))
            continue;

        // no string sent to the tcp client
        if (!sendCommand("set c r 0\r", "AOK"))
            continue;

        // tcp protocol
        if (!sendCommand("set i p 2\r", "AOK"))
            continue;

        // tcp retry
        if (!sendCommand("set i f 0x7\r", "AOK"))
            continue;
            
        // set dns server
        if (!sendCommand("set d n rn.microchip.com\r", "AOK"))
            continue;

        //dhcp
        sprintf(cmd, "set i d %d\r", (state.dhcp) ? 1 : 0);
        if (!sendCommand(cmd, "AOK"))
            continue;

        // ssid
        sprintf(cmd, "set w s %s\r", ssid);
        if (!sendCommand(cmd, "AOK"))
            continue;

        //auth
        sprintf(cmd, "set w a %d\r", state.sec);
        if (!sendCommand(cmd, "AOK"))
            continue;

        // if no dhcp, set ip, netmask and gateway
        if (!state.dhcp) {
            DBG("not dhcp\r");

            sprintf(cmd, "set i a %s\r\n", ip);
            if (!sendCommand(cmd, "AOK"))
                continue;

            sprintf(cmd, "set i n %s\r", netmask);
            if (!sendCommand(cmd, "AOK"))
                continue;

            sprintf(cmd, "set i g %s\r", gateway);
            if (!sendCommand(cmd, "AOK"))
                continue;
        }

        //key step
        if (state.sec != NONE) {
            if (state.sec == WPA)
                sprintf(cmd, "set w p %s\r", phrase);
            else if (state.sec == WEP_128)
                sprintf(cmd, "set w k %s\r", phrase);

            if (!sendCommand(cmd, "AOK"))
                continue;
        }

        //join the network (10s timeout)
        if (state.dhcp) {
            if (!sendCommand("join\r", "DHCP=ON", NULL, 10000))
                continue;
        } else {
            if (!sendCommand("join\r", "Associated", NULL, 10000))
                continue;
        }

        if (!sendCommand("save\r", "Stor"))
            continue;

        exit();

        state.associated = true;
        INFO("\r\nssid: %s\r\nphrase: %s\r\nsecurity: %s\r\n\r\n", this->ssid, this->phrase, getStringSecurity());
        return true;
    }
    return false;
}


bool Wifly::setProtocol(Protocol p)
{
    // use udp auto pairing
    char cmd[20];
    sprintf(cmd, "set i p %d\r", p);
    if (!sendCommand(cmd, "AOK"))
        return false;

    switch(p) {
        case TCP:
            // set ip flags: tcp retry enabled
            if (!sendCommand("set i f 0x07\r", "AOK"))
                return false;
            break;
        case UDP:
            // set ip flags: udp auto pairing enabled
            if (!sendCommand("set i h 0.0.0.0\r", "AOK"))
                return false;
            if (!sendCommand("set i f 0x40\r", "AOK"))
                return false;
            break;
    }
    state.proto = p;
    return true;
}

char * Wifly::getStringSecurity()
{
    switch(state.sec) {
        case NONE:
            return "NONE";
        case WEP_128:
            return "WEP_128";
        case WPA:
            return "WPA";
    }
    return "UNKNOWN";
}

bool Wifly::connect(const char * host, int port)
{
    char rcv[20];
    char cmd[20];

    // try to open
    sprintf(cmd, "open %s %d\r", host, port);
    if (sendCommand(cmd, "OPEN", NULL, 10000)) {
        state.tcp = true;
        state.cmd_mode = false;
        return true;
    }

    // if failed, retry and parse the response
    if (sendCommand(cmd, NULL, rcv, 5000)) {
        if (strstr(rcv, "OPEN") == NULL) {
            if (strstr(rcv, "Connected") != NULL) {
                wait(0.25);
                if (!sendCommand("close\r", "CLOS"))
                    return false;
                wait(0.25);
                if (!sendCommand(cmd, "OPEN", NULL, 10000))
                    return false;
            } else {
                return false;
            }
        }
    } else {
        return false;
    }

    state.tcp = true;
    state.cmd_mode = false;

    return true;
}


bool Wifly::gethostbyname(const char * host, char * ip)
{
    string h = host;
    char cmd[30], rcv[100];
    int l = 0;
    char * point;
    int nb_digits = 0;

    // no dns needed
    int pos = h.find(".");
    if (pos != string::npos) {
        string sub = h.substr(0, h.find("."));
        nb_digits = atoi(sub.c_str());
    }
    //printf("substrL %s\r\n", sub.c_str());
    if (count(h.begin(), h.end(), '.') == 3 && nb_digits > 0) {
        strcpy(ip, host);
    }
    // dns needed
    else {
        nb_digits = 0;
        sprintf(cmd, "lookup %s\r", host);
        if (!sendCommand(cmd, NULL, rcv))
            return false;

        // look for the ip address
        char * begin = strstr(rcv, "=") + 1;
        for (int i = 0; i < 3; i++) {
            point = strstr(begin + l, ".");
            DBG("str: %s", begin + l);
            l += point - (begin + l) + 1;
        }
        DBG("str: %s", begin + l);
        while(*(begin + l + nb_digits) >= '0' && *(begin + l + nb_digits) <= '9') {
            DBG("digit: %c", *(begin + l + nb_digits));
            nb_digits++;
        }
        memcpy(ip, begin, l + nb_digits);
        ip[l+nb_digits] = 0;
        DBG("ip from dns: %s", ip);
    }
    return true;
}


void Wifly::flush()
{
    buf_wifly.flush();
}

bool Wifly::sendCommand(const char * cmd, const char * ack, char * res, int timeout)
{
    if (!state.cmd_mode) {
        cmdMode();
    }
    if (send(cmd, strlen(cmd), ack, res, timeout) == -1) {
        ERR("sendCommand: cannot %s\r\n", cmd);
        exit();
        return false;
    }
    return true;
}

bool Wifly::cmdMode()
{
    // if already in cmd mode, return
    if (state.cmd_mode)
        return true;

    if (send("$$$", 3, "CMD") == -1) {
        ERR("cannot enter in cmd mode\r\n");
        exit();
        return false;
    }
    state.cmd_mode = true;
    return true;
}

bool Wifly::disconnect()
{
    // if already disconnected, return
    if (!state.associated)
        return true;

    if (!sendCommand("leave\r", "DeAuth"))
        return false;
    exit();

    state.associated = false;
    return true;

}

bool Wifly::is_connected()
{
    return (tcp_status.read() ==  1) ? true : false;
}


void Wifly::reset()
{
    reset_pin = 0;
    wait(0.2);
    reset_pin = 1;
    wait(0.2);
}

bool Wifly::reboot()
{
    // if already in cmd mode, return
    if (!sendCommand("reboot\r"))
        return false;
    
    wait(0.3);

    state.cmd_mode = false;
    return true;
}

bool Wifly::close()
{
    // if not connected, return
    if (!state.tcp)
        return true;

    wait(0.25);
    if (!sendCommand("close\r", "CLOS"))
        return false;
    exit();

    state.tcp = false;
    return true;
}


int Wifly::putc(char c)
{
    while (!wifi.writeable());
    return wifi.putc(c);
}


bool Wifly::exit()
{
    flush();
    if (!state.cmd_mode)
        return true;
    if (!sendCommand("exit\r", "EXIT"))
        return false;
    state.cmd_mode = false;
    flush();
    return true;
}


int Wifly::readable()
{
    return buf_wifly.available();
}

int Wifly::writeable()
{
    return wifi.writeable();
}

char Wifly::getc()
{
    char c;
    while (!buf_wifly.available());
    buf_wifly.dequeue(&c);
    return c;
}

void Wifly::handler_rx(void)
{
    //read characters
    while (wifi.readable())
        buf_wifly.queue(wifi.getc());
}

void Wifly::attach_rx(bool callback)
{
    if (!callback)
        wifi.attach(NULL);
    else
        wifi.attach(this, &Wifly::handler_rx);
}


int Wifly::send(const char * str, int len, const char * ACK, char * res, int timeout)
{
    char read;
    size_t found = string::npos;
    string checking;
    Timer tmr;
    int result = 0;

    DBG("will send: %s\r\n",str);

    attach_rx(false);

    //We flush the buffer
    while (wifi.readable())
        wifi.getc();

    if (!ACK || !strcmp(ACK, "NO")) {
        for (int i = 0; i < len; i++)
            result = (putc(str[i]) == str[i]) ? result + 1 : result;
    } else {
        //We flush the buffer
        while (wifi.readable())
            wifi.getc();

        tmr.start();
        for (int i = 0; i < len; i++)
            result = (putc(str[i]) == str[i]) ? result + 1 : result;

        while (1) {
            if (tmr.read_ms() > timeout) {
                //We flush the buffer
                while (wifi.readable())
                    wifi.getc();

                DBG("check: %s\r\n", checking.c_str());

                attach_rx(true);
                return -1;
            } else if (wifi.readable()) {
                read = wifi.getc();
                if ( read != '\r' && read != '\n') {
                    checking += read;
                    found = checking.find(ACK);
                    if (found != string::npos) {
                        wait(0.01);

                        //We flush the buffer
                        while (wifi.readable())
                            wifi.getc();

                        break;
                    }
                }
            }
        }
        DBG("check: %s\r\n", checking.c_str());

        attach_rx(true);
        return result;
    }

    //the user wants the result from the command (ACK == NULL, res != NULL)
    if ( res != NULL) {
        int i = 0;
        Timer timeout;
        timeout.start();
        tmr.reset();
        while (1) {
            if (timeout.read() > 2) {
                if (i == 0) {
                    res = NULL;
                    break;
                }
                res[i] = '\0';
                DBG("user str 1: %s\r\n", res);

                break;
            } else {
                if (tmr.read_ms() > 300) {
                    res[i] = '\0';
                    DBG("user str: %s\r\n", res);

                    break;
                }
                if (wifi.readable()) {
                    tmr.start();
                    read = wifi.getc();

                    // we drop \r and \n
                    if ( read != '\r' && read != '\n') {
                        res[i++] = read;
                    }
                }
            }
        }
        DBG("user str: %s\r\n", res);
    }

    //We flush the buffer
    while (wifi.readable())
        wifi.getc();

    attach_rx(true);
    DBG("result: %d\r\n", result)
    return result;
}
