/*
01-2020  M Sneijders
simple class for encoding nmea0183 VDM/VDO messages 
Not at all complete, for now enough for getting my nmea2000 ais receiver data in to OpenCPN using 
-Arduino Due
-ESP32

next on the todo list:
add message types for ATON and SAR
*/


#include "VDM_msg.h"

#include <Arduino.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


void tVdmMsg::Add0Byte(){
			uint8_t carryout; 
			uint8_t carryoutbits; 
			if (carrybits > 6 ){
				carryout = carryin >> 6; 
				carryoutbits = carrybits - 6; 
			}else{
				carryout = 0; 
				carryoutbits = 0;
			}
			EncodeByte(carryin );
			carrybits=carryoutbits; 
			carryin=carryout; 
			
}				
void tVdmMsg::Add1Byte(int8_t data, uint8_t length){				
			uint8_t carryout; 
			uint8_t carryoutbits; 
			int usefullbits; 
			int remainingbits; 

			remainingbits = (6 - length - carrybits);
			if (remainingbits > 0){
				// space remains in this byte,  carryout only, no encoding yet. 
				carryout= carryin | (data << carrybits);
				carryoutbits = length + carrybits; 
			}else{
			// this byte is full, encode it, and up the counter to the next byte
				EncodeByte((carryin | (data << carrybits)) & 0x3f); 
				carryout= data >> (6 - carrybits); 
				carryoutbits = length - (6 - carrybits); 
			}
			carrybits = carryoutbits; 
			carryin = carryout; 
			// if carrybits are 6 or more,  they fill up an entire byte,  encode carrybits, and up the counter
			if (carrybits > 5  ){
				Add0Byte();				
			}

}
void tVdmMsg::Add1UByte(uint8_t data, uint8_t length){				
			uint8_t carryout; 
			uint8_t carryoutbits; 
			int usefullbits; 
			int remainingbits; 
			remainingbits = (6 - length - carrybits);
			if (remainingbits > 0){
				// space remains in this byte,  carryout only, no encoding yet. 
				carryout=carryin | (data << carrybits);
				carryoutbits = length + carrybits; 
			}else{
			// this byte is full, encode it, and up the counter to the next byte
				EncodeByte((carryin | (data << carrybits)) & 0x3f); 
				carryout= data >> (6 - carrybits); 
				carryoutbits = length - (6 - carrybits); 
			}
			carrybits = carryoutbits; 
			carryin = carryout; 
			// if carrybits are 6 or more,  they fill up an entire byte,  encode carrybits, and up the counter
			if (carrybits > 5  ){
				Add0Byte();				
			}
}
void tVdmMsg::Add2Byte(int16_t data, uint8_t length){
		uint8_t request; 
	
	    while ( length > 0) {
			if (length > 7){
				request = 8; 
				length= length -8; 
			}else{
				request = length; 
				length = 0; 
			}
			Add1UByte((uint8_t)(data&0xFF), request); 
			data = data >> 8; 
		}
}
void tVdmMsg::Add2UByte(uint16_t data, uint8_t length){
		uint8_t request; 

	    while ( length > 0) {
			if (length > 7){
				request = 8; 
				length= length -8; 
			}else{
				request = length; 
				length = 0; 
			}
			Add1UByte((uint8_t)(data&0xFF), request); 
			data = data >> 8; 
		}
}
void tVdmMsg::Add4Byte(int32_t data, uint8_t length){
		int8_t request; 
	    
	    while ( length > 0) {
			if (length > 7){
				request = 8; 
				length= length -8; 
			}else{
				request = length; 
				length = 0; 
			}
			Add1UByte((uint8_t)(data & 0xff), request); 
			data = data >> 8; 
		}
}
void tVdmMsg::Add4UByte(uint32_t data, uint8_t length){
		uint8_t request; 

		while ( length > 0) {
			if (length > 7){
				request = 8; 
				length= length -8; 
			}else{
				request = length; 
				length = 0; 
			}
			Add1UByte((uint8_t)(data & 0xFF), request); 

			data = data >> 8; 
		}
		
}
void tVdmMsg::AddStr(const char *str, uint8_t length, char filler){
		int i=0; 
		int count=strlen(str);
		uint8_t result;
		for (i=count; i<length; i++){
			Add1UByte(((uint8_t)filler &0x3f), 6);  
		}
		for (i=0; i<count; i++){
			result=(uint8_t)str[count - 1 -i];
			if(result==0xff){
				result=(uint8_t)filler;
			}
			if(result > 96){
				result=result-32;
			}
			Add1UByte((result & 0x3f) , 6); 
		}	
	
}//*/
void tVdmMsg::EncodeByte( uint8_t result){
	 	result = (result & 0x3F ); 
		result=result+48; 
		if (result > 87){
          result=result + 8 ;
		}
     	Data[DataPointer]=result; 
		DataPointer++; 
}
tVdmMsg::tVdmMsg(){
	DataPointer = 0; 
	carryin=0; 
	carrybits=0; 
	
		
}
void tVdmMsg::Print(){
	int i;
	while (carrybits > 0){
		//Serial.print ((String) "carry bits remain,  finalizing!");
		Add0Byte();	
		//PrintBin (Data[DataPointer - 1], 1); 
		//Serial.print ("\n"); 
	}
	
	/*	
	for ( i=0; i < DataPointer; i++){
		Serial.print ((String) " counter:  " + i + " Data: " + Data[i] + " binary data: "); 
		PrintBin (Data[i], 1); 
		Serial.print ("\n"); 
	}
	*/
	
	for ( i=0; i < DataPointer; i++){
		PrintBin (Data[DataPointer - i], 1);
		//Serial.print ((char)Data[DataPointer - i] ); 
	}
	Serial.print ("\n"); 
}
void tVdmMsg::Get(char *message, uint8_t part){
	int i;

	while (carrybits > 0){
		Add0Byte();
	}
 
	
	
	// !AIVDM message overhead is 19 (20 if multimessage) so cut at 60 char's to comply with 81 char's maximum
	if (DataPointer > 60){
		if (part == 0){
			for ( i=0; i < 60; i++){
				message[i]=(char)Data[DataPointer - i -1];
			}
			message[60]=0; 			
		}
		if (part == 1){
			for ( i=60; i <  (DataPointer); i++){
				message[i-60]=(char)Data[DataPointer - i -1];
			}
			message[DataPointer-60]=0; 
		}
		if (part == 9){			//get the whole thing,  not caring how long it is. 
			for ( i=0; i <  (DataPointer); i++){
				message[i]=(char)Data[DataPointer - i -1];
			}
			message[DataPointer]=0; 
}
	}else{	
		for ( i=0; i <  (DataPointer); i++){
			message[i]=(char)Data[DataPointer - i -1];
		}
		message[DataPointer]=0; 
	}


}//*/
void tVdmMsg::PrintBin(uint8_t var, uint8_t length, uint8_t direction) {
  int i; 
  if (direction == 0){
		//left to right
		  
		  for (i=(8-length); i< 8; i++){
			  if ((( var >> (7 - i )) & 1) == 1){
				Serial.print("1"); 
			  }else{
				Serial.print("0");  
			  }
		  }
  }else{
		 //right to left
		  
		  for (i=0; i< length; i++){
			  if ((( var >> ( i )) & 1) == 1){
				Serial.print("1"); 
			  }else{
				Serial.print("0");  
			  }
		  }
  }
}
uint8_t tVdmMsg::MonthFromDate(uint16_t val){
	tmElements_t tm;
   // #ifndef _Time_h
   // time_t t=daysToTime_t(val);
  //  #else
    time_t t=val*86400;
 //   #endif
    breakTime(t, tm);
    val=(uint8_t)GetMonth(tm);
	return val;
}
uint8_t tVdmMsg::DayFromDate(uint16_t val){
	tmElements_t tm;
    //#ifndef _Time_h
   // time_t t=daysToTime_t(val);
   // #else
    time_t t=val*86400;
  //  #endif
    breakTime(t, tm);
    val=(uint8_t)GetDay(tm);
	return val;
}
uint8_t tVdmMsg::hoursfromseconds(double val) {
	uint8_t result = floor(val / 3600); 
	return result;

}
uint8_t tVdmMsg::minutesfromseconds(double val) {
	uint8_t hours = floor(val / 3600);
	uint16_t remaining = floor(val - (hours * 3600));
	uint8_t minutes = remaining / 60; 
	return minutes;

}
uint8_t tVdmMsg::secondsfromseconds(double val) {
	uint8_t hours = val / 3600;
	uint16_t remaining = val - (hours * 3600);
	uint8_t minutes = remaining / 60;
	uint8_t seconds = remaining - (minutes * 60);
	return seconds;

}
double tVdmMsg::RadToDeg(double val) { 
	return val*180.0/3.1415926535897932384626433832795; 
	}
