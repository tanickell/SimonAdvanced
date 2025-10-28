#ifndef _WEMOOBJ_H_
#define _WEMOOBJ_H_

/*
 *  Project: Wemo IoT Library
 *  Description: Library for controlling Wemo Smart Outlets at IOT Classroom
 *  Authors:  Brian Rashap
 *  Date:     06-FEB-2022
 */

#include "application.h"

class WemoObj {

    TCPClient WemoClient;
    int wemoPort = 49153;
    const char *wemoIP[6] = {"192.168.1.30","192.168.1.31","192.168.1.32","192.168.1.33","192.168.1.34","192.168.1.35"};
    int _wemoNum;
    bool onOff = false;

    public:

        WemoObj(int wemoNum) {
            _wemoNum = wemoNum;
        }

        // Turn on/off wemo outlets similar to digitalWrite
        void wemoWrite(bool wemoState) {
            if(wemoState) {
                switchON(_wemoNum);
            }
            else {
                switchOFF(_wemoNum);
            }
        }

        void toggleOnOff() {
            if (onOff) {
                wemoWrite(HIGH);
            }
            else {
                wemoWrite(false);
            }
            onOff = !onOff;
        }

        void turnOn() {
            switchON(_wemoNum);
        }

        void turnOff() {
            switchOFF(_wemoNum);
        }

    private:
        // turn on specified wemo outlet
        void switchON(int wemo) {
        
            String data1;
            
            Serial.printf("Switching On Wemo #%i\n",wemo);
            data1+="<?xml version=\"1.0\" encoding=\"utf-8\"?><s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:SetBinaryState xmlns:u=\"urn:Belkin:service:basicevent:1\"><BinaryState>1</BinaryState></u:SetBinaryState></s:Body></s:Envelope>"; // Use HTML encoding for comma's
            if (WemoClient.connect(wemoIP[wemo],wemoPort)) {
                    WemoClient.println("POST /upnp/control/basicevent1 HTTP/1.1");
                    WemoClient.println("Content-Type: text/xml; charset=utf-8");
                    WemoClient.println("SOAPACTION: \"urn:Belkin:service:basicevent:1#SetBinaryState\"");
                    WemoClient.println("Connection: keep-alive");
                    WemoClient.print("Content-Length: ");
                    WemoClient.println(data1.length());
                    WemoClient.println();
                    WemoClient.print(data1);
                    WemoClient.println();
                }

            if (WemoClient.connected()) {
                WemoClient.stop();
            }
        }

        // turn off wemo outlet specified
        void switchOFF(int wemo){
            String data1;
            
            Serial.printf("Switching Off Wemo #%i \n",wemo);
            data1+="<?xml version=\"1.0\" encoding=\"utf-8\"?><s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:SetBinaryState xmlns:u=\"urn:Belkin:service:basicevent:1\"><BinaryState>0</BinaryState></u:SetBinaryState></s:Body></s:Envelope>"; // Use HTML encoding for comma's
            if (WemoClient.connect(wemoIP[wemo],wemoPort)) {
                    WemoClient.println("POST /upnp/control/basicevent1 HTTP/1.1");
                    WemoClient.println("Content-Type: text/xml; charset=utf-8");
                    WemoClient.println("SOAPACTION: \"urn:Belkin:service:basicevent:1#SetBinaryState\"");
                    WemoClient.println("Connection: keep-alive");
                    WemoClient.print("Content-Length: ");
                    WemoClient.println(data1.length());
                    WemoClient.println();
                    WemoClient.print(data1);
                    WemoClient.println();
                }
            
            if (WemoClient.connected()) {
                WemoClient.stop();
            }
        }

};

#endif // _WEMOOBJ_H_