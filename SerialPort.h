#include<iostream>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef SERIALPORT_H
#define SERIALPORT_H

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

#endif
