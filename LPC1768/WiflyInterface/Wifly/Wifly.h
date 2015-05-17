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
 *
 * @section DESCRIPTION
 *
 * Wifly RN131-C, wifi module
 *
 * Datasheet:
 *
 * http://dlnmh9ip6v2uc.cloudfront.net/datasheets/Wireless/WiFi/WiFly-RN-UM.pdf
 */

#ifndef WIFLY_H
#define WIFLY_H

#include "mbed.h"
#include "RawSerial.h"
#include "CBuffer.h"

#define DEFAULT_WAIT_RESP_TIMEOUT 10000

enum Security {
    NONE = 0,
    WEP_128 = 1,
    WPA = 3
};

enum Protocol {
    UDP = (1 << 0),
    TCP = (1 << 1)
};

/**
 * The Wifly class
 */
class Wifly
{

public:
    /**
    * Constructor
    *
    * @param tx mbed pin to use for tx line of Serial interface
    * @param rx mbed pin to use for rx line of Serial interface
    * @param reset reset pin of the wifi module ()
    * @param tcp_status connection status pin of the wifi module (GPIO 6)
    * @param ssid ssid of the network
    * @param phrase WEP or WPA key
    * @param sec Security type (NONE, WEP_128 or WPA)
    */
    Wifly(  PinName tx, PinName rx, PinName reset, PinName tcp_status, const char * ssid, const char * phrase, Security sec);

    /**
    * Connect the wifi module to the ssid contained in the constructor.
    *
    * @return true if connected, false otherwise
    */
    bool join();

    /**
    * Disconnect the wifly module from the access point
    *
    * @return true if successful
    */
    bool disconnect();

    /**
    * Open a tcp connection with the specified host on the specified port
    *
    * @param host host (can be either an ip address or a name. If a name is provided, a dns request will be established)
    * @param port port
    * @return true if successful
    */
    bool connect(const char * host, int port);


    /**
    * Set the protocol (UDP or TCP)
    *
    * @param p protocol
    * @return true if successful
    */
    bool setProtocol(Protocol p);

    /**
    * Reset the wifi module
    */
    void reset();
    
    /**
    * Reboot the wifi module
    */
    bool reboot();

    /**
    * Check if characters are available
    *
    * @return number of available characters
    */
    int readable();

    /**
    * Check if characters are available
    *
    * @return number of available characters
    */
    int writeable();

    /**
    * Check if a tcp link is active
    *
    * @return true if successful
    */
    bool is_connected();

    /**
    * Read a character
    *
    * @return the character read
    */
    char getc();

    /**
    * Flush the buffer
    */
    void flush();

    /**
    * Write a character
    *
    * @param the character which will be written
    */
    int putc(char c);


    /**
    * To enter in command mode (we can configure the module)
    *
    * @return true if successful, false otherwise
    */
    bool cmdMode();

    /**
    * To exit the command mode
    *
    * @return true if successful, false otherwise
    */
    bool exit();

    /**
    * Close a tcp connection
    *
    * @return true if successful
    */
    bool close();

    /**
    * Send a string to the wifi module by serial port. This function desactivates the user interrupt handler when a character is received to analyze the response from the wifi module.
    * Useful to send a command to the module and wait a response.
    *
    *
    * @param str string to be sent
    * @param len string length
    * @param ACK string which must be acknowledge by the wifi module. If ACK == NULL, no string has to be acknoledged. (default: "NO")
    * @param res this field will contain the response from the wifi module, result of a command sent. This field is available only if ACK = "NO" AND res != NULL (default: NULL)
    *
    * @return true if ACK has been found in the response from the wifi module. False otherwise or if there is no response in 5s.
    */
    int send(const char * str, int len, const char * ACK = NULL, char * res = NULL, int timeout = DEFAULT_WAIT_RESP_TIMEOUT);

    /**
    * Send a command to the wify module. Check if the module is in command mode. If not enter in command mode
    *
    * @param str string to be sent
    * @param ACK string which must be acknowledge by the wifi module. If ACK == NULL, no string has to be acknoledged. (default: "NO")
    * @param res this field will contain the response from the wifi module, result of a command sent. This field is available only if ACK = "NO" AND res != NULL (default: NULL)
    *
    * @return true if successful
    */
    bool sendCommand(const char * cmd, const char * ack = NULL, char * res = NULL, int timeout = DEFAULT_WAIT_RESP_TIMEOUT);
    
    /**
    * Return true if the module is using dhcp
    *
    * @return true if the module is using dhcp
    */
    bool isDHCP() {
        return state.dhcp;
    }

    bool gethostbyname(const char * host, char * ip);

    static Wifly * getInstance() {
        return inst;
    };

protected:
    RawSerial wifi;
    DigitalOut reset_pin;
    DigitalIn tcp_status;
    char phrase[65];
    char ssid[33];
    const char * ip;
    const char * netmask;
    const char * gateway;
    int channel;
    CircBuffer<char> buf_wifly;

    static Wifly * inst;

    void attach_rx(bool null);
    void handler_rx(void);


    typedef struct STATE {
        bool associated;
        bool tcp;
        bool dhcp;
        Security sec;
        Protocol proto;
        bool cmd_mode;
    } State;

    State state;
    char * getStringSecurity();
};

#endif
