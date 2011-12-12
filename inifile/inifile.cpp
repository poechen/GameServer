
#include <windows.h>
#include <io.h>
#include "inifile.h"
#define INI_FILE_MAX_ROW 1024
#ifdef WIN32
#define ACCESS _access
#else        ACCESS access
#endif        

int isbreakchar(char ch)
{
    if(ch==9)        //Tab Key
	return 0;
    if((ch<32)&&(ch>0))
	return 1;
    else
	return 0;
}
/*从文件中读取一行*/
int readline(FILE* fpt, char* linestr)
{
    int i,rv;
    char cc;
    char buf[INI_FILE_MAX_ROW];

    if(feof(fpt)){
	return 1;
    }
    for(i=0;i<INI_FILE_MAX_ROW;i++){
	rv=fread(&cc,1,1,fpt);
	if(rv<1)
	    break;
	if(isbreakchar(cc)==1)
	    break;
	buf[i]=cc;
    }
    buf[i]='\0';
    if(lstrlen(buf) == 0)
	return 1;
    lstrcpy(linestr,buf);
    return 0;        
}
/*去掉首部的空格或者Tab键*/
void TrimLeftSpace(char *str)
{
    char buf[INI_FILE_MAX_ROW];
    char *ptr;

    if(str==NULL)
	return;
    ptr=str;
    while(*ptr != '\0')
    {
	if((*ptr !=' ')&&(*ptr !='\t'))
	    break;
	ptr ++;
    }
    lstrcpy(buf,ptr);
    lstrcpy(str,buf);
}
/*去掉尾部的空格或者Tab键*/
int TrimRightSpace(char *str)
{
    int pos_end;
    char *ptr;
    if(str==NULL)
	return 1;
    ptr = str;
    pos_end = lstrlen(ptr);
    if(pos_end==0)
	return 1;
    while((*(ptr+pos_end-1) ==' ')||(*(ptr+pos_end-1) == '\t'))
    {
	if(pos_end == 0)
	    break;
	pos_end--;
    }
    *(str+pos_end)='\0';
    return 0;
}