double tVdmMsg::msToKnots(double val) { 
	return val*3600/1852.0; 
	}

//message 1,  AIS Class A Position Report
void tVdmMsg::Mesg1 (	
					uint8_t MessageID,
					uint8_t Repeat,
					uint32_t UserID,
					uint8_t NavStatus,
					double ROT,
					double SOG, 
					bool Accuracy, 
					double Longitude, 
					double Latitude, 
					double COG, 
					double Heading,
					uint8_t Seconds, 
					uint8_t smi,
				    bool RAIM
					){	
			/*			
              //Repeat  -> Repeat_conv
                  uint8_t Repeat_conv=(uint8_t)Repeat; 
              //Navstatus -> Navstatus_conv                      
                  uint8_t NavStatus_conv=(uint8_t)NavStatus;	
			*/
				  
              //Latitude -> Latitude_conv
                  int32_t Latitude_conv=(int32_t)round((Latitude/1e-05)*6);
              //Longitude -> Longitude_conv
                  int32_t Longitude_conv=(int32_t)round((Longitude/1e-05)*6);
 
              //COG -> COG_conv   
                  uint16_t COG_conv=(uint16_t)round(RadToDeg(COG/0.1));
              //SOG -> SOG_conv
                  uint16_t SOG_conv=(uint16_t)round(msToKnots(SOG)/0.1);
              //Heading -> Heading_conv
                  uint16_t Heading_conv=(uint16_t)round(RadToDeg(Heading));
              //ROT -> ROT_conv
                  int16_t ROT_conv=(int16_t)round(ROT/3.125E-05);
	
		Add4UByte( (uint32_t)0x00, 19); 
		Add1UByte( (uint8_t)RAIM, 1); 
		Add1UByte( 0, 3); 
		Add1UByte( smi, 2 ); 
		Add1UByte( Seconds, 6); 
		Add2UByte( Heading_conv, 9 ); 
		Add2UByte( COG_conv, 12); 
		Add4Byte( Latitude_conv, 27);
		Add4Byte( Longitude_conv,28);
		Add1UByte( (uint8_t)Accuracy, 1); 
		Add2UByte( SOG_conv, 10); 
		Add1Byte( ROT_conv, 8); 
		Add1UByte( NavStatus, 4); 
		Add4UByte( UserID, 30); 
		Add1UByte( Repeat, 2);
		Add1UByte( MessageID, 6); 
		
	
}
//message 5,   AIS Class A Static and Voyage Related Data
void tVdmMsg::Mesg5 (
					uint8_t MessageID,
					uint8_t Repeat,
					uint32_t UserID,
					uint8_t AISversion,
					uint32_t IMOnumber,
					char *Callsign, 
					char *Name, 
					uint8_t VesselType, 
					double Length, 
					double Beam,
					double PosRefStbd, 
					double PosRefBow,
					uint8_t GNSStype,
					uint16_t ETAdate,
					double ETAtime,
					double Draught,
					char *Destination,
					uint8_t DTE
					){
						
		//calculate the dimensions. 				
		uint16_t dim_bow,dim_stern;
		uint8_t dim_port,dim_star;
		dim_star=(uint8_t)round(PosRefStbd);
		dim_bow=(uint16_t)round(PosRefBow); 
		dim_port=(uint8_t)round(Beam) - dim_star; 
		dim_stern=(uint16_t)round(Length) - PosRefBow; 				
		
		//draught
		uint8_t dim_draught=(uint8_t)round(Draught * 10); 
			
		//for now,   do weird stuff with ETA date and time,  just to make it work. 
		uint8_t etamonth=MonthFromDate(ETAdate);
		uint8_t etaday=DayFromDate(ETAdate);
		uint8_t etahour=hoursfromseconds(ETAtime); 
		uint8_t etamin=minutesfromseconds(ETAtime); 

		Add1UByte( 0, 2); // stuff 2 bits for full package  
		Add1UByte( 0, 1); 
		Add1UByte (DTE, 1); 
		AddStr   (Destination,20);  
		Add1UByte (dim_draught, 8); 
		Add1UByte (etamin, 6);
		Add1UByte (etahour, 5);
		Add1UByte (etaday, 5);
		Add1UByte (etamonth, 4);
		Add1UByte (GNSStype, 4);
		Add1UByte (dim_star, 6);
		Add1UByte (dim_port, 6);
		Add2UByte (dim_stern ,9);
		Add2UByte (dim_bow, 9);
		Add1UByte (VesselType, 8);
		AddStr   (Name, 20);
		AddStr   (Callsign, 7);
		Add4UByte (IMOnumber,30); 
		Add1UByte (AISversion, 2); 
		Add4UByte (UserID, 30); 
		Add1UByte (Repeat, 2);
		Add1UByte (MessageID, 6); 
	
}
//message 9,   AIS SAR Aircraft Position Report
void tVdmMsg::Mesg9 (
					uint8_t MessageID,
					uint8_t Repeat,
					uint32_t UserID,
					uint16_t Altitude,
					uint16_t SOG, 
					uint8_t Accuracy, 
					uint32_t Longitude, 
					uint32_t Latitude, 
					uint16_t COG, 
					uint8_t Seconds, 
					uint8_t DTE, 
					uint8_t assigned,
				    uint8_t RAIM
					){		
		Add4Byte( (uint32_t)0x00, 20); 
		Add1Byte( RAIM, 1); 
		Add1Byte( assigned, 1); 
		Add1Byte( 0, 3); 
		Add1Byte( DTE, 1 ); 
		Add1Byte( 0xff, 8); 
		Add1Byte( Seconds, 6 ); 
		Add2Byte( COG, 12); 
		Add4Byte( Latitude, 27); 
		Add4Byte( Longitude,28); 
		Add1Byte( Accuracy, 1); 
		Add2Byte( SOG, 10); 
		Add2Byte( Altitude, 12); 		
		Add4Byte( UserID, 30); 
		Add1Byte( Repeat, 2);
		Add1Byte( MessageID, 6); 
	
	
}
//message 18,  AIS Class B Position Report
void tVdmMsg::Mesg18 (					
					uint8_t MessageID,
					uint8_t Repeat,
					uint32_t UserID,
					double SOG,
					bool Accuracy,
					double Longitude, 
					double Latitude, 
					double COG, 
					double Heading,
					uint8_t Seconds, 
					uint8_t Unit,
					bool Display, 
					bool DSC,
					bool Band,
					bool Msg22,
					uint8_t Mode,
					bool RAIM 
					){	

             //Latitude -> Latitude_conv
                  int32_t Latitude_conv=(int32_t)round((Latitude/1e-05)*6);
              //Longitude -> Longitude_conv
                  int32_t Longitude_conv=(int32_t)round((Longitude/1e-05)*6);


		//COG -> COG_conv   
		  uint16_t COG_conv=(uint16_t)round(RadToDeg(COG/0.1));
		//SOG -> SOG_conv
		  uint16_t SOG_conv=(uint16_t)round(msToKnots(SOG)/0.1);
		//Heading -> Heading_conv
		uint16_t Heading_conv;
		if (Heading == -1e9){
                Heading_conv=511;
        }else{
				Heading_conv=(uint16_t)round(RadToDeg(Heading));			
		}
		Add4UByte( (uint32_t)0x00, 20); 
		Add1UByte( (uint8_t)RAIM, 1); 
		Add1UByte( Mode, 1); 
		Add1UByte( (uint8_t)Msg22, 1); 
		Add1UByte( (uint8_t)Band, 1); 
		Add1UByte( (uint8_t)DSC, 1); 
		Add1UByte( (uint8_t)Display, 1);
		Add1UByte( Unit, 1); 
		Add1UByte( 0xff,2);   //regional application
		Add1UByte( Seconds, 6); 
		Add2UByte( Heading_conv, 9 ); 
		Add2UByte( COG_conv, 12); 
		Add4Byte( Latitude_conv, 27); 
		Add4Byte( Longitude_conv,28); 
		Add1UByte( (uint8_t)Accuracy, 1); 
		Add2UByte( SOG_conv, 10); 
		Add1UByte( 0xff, 8); //regional application
		Add4UByte( UserID, 30); 
		Add1UByte( Repeat, 2);
		Add1UByte( MessageID, 6); 
	
	
}
//message 24, AIS Class B "CS" Static Data Report ( part and b)
void tVdmMsg::Mesg24a (
					uint8_t MessageID,
					uint8_t Repeat,
					uint32_t UserID,
					char *Name
					){
		Add1Byte (0,8);  //spare				
		AddStr   (Name, 20);
		Add1Byte( 0, 2);  // part a
		Add4Byte (UserID, 30); 
		Add1Byte (Repeat, 2);
		Add1Byte (MessageID, 6);
}
void tVdmMsg::Mesg24b (
					uint8_t MessageID,
					uint8_t Repeat,
					uint32_t UserID,
					uint8_t VesselType, 
					char *Vendor, 
					char *Callsign, 
					double Length, 
					double Beam,
					double PosRefStbd, 
					double PosRefBow,
					uint32_t MothershipID
					){
						
		uint16_t dim_bow,dim_stern;
		uint8_t dim_port,dim_star;
		
		dim_star=(uint8_t)round(PosRefStbd);
		dim_bow=(uint16_t)round(PosRefBow); 
		dim_port=(uint8_t)round(Beam) - dim_star; 
		dim_stern=(uint16_t)round(Length) - PosRefBow; 
		Add1Byte( 0, 6);    
		Add4UByte (MothershipID, 30);
		Add1Byte (dim_star, 6);
		Add1Byte (dim_port, 6);
		Add2Byte (dim_stern ,9);
		Add2Byte (dim_bow, 9);
		AddStr   (Callsign, 7);
		AddStr   (Vendor, 7);
		Add1UByte (VesselType, 8);
		Add1UByte( 1, 2);    // part b
		Add4UByte (UserID, 30); 
		Add1UByte (Repeat, 2);
		Add1UByte (MessageID, 6);
}



