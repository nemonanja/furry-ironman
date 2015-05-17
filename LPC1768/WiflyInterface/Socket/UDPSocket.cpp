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

#include "UDPSocket.h"

#include <string>
#include <algorithm>

UDPSocket::UDPSocket()
{
    endpoint_configured = false;
    endpoint_read = false;
}

int UDPSocket::init(void)
{
    wifi->setProtocol(UDP);
    wifi->exit();
    return 0;
}

// Server initialization
int UDPSocket::bind(int port)
{
    char cmd[17];
    
    // set local port
    sprintf(cmd, "set i l %d\r", port);
    if (!wifi->sendCommand(cmd, "AOK"))
        return -1;
        
    // save
    if (!wifi->sendCommand("save\r", "Stor"))
        return -1;
    
    // reboot
    wifi->reboot();
    
    // set udp protocol
    wifi->setProtocol(UDP);
    
    // connect the network
    if (wifi->isDHCP()) {
        if (!wifi->sendCommand("join\r", "DHCP=ON", NULL, 10000))
            return -1;
    } else {
        if (!wifi->sendCommand("join\r", "Associated", NULL, 10000))
            return -1;
    }
        
    // exit
    wifi->exit();
    wifi->flush();
    return 0;
}

// -1 if unsuccessful, else number of bytes written
int UDPSocket::sendTo(Endpoint &remote, char *packet, int length)
{
    Timer tmr;
    int idx = 0;

    confEndpoint(remote);

    tmr.start();

    while ((tmr.read_ms() < _timeout) || _blocking) {

        idx += wifi->send(packet, length);

        if (idx == length)
            return idx;
    }
    return (idx == 0) ? -1 : idx;
}

// -1 if unsuccessful, else number of bytes received
int UDPSocket::receiveFrom(Endpoint &remote, char *buffer, int length)
{
    Timer tmr;
    int idx = 0;
    int nb_available = 0;
    int time = -1;

    if (_blocking) {
        while (1) {
            nb_available = wifi->readable();
            if (nb_available != 0) {
                break;
            }
        }
    }

    tmr.start();

    while (time < _timeout) {

        nb_available = wifi->readable();
        for (int i = 0; i < min(nb_available, length); i++) {
            buffer[idx] = wifi->getc();
            idx++;
        }

        if (idx == length) {
            break;
        }
        
        time = tmr.read_ms();
    }

    readEndpoint(remote);
    return (idx == 0) ? -1 : idx;
}

bool UDPSocket::confEndpoint(Endpoint & ep)
{
    char * host;
    char cmd[30];
    if (!endpoint_configured) {
        host = ep.get_address();
        if (host[0] != '\0') {
            // set host
            sprintf(cmd, "set i h %s\r", host);
            if (!wifi->sendCommand(cmd, "AOK"))
                return false;
                
            // set remote port
            sprintf(cmd, "set i r %d\r", ep.get_port());
            if (!wifi->sendCommand(cmd, "AOK"))
                return false;
                
            wifi->exit();
            endpoint_configured = true;
            return true;
        }
    }
    return true;
}

bool UDPSocket::readEndpoint(Endpoint & ep)
{
    char recv[256];
    int begin = 0;
    int end = 0;
    string str;
    string addr;
    int port;
    if (!endpoint_read) {
        if (!wifi->sendCommand("get ip\r", NULL, recv))
            return false;
        wifi->exit();
        str = recv;
        begin = str.find("HOST=");
        end = str.find("PROTO=");
        if (begin != string::npos && end != string::npos) {
            str = str.substr(begin + 5, end - begin - 5);
            int pos = str.find(":");
            if (pos != string::npos) {
                addr = str.substr(0, pos);
                port = atoi(str.substr(pos + 1).c_str());
                ep.set_address(addr.c_str(), port);
                endpoint_read = true;
                wifi->flush();
                return true;
            }
        }
        wifi->flush();
    }
    return false;
}
