#include "log.h"
#include <stdio.h>
#include "..\utility\Utility.h"
#include <stdarg.h>
int WriteToLog(char * szFormat, ...)
{
	char   szBuffer [1024] ;
	va_list pArgList ;
	va_start (pArgList, szFormat) ;
	_vsnprintf( szBuffer, sizeof (szBuffer) / sizeof (char),
		szFormat, pArgList) ;
	va_end (pArgList) ;
	FILE* log;
	char inifile[256] = "";
	GetFullPath("Error.txt",inifile);
	log = fopen(inifile, "a+");
	if (log == NULL)
	{
		return -1;
	}
	fprintf(log, "%s\n", szBuffer);
	fclose(log);
	return 0;
}
int WriteToScreen(char * szFormat, ...)
{
	char   szBuffer [1024] ;
	va_list pArgList ;
	va_start (pArgList, szFormat) ;
	_vsnprintf( szBuffer, sizeof (szBuffer) / sizeof (char),
		szFormat, pArgList) ;
	va_end (pArgList) ;
	printf("%s\n", szBuffer);
	return 0;
}
void DumpBuffToFile(unsigned char* buf,int size)
{
	char fileName[256] = "";
	GetFullPath("logbuf.txt",fileName);

	FILE *fp = fopen(fileName, "a+");

	for (int i=0; i<size; i++)
	{			
		fprintf(fp,"%02X ", buf[i]);

		if (i % 16 == 7)
		{
			fprintf(fp, "\t");
		}

		if (i % 16 == 15)
		{
			fprintf(fp, "\r\n");				
		}		

	}		

	fprintf(fp,"\r\n");


	for (int i=0; i<size; i++)
	{			
		if (buf[i]>=32)// && buf[i]<=127)
		{
			fprintf(fp,"%c", buf[i]);						
		}
		else if (buf[i]==10)
		{
			fprintf(fp,"\\n");
		}
		else if (buf[i]==13)
		{
			fprintf(fp,"\\r");
		}
		else
		{
			fprintf(fp,".");
		}

	}	 
	fprintf(fp,"\r\n");

	fclose(fp);	
}
void DumpBuffToScreen(unsigned char* buf,int size)
{


	for (int i=0; i<size; i++)
	{			
		printf("%02X ", buf[i]);

		if (i % 16 == 7)
		{
			printf("\t");
		}

		if (i % 16 == 15)
		{
			printf("\r\n");				
		}		

	}		

	printf("\r\n");


	for (int i=0; i<size; i++)
	{			
		if (buf[i]>=32)// && buf[i]<=127)
		{
			printf("%c", buf[i]);						
		}
		else if (buf[i]==10)
		{
			printf("\\n");
		}
		else if (buf[i]==13)
		{
			printf("\\r");
		}
		else
		{
			printf(".");
		}

	}	 
	printf("\r\n");
}