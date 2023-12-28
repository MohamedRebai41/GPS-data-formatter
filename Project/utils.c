#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utils.h>
#include <globals.h>


int isEmpty(char* value)
{
	return strlen(value) == 0; 
}

int DataExtracted()
{
	return (!isEmpty(Time) && !isEmpty(Date) && !isEmpty(GPS_Pos_Valid) && !isEmpty(Nbr_Satellites) && !isEmpty(Altitude) && !isEmpty(Longitude));
}


void splitString(const char *inputString, char delimiter, char result[20][40], int *count) {
    int i, j = 0, k = 0;

    // Initialize result array
    for (i = 0; i < 20; i++) {
        result[i][0] = '\0';
    }

    // Split the input string and store in the array
    for (i = 0; inputString[i] != '\0' && j < 20; i++) {
        if (inputString[i] == delimiter) {
            if (i > 0) { // Avoid incrementing j at the start
                result[j][k] = '\0'; // End current string
                j++;
                k = 0;
                if (j >= 20) {
                    break; // Prevent exceeding the word limit
                }
            }
        } else {
            result[j][k] = inputString[i];
            k++;
            if (k >= 41) {
                result[j][k] = '\0'; // Ensure null termination
                break; // Prevent exceeding the string length limit
            }
        }
    }

    // Ensure the last word is null-terminated
    if (k < 99) {
        result[j][k] = '\0';
    }

    *count = j + 1; // Adjust count (account for the last word)
}

void formatTime(char *time)
{
    // hhmmss.sss format to "time: hh:mm:ss.sss"
    strcpy(Time,"\0");
    strncat(Time, time, 2);
    strcat(Time, ":");
    strncat(Time, time + 2, 2);
    strcat(Time, ":");
    strncat(Time, time + 4, 2);
    strcat(Time, ".");
    strncat(Time, time + 7, 3);
}

void formatLongitude(char *val, char*dir)
{
    // dddmm.mmmm format to "longitude: ddd.ddddddd E/W"
		strcpy(Longitude,"\0");
    strcat(Longitude, val);
    strcat(Longitude, " ");
    strcat(Longitude, dir);
}

void formatGPSPosition(char *pos)
{

    // 0 = invalid, 1 = GPS fix, 2 = DGPS fix
    if (strcmp(pos, "0") == 0)
    {
        strcpy(GPS_Pos_Valid, "invalid");
    }
    else if (strcmp(pos, "1") == 0)
    {
        strcpy(GPS_Pos_Valid, "GPS fix");
    }
    else if (strcmp(pos, "2") == 0)
    {
        strcpy(GPS_Pos_Valid, "DGPS fix");
    }
    else
    {
        strcpy(GPS_Pos_Valid, "unknown");
    }
}

void formatAltitude(char *altitude)
{
	  strcpy(Altitude,"\0");
    strcat(Altitude, altitude);
    strcat(Altitude, " m");
}

void formatNbSatellites(char* nbr)
{
	strcpy(Nbr_Satellites,nbr);
}



void formatDate(char *date)
{

    // ddmmyy format to "dd/mm/yy"
		strcpy(Date,"\0");
    strncat(Date, date, 2);
    strcat(Date, "/");
    strncat(Date, date + 2, 2);
    strcat(Date, "/");
    strncat(Date, date + 4, 2);

}

void extractData(char*sentence)
{
	char values[20][40];
	int count ;
	splitString(sentence,',', values, &count);
	if(!strncmp(values[0],"GPGGA", 5) && count > 9)
	{
		
		formatTime(isEmpty(values[1]) ? "unknown" : values[1]);
		formatLongitude(isEmpty(values[4]) ? "unknown" : values[4],values[5]);
		formatGPSPosition(isEmpty(values[6]) ? "unknown" : values[6]);
		formatAltitude(isEmpty(values[9]) ? "unknown" : values[9]);
	}
	else if(!strncmp(values[0],"GPGSV", 5) && count > 3)
	{
		formatNbSatellites(isEmpty(values[3]) ? "unknown" : values[3]);
	}
	else if(!strncmp(values[0],"GPRMC", 5) && count > 9)
	{
		formatDate(isEmpty(values[9]) ? "unknown" : values[9]);
	}
}





void buildTransmitMessage()
{
	Transmit_Buffer[0]='\0';
	strcat(Transmit_Buffer, "Date: ");
	strcat(Transmit_Buffer, Date);
	strcat(Transmit_Buffer, "\n");
	
	strcat(Transmit_Buffer, "Time: ");
	strcat(Transmit_Buffer, Time);
	strcat(Transmit_Buffer, "\n");
	
	strcat(Transmit_Buffer, "Longitude: ");
	strcat(Transmit_Buffer, Longitude);
	strcat(Transmit_Buffer, "\n");
	
	strcat(Transmit_Buffer, "GPS position: ");
	strcat(Transmit_Buffer, GPS_Pos_Valid);
	strcat(Transmit_Buffer, "\n");
	
	strcat(Transmit_Buffer, "Altitude: ");
	strcat(Transmit_Buffer, Altitude);
	strcat(Transmit_Buffer, "\n");
	
	strcat(Transmit_Buffer, "Number of sattelites in view: ");
	strcat(Transmit_Buffer, Nbr_Satellites);
	strcat(Transmit_Buffer, "\n");
		
}