void tVdmMsg::PrintBin8 (int8_t data, uint8_t direction){
	uint8_t request=(data & 0xff);
	PrintBin(data,8, direction);
	Serial.print("\n");
}
void tVdmMsg::PrintBinU8 (uint8_t data, uint8_t direction){
	PrintBin(data,8, direction);
	Serial.print("\n");
}
void tVdmMsg::PrintBin16 (int16_t data, uint8_t direction){
for (int i = 0; i < 2; i++){
		uint8_t request=((data >> (8-(i*8))) & 0xff);
		PrintBin(request,8, direction);
		Serial.print(" ");
	}
	Serial.print("\n");
}
void tVdmMsg::PrintBinU16 (uint16_t data, uint8_t direction){
for (int i = 0; i < 2; i++){
		uint8_t request=((data >> (8-(i*8))) & 0xff);
		PrintBin(request,8, direction);
		Serial.print(" ");
	}
	Serial.print("\n");
}
void tVdmMsg::PrintBin32 (int32_t data, uint8_t direction){
	Serial.print ((String) "Data("+ direction +")=>" + data);
	if (direction == 0){
		for (int i = 0 ; i< 4; i++){
			uint8_t request=((data >> (24-(i*8))) & 0xff);
			Serial.print ((String) " byte "+ i + "=" + request + " - ");
			PrintBin(request, 8,direction);
			Serial.print(" ");
		}
	}else{
		for (int i = 0 ; i< 4; i++){
			uint8_t request=((data >> (i*8)) & 0xff);
			Serial.print ((String) " byte "+ i + "=" + request + " - ");
			PrintBin(request,8, direction);
			Serial.print(" ");
		}		
	}
	Serial.print("<\n");
}
void tVdmMsg::PrintBinU32 (uint32_t data, uint8_t direction){
	Serial.print ("Data=>");
	for (int i = 0 ; i< 4; i++){
		uint8_t request=((data >> (32-(i*8))) & 0xff);
		Serial.print ((String) " request = " + request + " - ");
		PrintBin(request, direction);
		Serial.print(" ");
	}
	Serial.print("<\n");
}
