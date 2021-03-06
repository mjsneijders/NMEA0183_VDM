#ifndef _VDM_msg_H_
#define _VDM_msg_H_


#include <stdint.h>

#include <time.h>
#ifndef _Time_h
typedef tm tmElements_t;
#endif
//inline double RadToDeg(double v) { return N2kIsNA(v)?v:v*180.0/3.1415926535897932384626433832795; }
//inline double msToKnots(double v) { return N2kIsNA(v)?v:v*3600/1852.0; }


class tVdmMsg {
public:
	static const int MaxDataLen=200; 
	int Datalen;
	uint8_t Data[MaxDataLen];
	int DataPointer;
	uint8_t carryin; 
	uint8_t carrybits; 

	void Add0Byte();
	void Add1Byte(int8_t data, uint8_t length);
	void Add2Byte(int16_t data, uint8_t length);
	void Add4Byte(int32_t data, uint8_t length);
	void Add1UByte(uint8_t data, uint8_t length);
	void Add2UByte(uint16_t data, uint8_t length);
	void Add4UByte(uint32_t data, uint8_t length);
	void AddStr(const char *str, uint8_t length, char filler='@');
    void PrintBin8 (int8_t data, uint8_t direction=0); 
	void PrintBinU8 (uint8_t data, uint8_t direction=0); 
	void PrintBin16 (int16_t data, uint8_t direction=0);
	void PrintBinU16 (uint16_t data, uint8_t direction=0);
	void PrintBin32 (int32_t data, uint8_t direction=0);
	void PrintBinU32 (uint32_t data, uint8_t direction=0);
	tVdmMsg();
	void Print(); 
	void Get(char *message, uint8_t part=0); 

	void Mesg1 (	
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
					);
	void Mesg5 (	
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
					);
	void Mesg9 (
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
					); 
	void Mesg18(
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
				); 
	void Mesg24a(
					uint8_t MessageID,
					uint8_t Repeat,
					uint32_t UserID,
					char *Name
				); 
	void Mesg24b(
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
				); 
   #ifdef _Time_h
    static inline void SetYear(tmElements_t &TimeElements, int val) { TimeElements.Year=val-1970; } //
    static inline void SetMonth(tmElements_t &TimeElements, int val) { TimeElements.Month=val>0?val-1:val; }
    static inline void SetDay(tmElements_t &TimeElements, int val) { TimeElements.Day=val; }
    static inline void SetHour(tmElements_t &TimeElements, int val) { TimeElements.Hour=val; }
    static inline void SetMin(tmElements_t &TimeElements, int val) { TimeElements.Minute=val; }
    static inline void SetSec(tmElements_t &TimeElements, int val) { TimeElements.Second=val; }
    static inline int GetYear(const tmElements_t &TimeElements) { return TimeElements.Year+1970; }
    static inline int GetMonth(const tmElements_t &TimeElements) { return TimeElements.Month+1; }
    static inline int GetDay(const tmElements_t &TimeElements) { return TimeElements.Day; }
    static inline time_t makeTime(tmElements_t &TimeElements) { return ::makeTime(TimeElements); }
    static inline void breakTime(time_t time, tmElements_t &TimeElements) { ::breakTime(time,TimeElements); }
    #else
    static inline void SetYear(tmElements_t &TimeElements, int val) { TimeElements.tm_year=val-1900; } //
    static inline void SetMonth(tmElements_t &TimeElements, int val) { TimeElements.tm_mon=val>0?val-1:val; }
    static inline void SetDay(tmElements_t &TimeElements, int val) { TimeElements.tm_mday=val; }
    static inline void SetHour(tmElements_t &TimeElements, int val) { TimeElements.tm_hour=val; }
    static inline void SetMin(tmElements_t &TimeElements, int val) { TimeElements.tm_min=val; }
    static inline void SetSec(tmElements_t &TimeElements, int val) { TimeElements.tm_sec=val; }
    static inline int GetYear(const tmElements_t &TimeElements) { return TimeElements.tm_year+1900; }
    static inline int GetMonth(const tmElements_t &TimeElements) { return TimeElements.tm_mon+1; }
    static inline int GetDay(const tmElements_t &TimeElements) { return TimeElements.tm_mday; }
    static inline time_t makeTime(tmElements_t &TimeElements) { return mktime(&TimeElements); }
    static inline void breakTime(time_t time, tmElements_t &TimeElements) { TimeElements=*localtime(&time); }
    static time_t daysToTime_t(unsigned long val);
    #endif


private: 
	void EncodeByte( uint8_t result);
	void PrintBin(uint8_t var, uint8_t length, uint8_t direction = 0);
	uint8_t MonthFromDate(uint16_t val);
	uint8_t DayFromDate(uint16_t val);
	uint8_t hoursfromseconds(double val); 
	uint8_t minutesfromseconds(double val);
	uint8_t secondsfromseconds(double val);
	double RadToDeg(double v); 
    double msToKnots(double v); 
};




















#endif