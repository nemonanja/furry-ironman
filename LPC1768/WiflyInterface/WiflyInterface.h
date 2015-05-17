/* WiflyInterface.h */
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
 
#ifndef WIFLYINTERFACE_H_
#define WIFLYINTERFACE_H_

#include "Wifly.h"

 /**
 * Interface using Wifly to connect to an IP-based network
 */
class WiflyInterface: public Wifly {
public:

    /**
    * Constructor
    *
    * \param tx mbed pin to use for tx line of Serial interface
    * \param rx mbed pin to use for rx line of Serial interface
    * \param reset reset pin of the wifi module ()
    * \param tcp_status connection status pin of the wifi module (GPIO 6)
    * \param ssid ssid of the network
    * \param phrase WEP or WPA key
    * \param sec Security type (NONE, WEP_128 or WPA)
    */
  WiflyInterface(PinName tx, PinName rx, PinName reset, PinName tcp_status, const char * ssid, const char * phrase, Security sec = NONE);

  /** Initialize the interface with DHCP.
  * Initialize the interface and configure it to use DHCP (no connection at this point).
  * \return 0 on success, a negative number on failure
  */
  int init(); //With DHCP

  /** Initialize the interface with a static IP address.
  * Initialize the interface and configure it with the following static configuration (no connection at this point).
  * \param ip the IP address to use
  * \param mask the IP address mask
  * \param gateway the gateway to use
  * \return 0 on success, a negative number on failure
  */
  int init(const char* ip, const char* mask, const char* gateway);

  /** Connect
  * Bring the interface up, start DHCP if needed.
  * \return 0 on success, a negative number on failure
  */
  int connect();
  
  /** Disconnect
  * Bring the interface down
  * \return 0 on success, a negative number on failure
  */
  int disconnect();
  
  /** Get IP address
  *
  * \return ip address
  */
  char* getIPAddress();
  
private:
    char ip_string[20];
    bool ip_set;
};

#include "TCPSocketConnection.h"
#include "TCPSocketServer.h"
#include "UDPSocket.h"

#endif /* WIFLYINTERFACE_H_ */
