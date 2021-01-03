#ifndef _TRIGGER_GOOGLESHEETS_H_
#define _TRIGGER_GOOGLESHEETS_H_

#include <stdarg.h>
#include <string.h>

void sheets_initialization();
void Send_Data();
void Google_Sheets_Init(char array_2d[10][6], String sheets_gas_id, int param_size);

String url;

char column_name[ ][6]={"x","y"};;

double random_values[100];

void float_to_string();
const char* host = "script.google.com";
const int httpsPort = 443;

char Sensor_Values[10][10];
const char* fingerprint = "46 B2 C3 44 9C 59 09 8B 01 B6 F8 BD 4C FB 00 74 91 2F EF F6";
String GAS_ID;
int Count;

void Google_Sheets_Init(char test[ ][6], String sheets_gas_id, int param_size)
{
	GAS_ID = sheets_gas_id;
	Count = param_size;
	
	for (int i = 0; i < Count; i++)
    {
	 for (int j = 0; j < 6; j++)
     {
		column_name[i][j] = test[i][j];
     }
    }
	
	#ifdef Debug_Serial_Mon
	   for(int i=0;i<Count;i++)
	   {
			Serial.print("column_name= ");
			Serial.println(column_name[i]);
	   }
   #endif
}

void Data_to_Sheets(int num, ...)
{
	va_list lst;
	va_start(lst,num);
	
	for(int i=0;i<num;i++)
	{
		random_values[i]= va_arg(lst,double);
	}
	va_end(lst);	
	
	float_to_string();
	Send_Data();
}

void float_to_string()
{
  for(int j=0;j<Count;j++)
  {
    sprintf(Sensor_Values[j],"%.02f",random_values[j]);
	#ifdef Debug_Serial_Mon
		Serial.print("Sensor Values : ");
		Serial.println(Sensor_Values[j]);
	#endif
  }
}
void Send_Data()
{
	sheets_initialization();
    
	String url = "/macros/s/" + GAS_ID + "/exec?";
	int i=0;
	while(i!=Count)
	{
		if(i==0)
		{
		  url = url+column_name[i]+"="+Sensor_Values[i];
		  i++;
		}
		if(i==Count)
		  break;
		url = url+"&"+column_name[i]+"="+Sensor_Values[i];
		i++;    
	}
  
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");

  #ifdef Debug_Serial_Mon
  Serial.println("request sent");
  #endif
  while (client.connected()) 
  {
	  String line = client.readStringUntil('\n');
	  if (line == "\r") 
	  {
		#ifdef Debug_Serial_Mon  
		Serial.println("headers received");
		#endif
		break;
	  }
  }
  String line = client.readStringUntil('\n');
  
  if (line.startsWith("{\"state\":\"success\"")) 
  {
	#ifdef Debug_Serial_Mon
	Serial.println("esp8266/Arduino CI successfull!");
	#endif
  } 
  else 
  {
	#ifdef Debug_Serial_Mon  
	Serial.println("esp8266/Arduino CI has failed");
	#endif
  }
  
  #ifdef Debug_Serial_Mon
	  Serial.println("reply was:");
	  Serial.println("==========");
	  Serial.println(line);
	  Serial.println("==========");
	  Serial.println("closing connection");
  #endif
	
}
void sheets_initialization()
{
	client.setInsecure(); 
  
  #ifdef Debug_Serial_Mon
	  Serial.print("connecting to ");
	  Serial.println(host);
  #endif
  
  if (!client.connect(host, httpsPort)) 
  {
    #ifdef Debug_Serial_Mon
	Serial.println("connection failed");
	#endif
    return;
  }

  if (client.verify(fingerprint, host)) 
  {
	#ifdef Debug_Serial_Mon
	Serial.println("certificate matches");
	#endif
  } 
  else 
  {
    #ifdef Debug_Serial_Mon
	Serial.println("certificate doesn't match");
	#endif
  }
	
}
#endif
