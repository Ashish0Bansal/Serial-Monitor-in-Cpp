#include <ctime>
#include<iostream>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <conio.h>
#include "..\gotoxy.h"
#include <string>

using std::cout;
using std::cin;
using std::endl;


class SerialPort
{
	private:
		HANDLE handleToCOM;
		
		COMSTAT status;
		DWORD errors;
		
	public:
		bool connected;
		SerialPort(char *portName, long baud_rate);
		~SerialPort();
		
		int ReadSerialPort(char*buffer, unsigned int buf_size);
		bool WritrSerialPort(char* buffer, unsigned int buf_size);
		void Reconnect(long baud_rate);
	protected:
};

void Error_message(DWORD errMsg);

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



long const baud_rates[] = {300,600, 2400, 4800, 9600, 19200};
#define total_Baud_rates sizeof(baud_rates)/sizeof(long)
void Display_Baud_rates(){
	cout<<"Baud Rate : \n";
	for(short i=0; i<total_Baud_rates; ++i){
		cout<<i+1<<". "<<baud_rates[i]<<endl;
	}
	
}

char* Current_time(void){
	
	time_t now = time(0);
	char* dt = ctime(&now)+11;
	*(dt+8) = 0;
	return dt;
}

int main(){
	
	char port[4];
	port[3] = '\0';
	int y=1;
	int timestamp_permission=0;

	Display_Baud_rates();
	
	int ch;
	
	do{
		gotoxy(10, y);
		cout<<"<-";
		ch = getch();
		gotoxy(10, y);
		cout<<"  ";	
		switch(ch){
			case 72: 
					if(y!=1)
						y--;	
					break;
			case 80: 
					if(y!=total_Baud_rates)
						y++;
					
					break;
		}
		
	}while(ch!=13);
	
	cout<<"\nCom Port : ";
	cin>>port;
	
	cout<<"Show timestamps? Y/N ";
	int x=17;
	do{
		gotoxy(x, total_Baud_rates+2);
		ch = getch();

		switch(ch){
			case 75: 
					if(x==17)
						x+=2;
					else
						x-=2;	
					break;
			case 77: 
					if(x==19)
						x-=2;
					else
						x+=2;
					
					break;
		}
		
	}while(ch!=13);
	
	timestamp_permission = x==17 ? 1:0;
	
	SerialPort comport(port, baud_rates[y-1]);
	cout<<endl;
	char buffer[7] = {0};
	int count = 0;
	while(comport.connected) {
		count++;
		
		buffer[7] = 0;
		comport.ReadSerialPort(buffer, 7);
		if(buffer[0]!=0){
			if(timestamp_permission==1)
				cout<<Current_time()<<" -> ";
			cout<<buffer;
		}
		if(GetLastError()!=0){
			comport.Reconnect(baud_rates[y-1]);
		}
		Sleep(100);
	}
	
	return 0;
}
