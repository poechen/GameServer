//file name :inifile.h
#ifndef _DT_INI_H
#define _DT_INI_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "..\Utility\Utility.h"
#include "..\log\log.h"

#pragma warning(disable: 4267)

int isbreakchar(char ch);
int readline(FILE* fpt, char* linestr);
int trimspace(char* ori,char * trim);
int readitemfromline(char *line,char *item,char *value);
int isfindsection(char *line,char *section);
int getiniitem(char * inifile,char* section,char *item,char *value);
int setiniitem(char * inifile,char* section,char *item,char *value);
/*去掉首部的空格或者Tab键*/
void TrimLeftSpace(char *str);
/*去掉尾部的空格或者Tab键*/
int TrimRightSpace(char *str);
#endif