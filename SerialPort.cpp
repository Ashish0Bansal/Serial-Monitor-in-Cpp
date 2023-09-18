#include "SerialPort.h"

using std::cout;
using std::cin;

void Error_message(DWORD errMsg){
	if(errMsg==2){
		std::cout<<"Plug the device\n";
	}
	else if(errMsg==5)
		std::cout<<"USED BY ANOTHER APP\n";
}

SerialPort::SerialPort(char *portName, long baud_rate=9600)
{
	errors = 0;
	status = {0};
	connected = false;
	
	handleToCOM = CreateFileA(static_cast<LPSTR>(portName), GENERIC_READ|GENERIC_WRITE,0,NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL);
	DWORD errMsg = GetLastError();
	
	if(errMsg==0){
		DCB dcbSerialParameters = {0};
		
		if(!GetCommState(handleToCOM, &dcbSerialParameters)){
			cout<<"Failed to connect";
		}
		else{
			dcbSerialParameters.BaudRate = baud_rate;
			dcbSerialParameters.ByteSize = 8;
			dcbSerialParameters.StopBits = ONESTOPBIT;
			dcbSerialParameters.Parity = NOPARITY;
			dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;
			
			if(!SetCommState(handleToCOM, &dcbSerialParameters)){
				cout<<"COULDN'T set serial port parameters";
			}
			else{
				connected = true;
				PurgeComm(handleToCOM, PURGE_RXCLEAR | PURGE_TXCLEAR);
				Sleep(10);
			}
		}
	}
	else Error_message(errMsg);	
}

int SerialPort::ReadSerialPort(char*buffer, unsigned int buf_size){
	DWORD byteRead;
	unsigned int toRead = 0;
	
	ClearCommError(handleToCOM, &errors, &status);
	
	if(status.cbInQue > 0){
		if(status.cbInQue>buf_size){
			toRead = buf_size;
		}
		else
			toRead = status.cbInQue;
	}
	
	if(ReadFile(handleToCOM, buffer, 7, &byteRead, NULL)){
		return byteRead;
	}
	return 0;
}

bool SerialPort::WritrSerialPort(char* buffer, unsigned int buf_size){
	;
}

void SerialPort::Reconnect(long baud_rate=9600){
	handleToCOM = CreateFileA(static_cast<LPSTR>("COM8"), GENERIC_READ|GENERIC_WRITE,0,NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL);
	DCB dcbSerialParameters = {0};
		
	if(GetCommState(handleToCOM, &dcbSerialParameters)){
		dcbSerialParameters.BaudRate = baud_rate;
		dcbSerialParameters.ByteSize = 8;
		dcbSerialParameters.StopBits = ONESTOPBIT;
		dcbSerialParameters.Parity = NOPARITY;
		dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;
		
		if(SetCommState(handleToCOM, &dcbSerialParameters)){
			connected = true;
			PurgeComm(handleToCOM, PURGE_RXCLEAR | PURGE_TXCLEAR);
			Sleep(10);
		}
	}
}

SerialPort::~SerialPort()
{
	if(connected==true){
		connected = false;
		CloseHandle(handleToCOM);
	}
}