int trimspace(char* ori,char * trim)
{
    char *ptrsrc,*ptrdes;

    ptrsrc = ori;
    ptrdes = trim;
    while(*ptrsrc != '\0')
    {
	if((*ptrsrc != ' ')&&(*ptrsrc!='\r'))
	{
	    *ptrdes = *ptrsrc;
	    ptrdes++;
	}
	ptrsrc ++;
    }
    *ptrdes = '\0';
    return 0;
}
/***********************
从一行中解析出item和value
参数：
line：输入
item：输入
value：输出
返回值：
0：成功
1：失败
*/ 
int readitemfromline(char *line,char *item,char *value)
{

    char linebuf[INI_FILE_MAX_ROW],leftbuf[INI_FILE_MAX_ROW],rightbuf[INI_FILE_MAX_ROW];
    char *ptr;

    if((line==NULL)||(item==NULL))
	return 1;
    lstrcpy(linebuf,line);
    TrimLeftSpace(linebuf);
    if(linebuf[0] == '#')/*表示次行为注释*/
	return 1;
    ptr = strchr(linebuf,'=');
    if(ptr==NULL)
	return 1;
    *ptr = '\0';
    /*把‘=’2边的字符串分开*/
    lstrcpy(leftbuf,linebuf);
    lstrcpy(rightbuf,ptr+1);
    /*去掉2边的空格*/
    TrimLeftSpace(leftbuf);
    TrimRightSpace(leftbuf);
    TrimLeftSpace(rightbuf);
    TrimRightSpace(rightbuf);
    if(lstrcmp(leftbuf,item)==0)/*找到item*/
    {
	lstrcpy(value,rightbuf);
	return 0;
    }
    else
	return 1;
}
/*查找一行字符串是否是有section，找到返回1，否则返回0*/
int isfindsection(char *line,char *section)
{
    char *ptr;
    char linebuf[INI_FILE_MAX_ROW];

    if((line==NULL)||(section==NULL))
	return 0;
    if(*line != '[')
	return 0;
    lstrcpy(linebuf,line+1);
    ptr = strchr(linebuf,']');
    if(ptr==NULL)
	return 0;
    *ptr = '\0';
    /*去掉‘[’‘]’中间字符串中的空格*/
    TrimLeftSpace(linebuf);
    TrimRightSpace(linebuf);
    if(lstrcmp(linebuf,section)==0)
	return 1;
    else
	return 0;
}
/***************************************
描述：
从ini文件中读取section中item的value
参数：
inifile:输入参数，ini文件名
section：输入参数，
item：输入参数
value：输出参数
返回值：
0：成功
1：失败
*****************************************/
int getiniitem(char * inifile,char* section,char *item,char *value)
{
    FILE *fp;
    int bOK;
    char line[INI_FILE_MAX_ROW];
    fp = fopen(inifile,"r");
    if(fp == NULL)
    {
	WriteToLog("open file error");
	return 1;
    }
    /*设置标志，从文件中查找section*/
    bOK = 0;
    while(!readline(fp,line))
    {
	if(isfindsection(line,section))
	{
	    bOK = 1;/*找到section*/
	    break;
	}
    }
    if(!bOK)/*没有找到section，返回1*/
	return 1;

    /*设置标志，从文件中读取item*/
    bOK = 0;
    while(!readline(fp,line))
    {
	//trimspace(line,linebuf);
	if(line[0] == '[')/* 找到'['，说明到达了下一section，没发现item */
	    break;
	if(readitemfromline(line,item,value) == 0)/*从一行字符串中查找item，并读取value*/
	{
	    bOK = 1;/*找到item*/
	    break;
	}
    }
    fclose(fp);
    return !bOK;
}
/**********************************
均为输入参数
返回：
0：操作成功
1：失败
**********************************/
int setiniitem(char * inifile,char* section,char *item,char *value)
{
    FILE *fp,*fptmp;
    int bfindsection,bok;
    char valueold[INI_FILE_MAX_ROW],line[INI_FILE_MAX_ROW];
    char tmpfile[128];
    char *ptr;
    /*判断ini文件是否存在*/
    if(ACCESS(inifile,0))/*不存在*/
    {
	fp = fopen(inifile,"w");
	if(fp == NULL)
	    return 1;
	fprintf(fp,"[%s]\n",section);
	fprintf(fp,"%s=%s\n",item,value);
	fclose(fp);
	return 0;
    }
    /*ini文件已经存在*/
    fp = fopen(inifile,"r");
    if(fp == NULL)
    {
	WriteToLog("open file for read error!");
	return 1;
    }
    /*新建立一个临时文件*/
    sprintf(tmpfile,"%s.tmp",inifile);
    fptmp = fopen(tmpfile,"w");
    if(fptmp == NULL)
    {
	WriteToLog("open file for write error!");
	return 1;
    }

    bfindsection = 0;/*是否发现section标志*/
    bok = 0;/*是否已写入标志*/
    while(!readline(fp,line))
    {        
	if(bok)/*已经把section中的item写入，把剩下的文件直接写到临时文件中*/
	{
	    fprintf(fptmp,"%s\n",line);//
	    continue;
	}
	if(!bfindsection)/*未找到section*/
	{
	    if(isfindsection(line,section))
	    {
		bfindsection = 1;
		fprintf(fptmp,"%s\n",line);
		continue;
	    }
	    else
	    {
		fprintf(fptmp,"%s\n",line);
		continue;
	    }
	}
	else/*找到了section*/
	{
	    if(line[0] != '[')/*未到达下一个section*/
	    {
		if(!readitemfromline(line,item,valueold))/*读取到旧的item*/
		{
		    ptr = strchr(line,'=');
		    *ptr='\0';
		    fprintf(fptmp,"%s=%s\n",line,value);/*更新新值*/
		    bok = 1;
		    continue;
		}
		else
		{
		    fprintf(fptmp,"%s\n",line);
		    continue;
		}
	    }
	    else/*到达下一个section*/
	    {
		fprintf(fptmp,"%s=%s\n",item,value);/*添加一个新item*/
		fprintf(fptmp,"%s\n",line);
		bok = 1;
	    }
	}
    }

    if(!bok&&!bfindsection)
    {
	fprintf(fptmp,"[%s]\n",section);
	fprintf(fptmp,"%s=%s\n",item,value);
    }
    if(!bok&&bfindsection)
    {
	fprintf(fptmp,"%s=%s\n",item,value);
    }
    fclose(fp);
    fclose(fptmp);
    remove(inifile);
    rename(tmpfile,inifile);
    return 0;
}
