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

#include "ofxPJControl.h"
#include "ofMain.h"

ofxPJControl::ofxPJControl() {
	connected = false;
	projStatus = false;
}

ofxPJControl::~ofxPJControl() {

}

bool ofxPJControl::getProjectorStatus() {
	return projStatus;
}

void ofxPJControl::setProjectorType(int protocol) { //NEC_MODE or PJLINK_MODE
		commMode = protocol;
}

void ofxPJControl::setup(string IP_add, int port, int protocol, string password) {
	setProjectorIP(IP_add);
	setProjectorType(protocol);
	setProjectorPort(port);
    setProjectorPassword(password);
}

void ofxPJControl::setProjectorIP(string IP_add) {
	IPAddress = IP_add;
}

void ofxPJControl::setProjectorPort(int port) {
	pjPort = port;
}

void ofxPJControl::setProjectorPassword(string passwd) {
	password = passwd;
}


void ofxPJControl::On() {
	if(commMode == NEC_MODE) {
		nec_On();
	}
	else if (commMode == PJLINK_MODE) {
		pjLink_On();
	}
    else if (commMode == CHRISTIE_MODE) {
		christie_On();
	}
    else if (commMode == SANYO_MODE) {
		sanyo_On();
	}
    else if (commMode == PJDESIGN_MODE) {
        pjDesign_On();
    }
}

void ofxPJControl::Off(){
	if(commMode == NEC_MODE) {
		nec_Off();
	}
	else if (commMode == PJLINK_MODE) {
		pjLink_Off();
	}
	else if (commMode == CHRISTIE_MODE) {
		christie_Off();
	}
    else if (commMode == SANYO_MODE) {
		sanyo_Off();
	}
    else if (commMode == PJDESIGN_MODE) {
        pjDesign_Off();
    }
}

void ofxPJControl::sendPJLinkCommand(string command) {
		string msgRx="";

        if(!pjClient.isConnected()) {
            pjClient.setVerbose(true);
            connected = pjClient.setup(IPAddress, pjPort,true);
            if(connected){
                ofLogNotice() << "connection established: " << IPAddress << ":" << pjPort;
                string response = "";
                while (msgRx.length() < 8) {
                    msgRx = pjClient.receiveRaw();
                }
                ofLogNotice() << "received response: " << msgRx;
            } else {
                ofLogError() << "faled to connect.";
            }
        }
    
        //cout << "OK" << endl;
        if(connected){
            string authToken = "";

            //eg. PJLINK 1 604cc14d
            if(msgRx[7] == '1') {
                ofLogNotice() << "with authentication";
                MD5Engine md5;
                md5.reset();
                string hash = msgRx.substr(9,8);
                ofLogNotice() << hash << endl;
                //cout << "OK" << endl;
                md5.update(hash + password);
                authToken = DigestEngine::digestToHex(md5.digest());
            }
            ofLogNotice() << "sending command: " << authToken+command;
            pjClient.sendRaw(authToken+command);
            msgRx = "";
            while (msgRx.length() < 8) {
                msgRx = pjClient.receiveRaw();
            }
            ofLogNotice() << "received response: " << msgRx;

        
            pjClient.close();
            //connected = false;
        } else {
            ofLogError()<< "still not connected.";
            pjClient.close();

        }
}

void ofxPJControl::sendCommand(string command){
        if(!pjClient.isConnected()) {
			pjClient.setVerbose(true);
			ofLogNotice() << "connecting to : " << IPAddress << ":" << pjPort;
			connected = pjClient.setup(IPAddress, pjPort, true);
			ofLogNotice() << "connection state : " << connected;
		}
        ofLogNotice() << "sending command : " << command;
        pjClient.sendRaw(command);
        ofSleepMillis(100);
        ofLogNotice() << "Response length (Bytes) : " << pjClient.getNumReceivedBytes();
        msgRx = "";
        msgRxRaw.resize(128);
        if(pjClient.getNumReceivedBytes() > 0){
            //msgRx = pjClient.receiveRaw();
            int nsize = pjClient.receiveRawMsg(&msgRxRaw.front(), msgRxRaw.size());
            msgRxRaw.resize(nsize);
            ofLogNotice() << "received response : " << msgRx;
        }
    
        pjClient.close();
}

void ofxPJControl::nec_On(){

	pjClient.close(); //close any open connections first
	char* buffer = new char[6]; //02H 00H 00H 00H 00H 02H (the on command in hex)
	buffer[0] = 2;
	buffer[1] = 0;
	buffer[2] = 0;
	buffer[3] = 0;
	buffer[4] = 0;
	buffer[5] = 2;

	pjClient.setVerbose(true);
	if(!pjClient.isConnected()) {
		connected = pjClient.setup(IPAddress, NEC_PORT);
		ofLogNotice() << "connection established: " << IPAddress << ":" << NEC_PORT;
	}
	ofLogNotice() << "sending command: ON";

	pjClient.sendRawBytes(buffer, 6);

	printf("sent: %x %x %x %x %x %x\n",buffer[0] , buffer[1] , buffer[2] , buffer[3] , buffer[4] , buffer[5] );

	char* rxBuffer = new char[6];

	pjClient.receiveRawBytes(rxBuffer, 6);

	printf("received: %x %x %x %x %x %x\n",rxBuffer[0] , rxBuffer[1] , rxBuffer[2] , rxBuffer[3] , rxBuffer[4] , rxBuffer[5] );

	projStatus = true;

	delete[] rxBuffer;
	delete[] buffer;
}

