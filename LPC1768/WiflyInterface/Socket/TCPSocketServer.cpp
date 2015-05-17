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

#include "TCPSocketServer.h"
#include <string>

TCPSocketServer::TCPSocketServer() {}

// Server initialization
int TCPSocketServer::bind(int port) {
    char cmd[20];
    
    // set TCP protocol
    wifi->setProtocol(TCP);
    
    // set local port
    sprintf(cmd, "set i l %d\r", port);
    if (!wifi->sendCommand(cmd, "AOK"))
        return -1;
    
    // save
    if (!wifi->sendCommand("save\r", "Stor"))
        return -1;
    
    // reboot
    wifi->reboot();
    
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
    
    wait(0.2);
    wifi->flush();
    return 0;
}

int TCPSocketServer::listen(int backlog) {
    if (backlog != 1)
        return -1;
    return 0;
}


int TCPSocketServer::accept(TCPSocketConnection& connection) {
    int nb_available = 0, pos = 0;
    char c;
    string str;
    bool o_find = false;
    while (1) {
        while(!wifi->readable());
        nb_available = wifi->readable();
        for (int i = 0; i < nb_available; i++) {
            c = wifi->getc();
            if (c == '*') {
                o_find = true;
            }
            if (o_find && c != '\r' && c != '\n') {
                str += c;
                pos = str.find("*OPEN*");
                if (pos != string::npos) {
                    wifi->flush();
                    return 0;
                }
            }
        }
    }
}