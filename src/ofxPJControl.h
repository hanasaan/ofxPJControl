/*
 *  pjControl.h
 *
 *  Created by Noah Shibley on 8/9/10.
 *  Updated by Martial GALLORINI on 19/03/2015
 *
	Video projector control class.
 *
 *
 */

/* usage
 ofxPJControl pj;
 pj.setup("192.168.7.60", 4352, PJLINK_MODE, ""); //ipはプロジェクタのIP

 //電源ON
 pj.On();
 
 
 
 //入力切替　input Aに
 pj.inputSelect(SONY_INPUT_A);
 
 //入力切替 input Bに
 pj.inputSelect(SONY_INPUT_B);
 
 //入力切替 input Cに
 pj.inputSelect(SONY_INPUT_C);
 
 //入力切替 input Dに
 pj.inputSelect(SONY_INPUT_D);
 
 //電源OFF
 pj.Off();
 
*/

#ifndef OFXPJCONTROL_H
#define OFXPJCONTROL_H

#include "ofxNetwork.h"
#include "Poco/MD5Engine.h"
#include "Poco/DigestStream.h"
#include "Poco/StreamCopier.h"

using Poco::DigestEngine;
using Poco::MD5Engine;
using Poco::DigestOutputStream;
using Poco::StreamCopier;


const int NEC_PORT = 7142; //NEC projector port
const int PJLINK_PORT = 4352; //PJLink projector protocol port
const int CHRISTIE_PORT = 3002; //CHRISTIE projector protocol port
const int SANYO_PORT = 100; //SANYO projector protocol port
const int PJDESIGN_PORT = 1025; //Projectino Design projector protocol port
const int DIGITALCOM_PORT = 7000; //Digital Communication projector port

const int PJLINK_MODE = 0;
const int NEC_MODE = 1;
const int CHRISTIE_MODE = 2;
const int SANYO_MODE = 3;
const int PJDESIGN_MODE = 4;

enum{
    SONY_INPUT_A=1,
    SONY_INPUT_B,
    SONY_INPUT_C,
    SONY_INPUT_D,
};

class ofxPJControl
{
public:
	ofxPJControl();
	~ofxPJControl();

	//methods
	void On(); //command to turn the projector off
	void Off(); //command to turn the projector on
	void sendPJLinkCommand(string command); //send any PJLink command to the projector
	void setup(string IP_add="192.168.0.100",int port = 4352, int protocol=PJLINK_MODE, string password=""); //default
	void setProjectorType(int protocol); //NEC_MODE or PJLINK_MODE
	void setProjectorIP(string IP_add); //the network IP of the projector
    void setProjectorPassword(string passwd); //password for PJLink authentication
	bool getProjectorStatus(); //return whether projector is on (true) or off (false)
	void setProjectorPort(int port); //the network port of the projector
	void sendCommand(string command); //send any string command to the projector without password authentication
    
    void shutter(bool b);
    void christie_shutter(bool b);
    void digitalcom_shutter(bool b);
    bool getShutterState()const{return shutterState;}
    void inputSelect(int input);

    void digitalcom_inputSelect(int input);
    int getInputState()const{return inputState;}

protected:

	void nec_On();
	void nec_Off();
	void pjLink_On();
	void pjLink_Off();
	void sanyo_On();
	void sanyo_Off();
	void christie_On();
	void christie_Off();
    void pjDesign_On();
    void pjDesign_Off();

	ofxTCPClient pjClient;

	string IPAddress;
	int pjPort;
    string password;

	bool projStatus;
	string msgTx;
	string msgRx;
	bool connected;
	int commMode;

    bool shutterState;
    int inputState;
};

#endif