void ofxPJControl::nec_Off() {

	char* buffer = new char[6]; //02H 01H 00H 00H 00H 03H (the off command in hex)
	buffer[0] = 2;
	buffer[1] = 1;
	buffer[2] = 0;
	buffer[3] = 0;
	buffer[4] = 0;
	buffer[5] = 3;

	projStatus = true;

	pjClient.setVerbose(true);

	if(!pjClient.isConnected()) {
		connected = pjClient.setup(IPAddress, NEC_PORT);
		ofLogNotice() << "connection established: " << IPAddress << ":" << NEC_PORT;
	}

	ofLogNotice() << "sending command: OFF ";

	pjClient.sendRawBytes(buffer, 6);
	printf("send: %x %x %x %x %x %x\n",buffer[0] , buffer[1] , buffer[2] , buffer[3] , buffer[4] , buffer[5] );


	char* rxBuffer = new char[6];

	pjClient.receiveRawBytes(rxBuffer, 6);

	printf("receive: %x %x %x %x %x %x\n",rxBuffer[0] , rxBuffer[1] , rxBuffer[2] , rxBuffer[3] , rxBuffer[4] , rxBuffer[5] );

	projStatus = false;

	delete[] rxBuffer;
	delete[] buffer;
}

void ofxPJControl::pjLink_On() {
	string command = "%1POWR 1\r";
    sendPJLinkCommand(command);
    projStatus = true; //projector on

}

void ofxPJControl::pjLink_Off() {
	string command = "%1POWR 0\r";
	sendPJLinkCommand(command);
	projStatus = false; //projector off
}

void ofxPJControl::sanyo_On() {
	string command = "PWR ON\r";
	sendCommand(command);
	projStatus = true; //projector on
}

void ofxPJControl::sanyo_Off() {
	string command = "PWR OFF\r";
	sendCommand(command);
	projStatus = false; //projector off
}

void ofxPJControl::christie_On() {
	string command = "(PWR1)";
	sendCommand(command);
	projStatus = true; //projector on
}

void ofxPJControl::christie_Off() {
	string command = "(PWR0)";
	sendCommand(command);
	projStatus = false; //projector off
}

void ofxPJControl::pjDesign_On() {
    string command = ":POWR 1\r";
    sendCommand(command);
    projStatus = true; //projector on
}

void ofxPJControl::pjDesign_Off() {
    string command = ":POWR 0\r";
    sendCommand(command);
    projStatus = false; //projector off
}

void ofxPJControl::shutter(bool b) {
    if (commMode == PJLINK_MODE) {
        pjLink_shutter(b);
    }
    else if (commMode == CHRISTIE_MODE) {
        christie_shutter(b);
    }
    else if (commMode == BARCO_MODE) {
        barco_shutter(b);
    }
}

void ofxPJControl::pjLink_shutter(bool b)
{
    if(b){
        sendPJLinkCommand("%1AVMT 31\r");
    }
    else{
        sendPJLinkCommand("%1AVMT 30\r");
    }
    shutterState = b;
}

void ofxPJControl::christie_shutter(bool b){
    if(b){
        string command = "(SHU 1)";
        sendCommand(command);
    }
    else{
        string command = "(SHU 0)";
        sendCommand(command);
    }
    shutterState = b;
}

void ofxPJControl::barco_shutter(bool b){
    if(b){
        vector<uint8_t> com = {0x23, 0x42, 0x00};
        sendBarcoCommand(com);
    }
    else{
        vector<uint8_t> com = {0x22, 0x42, 0x00};
        sendBarcoCommand(com);
    }
    shutterState = b;
}

void ofxPJControl::sendBarcoCommand(const vector<uint8_t>& com)
{
    int checksum = 0;
    for (const auto& c : com) {
        checksum += c;
    }
    checksum = checksum % 256;
    string command;
    command.resize(com.size() + 4, 0xFF);
    command[0] = 0xFE;
    command[1] = 0x00;
    for (int i=0; i<com.size(); ++i) {
        command[i + 2] = com.at(i);
    }
    command[2 + com.size()] = checksum;
    command[3 + com.size()] = 0xFF;
    sendCommand(command);
}


void ofxPJControl::digitalcom_shutter(bool b)
{
    if(b){
        string cmd = "*shutter = on\r";
        sendCommand(cmd);
    }
    else{
        string cmd = "*shutter = off\r";
        sendCommand(cmd);
    }
    shutterState = b;
}

void ofxPJControl::digitalcom_inputSelect(int input)
{
    string cmd = "*input = " + ofToString(input) + "\r";
    sendCommand(cmd);
    inputState = input;
}

void ofxPJControl::inputSelect(int input)
{
    string command;
    
    switch(input){
    case SONY_INPUT_A:
        command = "%1INPT 11\r";
        break;
    case SONY_INPUT_B:
        command = "%1INPT 31\r";
        break;
    case SONY_INPUT_C:
        command = "%1INPT 32\r";
        break;
    case SONY_INPUT_D:
        command = "%1INPT 51\r";
        break;
    default:
        command = "";
            break;
    }
    
    if(command != ""){
        sendPJLinkCommand(command);
    }
}

bool ofxPJControl::getShutterCommand()
{
    if (commMode == CHRISTIE_MODE) {
        return christie_get_shutter();
    }
    else if (commMode == BARCO_MODE) {
        return barco_get_shutter();
    }
    return false;
}


bool ofxPJControl::barco_get_shutter()
{
    vector<uint8_t> com = {0x21, 0x42};
    sendBarcoCommand(com);
    if (msgRxRaw.size() > 4) {
        return msgRxRaw[4] == 0x00;
    }
    ofLogError() << "get shutter status failed.";
    return false;
}

bool ofxPJControl::christie_get_shutter()
{
    return false;
}




