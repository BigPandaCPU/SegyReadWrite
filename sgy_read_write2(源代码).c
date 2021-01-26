#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include <memory.h> 
#include <windows.h>
/*
  This program is used for reed and write a seismic data from a file to another file.
  Auther:BigPanda
  Version:2.0
  E-mail:1196449387@qq.com
  Data:2016-6-8 Last-Edit:2016-6-12  Last-Edit:2016-12-15
  College of GeoScience
  China University of Petroleum(Beijing)
  PS:only support IBM-32-Float and IEEE-32-Float and with the real head fixed
     trace legnth format
*/
//////////////////////// 3200字节卷头的EBCDIC编码///////////////////////////////
	unsigned char E2A[256]={
	0, 1, 2, 3,156, 9,134,127,151,141,142, 11, 12, 13, 14, 15, 16, 17, 18, 19,157,133,
	8,135, 24, 25,146,143, 28, 29, 30, 31, 128,129,130,131,132,10,23,
	27,136,137,138,139,140, 5, 6, 7, 144,145, 22,147,148,149,150, 4,152,153,154,155,
	20, 21,158, 26, 32,160,161,162,163,164,165,166,167,168, 91, 46, 60, 40, 43, 33,
	38,169,170,171,172,173,174,175,176,177, 93, 36, 42, 41, 59, 94, 45,
	47,178,179,180,181,182,183,184,185,124, 44, 37, 95, 62, 63,
	186,187,188,189,190,191,192,193,194, 96, 58, 35, 64, 39, 61, 34, 195, 97, 98,
	99,100,101,102,103,104,105,196,197,198,199,200,201,
	202,106,107,108,109,110,111,112,113,114,203,204,205,206,207,
	208,209,126,115,116,117,118,119,120,121,122,210,211,212,213,214,215,
	216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231, 123, 65, 66, 67,
	68, 69, 70,71, 72, 73,232,233,234,235,236,237, 125, 74, 75, 76,
	77,78,79,80,81,82,238,239,240,241,242,243, 92,159, 83, 84, 85, 86, 87, 88, 89,
	90,244,245,246,247,248,249, 48,49, 50, 51, 52, 53, 54, 55, 56,
	57,250,251,252,253,254,255}; 
//////////////////////////////////////////////////////////////////////////////// 

/////////////////////////// 几个函数的申明 //////////////////////////////////
	////////////     大端转换为小端   ///////////////////////// 
	///////////  unsigned long 型大端转小端   ///////////////// 
unsigned long unsignedlongbig2little(unsigned long  data)
{
	union undata
	{
		unsigned long  i;
		unsigned char c[4];
	};
	union undata ut1,ut2;
	ut1.i=data;
	ut2.c[0]=ut1.c[3];
	ut2.c[1]=ut1.c[2];
	ut2.c[2]=ut1.c[1];
	ut2.c[3]=ut1.c[0];
	return (ut2.i);
}
 ///////////  long int 型大端转小端  ////////////// 
 long int longintbig2little(long int  data)
{
	union undata
	{
		long int i;
		unsigned char c[4];
	};
	union undata ut1,ut2;
	ut1.i=data;
	ut2.c[0]=ut1.c[3];
	ut2.c[1]=ut1.c[2];
	ut2.c[2]=ut1.c[1];
	ut2.c[3]=ut1.c[0];
	return (ut2.i);
}
///////////  short int 型大端转小端  ///////////// 
short int shortintbig2little(short int data)
{
	union undata
	{
		short int i;
		unsigned char c[2];
	};
	union undata ut1,ut2;
	ut1.i=data;
	ut2.c[0]=ut1.c[1];
	ut2.c[1]=ut1.c[0];
	return (ut2.i);
}
///////////  float 型大端转小端    ///////////////
float floatbig2little(float data)
{
	union undata
	{
		float i;
		unsigned char c[4];
	};
	union undata ut1,ut2;
    ut1.i=data;
	ut2.c[0]=ut1.c[3];
	ut2.c[1]=ut1.c[2];
	ut2.c[2]=ut1.c[1];
	ut2.c[3]=ut1.c[0];
	return (ut2.i);
}

/*void byteswap4(char *pData,int nNumber)
{
	int i;
	int nLength=nNumber*4;
	char a,b;
	for(i=0;i<nLength;i=i+4)
	{
		a=pData[i];
		pData[i]=pData[i+3];
		pData[i+3]=a;
		
		b=pData[i+1];
		pData[i+1]=pData[i+2];
		pData[i+2]=b;
	}
}*/
//////////////////////////////////////////////////////////////
	////////////////////  小端转化成大端  ////////////////////
long int longintlittle2big(long int data)
{
	union undata
	{
		long int i;
		unsigned char c[4];
	};
	union undata ut1,ut2;
	ut1.i=data;
	ut2.c[0]=ut1.c[3];
	ut2.c[1]=ut1.c[2];
	ut2.c[2]=ut1.c[1];
	ut2.c[3]=ut1.c[0];
	return (ut2.i);
}
/////////////////////////////////////////////////////////////////////////////

///////////////////////// ibm2ieee函数  /////////////////////////////
unsigned long ibm2ieee(unsigned long data)
{
	if( (data<<1)==0)
	{
		//若为IBM 浮点数，返回对应特定IEEE 二进制编码
		return data; 
	}
	// 获得IBM浮点数的S，符号部分
	unsigned long S_IBM_32=data&0x80000000;
	// 获得IBM浮点数的E，指数部分
	unsigned long E_IBM_32=data&0x7f000000;
	// 获得IBM浮点数的F，小数部分
	unsigned long F_IBM_32=data&0x00ffffff;
	
	// 获得IBM浮点数的F,小数部分
	unsigned long radix=0;
	unsigned long F_IEEE_32=F_IBM_32;
	while (radix<=3&&F_IEEE_32<0x01000000)
	{
		radix++;
		F_IEEE_32=F_IEEE_32<<1;
	} 
	//放置回IEEE类型F部分的相应位置
	F_IEEE_32=(F_IEEE_32-0x01000000)>>1;
	
	//获得IBM浮点数的E，指数部分
	// 开始计算，放置在正确位置
	unsigned long E_IEEE_32=(((E_IBM_32>>22)-130)-(radix-1))<<23;
	//是否溢出
	if(E_IEEE_32>0x7f800000)
	{
		return S_IBM_32|0x7f800000;
	} 
	if(E_IEEE_32<0x10000000)
	{
		return S_IBM_32;
	}
	else
	return S_IBM_32|E_IEEE_32|F_IEEE_32;	 
} 

////////////////////////////////////////////////////////////////////

//////////////////////////  几个结构体的申明 ////////////////////////////////
///////////////////  卷头二进制编码的400字节的结构体   ///////////////////// 
	struct BinarySegyHeaderStruct
	{

		int jobid; /* job identification number */
		int lino; /* line number (only one line per reel) */
		int reno; /* reel number */
		short int ntrpr; /* number of data traces per record */
		short int nart; /* number of auxiliary traces per record */
		short int hdt; /* sample interval in micro secs for this reel */
		short int dto; /* sameple for original field recording */
		short int hns; /* number of samples per trace for this reel */
		short int nso; /* number of samples per trace for original field recording */
		short int format; /* data sample format code:
		1 = floating point (4 bytes) 2 = fixed point (4 bytes) 3 = fixed point (2 bytes)
	    4 = fixed point w/gain code (4 bytes) */
		short int fold; /* CDP fold expected per CDP ensemble */
		short int tsort; /* trace sorting code: 1 = as recorded (no sorting) 2 = CDP ensemble
		3 = single fold continuous profile 4 = horizontally stacked */
		short int vscode; /* vertical sum code: 1 = no sum 2 = two sum ... N = N sum (N =
		32,767) */
		short int hsfs; /* sweep frequency at start */
		short int hsfe; /* sweep frequency at end */
		short int hslen; /* sweep length (ms) */
		short int hstyp; /* sweep type code:
		1 = linear 2 = parabolic 3 = exponential 4 = other */
		short int schn; /* trace number of sweep channel */
		short int hstas; /* sweep trace taper length (msec) at start if tapered (the taper
		starts at zero time and is effective for this length) */
		short int hstae; /* sweep trace taper length (msec) at end (the ending taper starts
		at sweep length minus the taper length at end) */
		short int htatyp; /* sweep trace taper type code: 1 = linear 2 = cos-squared 3 =
		other */
		short int hcorr; /* correlated data traces code:1 = no 2 = yes */
		short int bgrcv; /* binary gain recovered code: 1 = yes 2 = no */
		short int rcvm; /* amplitude recovery method code: 1 = none 2 = spherical divergence
		3 = AGC 4 = other */
		short int mfeet; /* measurement system code: 1 = meters 2 = feet */
		short int polyt; /* impulse signal polarity code:
		1 = increase in pressure or upward geophone case movement gives negative number on
		tape
		2 = increase in pressure or upward geophone case movement gives positive number on
		tape */
		short int vpol; /* vibratory polarity code: code seismic signal lags pilot by
		337.5 to 22.5 degrees 22.5 to 67.5 degrees 67.5 to 112.5 degrees
		112.5 to 157.5 degrees 157.5 to 202.5 degrees 202.5 to 247.5 degrees 247.5 to 292.5
		degrees 293.5 to 337.5 degrees */
		short int hunass[170]; /* unassigned */
	} ; /* 400 bytes is tightly packed */

	struct BinarySegyHeaderStruct FileHeader;
	///////////////////////////////////////////////////////////////////////////////////////
	 
	///////////////////  道头240字节结构体的申明   ///////////////////////////// 
	struct SegyTraceHdrStruct
	{ 
		int tracl; /* trace sequence number within line  1-4*/
		int tracr; /* trace sequence number within reel 5-8*/
		int fldr; /* field record number 9-12*/
		int tracf; /* trace number within field record 13-16*/
		int ep; /* energy source point number 17-20*/
		int cdp; /* CDP ensemble number 21-24*/
		int cdpt; /* trace number within CDP ensemble 25-26*/
		short int trid; /* trace identification code:
		1 = seismic data 2 = dead 3 = dummy 4 = time break 5 = uphole 6 = sweep 7 = timing
		8 = water break 9---, N = optional use (N = 32,767) Following are CWP id flags: 9
		= autocorrelation 10 = Fourier transformed - no packing
		xr[0],xi[0], ..., xr[N-1],xi[N-1]
		11 = Fourier transformed - unpacked Nyquist xr[0],xi[0],...,xr[N/2],xi[N/2] 12 =
		Fourier transformed - packed Nyquist
		even N:
		xr[0],xr[N/2],xr[1],xi[1], ..., xr[N/2 -1],xi[N/2 -1] (note the exceptional second
		entry)
		odd N:
		xr[0],xr[(N-1)/2],xr[1],xi[1], ..., xr[(N-1)/2 -1],xi[(N-1)/2 -1], xi[(N-1)/2]
		(note the exceptional second & last entries) 13 = Complex signal in the time domain
		xr[0],xi[0], ..., xr[N-1],xi[N-1] 14 = Fourier transformed -amplitude/phase
		a[0],p[0], ..., a[N-1],p[N-1] 15 = Complex time signal - amplitude/phase
		a[0],p[0], ..., a[N-1],p[N-1] 16 = Real part of complex trace from 0 to Nyquist 17
		= Imag part of complex trace from 0 to Nyquist 18 = Amplitude of complex trace from
		0 to Nyquist
		19 = Phase of complex trace from 0 to Nyquist 21 = Wavenumber time domain (k-t) 22
		= Wavenumber frequency (k-omega) 30 = Depth-Range (z-x) traces
		101 = Seismic data packed to bytes (by supack1) 102 = Seismic data packed to 2 bytes
		(by supack2) 200 = GPR data
		*/
		short int nvs; /* number of vertically summed traces (see vscode in reel header
		structure) */
		short int nhs; /* number of horizontally summed traces (see vscode in reel header
		structure) */
		short int duse; /* data use: 1 = production 2 = test */
		int offset; /* distance from source point to receiver group (negative if
		opposite to direction in which the line was shot) */
		int gelev; /* receiver group elevation from sea level (above sea level is
		positive) */
		int selev; /* source elevation from sea level (a bove sea level is positive)
		*/
		int sdepth; /* source depth below surface (positive) */
		int gdel; /* datum elevation at receiver group */
		int sdel; /* datum elevation at source */
		int swdep; /* water depth at source */
		int gwdep; /* water depth at receiver group */
		short int scalel; /* scale factor for previous 7 entries
		with value plus or minus 10 to the power 0, 1, 2, 3, or 4 (if positive, multiply,
		if negative divide) */
		short int scalco; /* scale factor for next 4 entries with value plus or minus 10
		to the power 0, 1, 2, 3, or 4 (if positive, multiply, if negative divide) */
		int sx; /* X source coordinate */
		int sy; /* Y source coordinate */
		int gx; /* X group coordinate */
		int gy; /* Y group coordinate */
		short int counit; /* coordinate units code:
		for previous four entries 1 = length (meters or feet) 2 = seconds of arc (in this
		case, the X values are longitude and the Y values are latitude, a positive value
		designates the number of seconds east of Greenwich or north of the equator */
		short int wevel; /* weathering velocity */
		short int swevel; /* subweathering velocity */
		short int sut; /* uphole time at source */
		short int gut; /* uphole time at receiver group */
		short int sstat; /* source static correction */
		short int gstat; /* group static correction */
		short int tstat; /* total static applied */
		short int laga; /* lag time A, time in ms between end of 240-
		byte trace identification header and time break, positive if time break occurs
		after end of header, time break is defined as the initiation pulse which maybe
		recorded on an auxiliary trace or as otherwise specified by the recording system
		*/
		short int lagb; /* lag time B, time in ms between the time break and the initiation
		time of the energy source, may be positive or negative */
		short int delrt; /* delay recording time, time in ms between initiation time of energy
		source and time when recording of data samples begins (for deep water work if
		recording does not start at zero time) */
		short int muts; /* mute time--start */
		short int mute; /* mute time--end */
		unsigned short int ns; /* number of samples in this trace */
		unsigned short int dt; /* sample interval; in micro-seconds */
		short int gain; /* gain type of field instruments code: 1 = fixed 2 = binary 3 =
		floating point 4 ---- N = optional use */
		short int igc; /* instrument gain constant */
		short int igi; /* instrument early or initial gain */
		short int corr; /* correlated: 1 = no 2 = yes */
		short int sfs; /* sweep frequency at start */
		short int sfe; /* sweep frequency at end */
		short int slen; /* sweep length in ms */
		short int styp; /* sweep type code: 1 = linear 2 = parabolic 3 = exponential 4 =
		other */
		short int stas; /* sweep trace taper length at start in ms */
		short int stae; /* sweep trace taper length at end in ms */
		short int tatyp; /* taper type: 1=linear, 2=cos^2, 3=other */
		short int afilf; /* alias filter frequency if used */
		short int afils; /* alias filter slope */
		short int nofilf; /* notch filter frequency if used */
		short int nofils; /* notch filter slope */
		short int lcf; /* low cut frequency if used */
		short int hcf; /* high cut frequncy if used */
		short int lcs; /* low cut slope */
		short int hcs; /* high cut slope */
		short int year; /* year data recorded */
		short int day; /* day of year */
		short int hour; /* hour of day (24 hour clock) */
		short int minute; /* minute of hour */
		short int sec; /* second of minute */
		short int timbas; /* time basis code:1 = local 2 = GMT 3 = other */
		short int trwf; /* trace weighting factor, defined as 1/2^N volts for the least
		sigificant bit */
		short int grnors; /* geophone group number of roll switch position one */
		short int grnofr; /* geophone group number of trace one within original field record
		*/
		short int grnlof; /* geophone group number of last trace within original field record
		*/
		short int gaps; /* gap size (total number of groups dropped) */
		short int otrav; /* overtravel taper code: 1 = down (or behind) 2 = up (or ahead)
		*/
		int xcdp;/* X coordinate of ensample(cdp) position of this trace  181-184 */
        int ycdp;/* Y coordinate of ensample(cdp) position of this trace 185-188 */
		short int unass[22]; /* unassigned -- for optional info */
	};
	struct SegyTraceHdrStruct TraceHeader;
	
////////////////////////////////////////////////////////////////////////////////////////////


int main()
{
	FILE *fp,*fp_write;
    char filename[200],filenameout[200];
    char f3200[3200];
    char file[200];
    //float *tracedata;
    printf("\n");
    printf("/*********  本程序的功能是提取地震数据中某一道的数据输出为txt  **************/\n");
    printf("/*******************   written by BigPanda 2016/6/21   ************************/\n");
    //printf("/*********************   送给静静师姐的毕业礼物   ***************************/\n");
    printf("\n");

	printf("Input Sgy File Name:\n");
	scanf("%s",filename); //输入文件名


		
	////////////////  计算出该文件中的字节数  //////////////////
    struct _stati64 buf;
    if(_stati64(filename,&buf)<0)
    {
    	printf("Failed to open the file!\n");
    }
    long long sumbyte;
	sumbyte=(long long)buf.st_size;
	//printf("Sumbyte is %lld\n",sumbyte);
    ////////////////////////////////////////////////////////////
	
	////////////////////////////////////////////////////////////
	//////////// 打开需要读入数据的文件 //////////  
	fp=fopen(filename,"rb");

	
	////////////////////////////////////////////////////////////
	
	
	///////////     读取3200个字节EBCDIC编码的卷头信息  ///////////////
	fread(f3200,3200,1,fp);
	

    //////////     读取400字节的二进制编码的卷头信息   ////////////////
	/////////      大端存储的，显示时需要转换成小端    ////////////////
	////////   此处需要提取两个参数，每到的采样点数和数据类型 /////////
	fread(&FileHeader,400,1,fp);
    
    
    ////////// 判断大小端 //////////////
    short int endform;
    endform=FileHeader.format;
    //printf("endform=%d\n",endform);
    
    //exit(0);
	if(endform>0&&endform<6)
		endform=0;///ennform 表示数据存储字节序，0代表小端存储，1代表大端存储 
	else
		endform=1;
	
	//printf("endform=%d\n",endform);
	//exit(0);
    //////////////////////////////////// 
 
 
    ////////////  获取每道采样点数 //////////// 
	short int numsam;
	if(endform==1)
		numsam=shortintbig2little(FileHeader.hns);//获取每道的采样点数
	else
		numsam=FileHeader.hns;
	//printf("numsam=%d\n",numsam);
	//////////////////////////////////////////////
	
	///////////  获取每道采样间隔 //////////// 
    short int dt;//	
    if(endform==1)
		dt=shortintbig2little(FileHeader.hdt);
	else
		dt=FileHeader.hdt; 
    dt=dt/1000;//时间单位转换为ms 
	short int datatype,databyte;
	if(endform==1)
		datatype=shortintbig2little(FileHeader.format);//获取数据记录类型
    else
    	datatype=FileHeader.format;
    	
    /////////////////////   求出数据记录的字节数   //////////////
	if(datatype==1)
		{databyte=4;}//printf("Data format is 4-byte IBM float\n");}
 	else if(datatype==2)
		{
			databyte=4;
			printf("\nThis program didn't support this format data!\n");
			printf("Press anykey to exit!\n");
			scanf("%s",&file);
			
		}
 	else if(datatype==3)
		{
			databyte=2;
			printf("\nThis program didn't support this format data!\n");
			printf("Press anykey to exit!\n");
			scanf("%s",&file);
		}//,printf("Data format is 2-byte two complement int\n");}
 	else if(datatype==4)
		{
			databyte=4;
			printf("\nThis program didn't support this format data!\n");
			printf("Press anykey to exit!\n");
			scanf("%s",&file);
		}//,printf("Data format is 4-byte fixed point with gain\n");}
 	else if(datatype==5)
		{databyte=4;}//,printf("Data format is 4-byte IEEE float\n");}
 	else
		{
			databyte=0;
			printf("\nThis program didn't support this format data!\n");
			printf("Press anykey to exit!\n");
			scanf("%s",&file);
		}//printf("Data format faulse\n"),exit(0);}
    ////////////////////////////////////////////////////////////
    /////////// 输出的数据格式为 IEEE bigend 型 ////////////////////////////////
    //if(datatype==1)/// 若前面以判断数据存为为大端，endform=1；则此处需要转换回去 
		FileHeader.format=shortintbig2little(5);
	//else
		//FileHeader.format=5; 
	//////////////////////////////////////////////////////////////////////////// 
    //fwrite(&FileHeader,400,1,fp_write);

	///////////////////////  计算总道数  ///////////////////////
	long sumtrace;
	sumtrace=(sumbyte-3600)/(240+numsam*databyte);
    //printf("Sumtrace=%ld\n",sumtrace);
    //exit(0);
    ///////////////////////////////////////////////////////////
    
    ///////////// 输入你要输出的某一道cdp的坐标  /////////////
	long cdpnum_out;///记录需要输出的某个cdp的坐标 
	int Xline,Inline;//需要输出的Xline和Inline测线 
	int Xline_out,Inline_out;//
	printf("\n");
	printf("Please input the XLine:");
	scanf("%d",&Xline_out);
	printf("Please input the InLine:");
	scanf("%d",&Inline_out);
	printf("\n");
	//printf("Xline=%d,Inline=%d",Xline,Inline);
	///////////////////////////////////////////////////////// 
    
    
    ///////////////////    对每一道进行处理  /////////////////// 
    
    int j;
	long i;
    long xcdp; 
    
    unsigned long unsignedlong_p;
    
    float *tracedata_out;
	tracedata_out=(float *)malloc(numsam*sizeof(float)); 
	
    unsigned long *tracedata;
    tracedata=(unsigned long *)malloc(numsam*sizeof(unsigned long));
    
	for( i=0;i<sumtrace;i++)
    {
    	fread(&TraceHeader,240,1,fp); //读取每一道的道头 	 
    	
    	
    	if(endform==1)
    		Xline=longintbig2little(TraceHeader.fldr);
		else
			Xline=TraceHeader.fldr;
		
		if(endform==1)
    		Inline=longintbig2little(TraceHeader.tracf);
		else
			Inline=TraceHeader.tracf;
		
		//printf("Xline=%d,Inline=%d\n",Xline,Inline);
			
			//printf("cdpnum=%d\n",cdpnum);
		   	
    	fread(tracedata,databyte,numsam,fp);//读取每一道的数据 
    	
		////////////////////////////////////////////////////////
    	
    	
    	if( (Xline_out==Xline) && (Inline_out)==Inline)
    	{
    			
			////////// 打开需要写入数据的文件  /////////// 
			//filenameout=strcat(filename,"_out.sgy")
			
			char ss[100];
			sprintf(ss,"Xline_%d_Inline_%d.txt",Xline_out,Inline_out); 
			fp_write=fopen(ss,"w");
    		//////  将IBM数据转换为IEEE数据  ///////
			for(j=0;j<numsam;j++)
			{	if(endform==1)
				{
					tracedata[j]=unsignedlongbig2little(tracedata[j]);
				}
				if(datatype==1)///判断数据存储类型，如果为IBM型，就进行IBM转IEEE 
				{
					unsignedlong_p=ibm2ieee(tracedata[j]);
					tracedata_out[j]=*(float*)&unsignedlong_p;
				}
				else if(datatype==5)//// 若数据存储类型为IEEE型，不用转换 
				{
					unsignedlong_p=tracedata[j];
					tracedata_out[j]=*(float*)&unsignedlong_p; 
				}
				else
				{
					printf("This program didn't support this data format!\n");
					exit(0); 
				} 	
		   }
			
			fprintf(fp_write,"%s\n","time:ms  value:");
			
			for(j=0;j<numsam;j++)
			{
				fprintf(fp_write,"%4d     %f\n",j*dt,tracedata_out[j]);
			}
			
			printf("Succeed to get the data!\n");
			//printf("\nPlease input anykey to exit!");
			//scanf("%s",&file);
			Sleep(2000);
			
			fclose(fp);
			fclose(fp_write);
			exit(0);
        }
       	if(i==sumtrace-1)
		{
			printf("The Xline and Inline you put is out of this data,please check your data!\n\n");
			//system("pause"); 
			//getchar();
			//printf("\nPlease input anykey to exit!");
			//scanf("%s",&file); 
			Sleep(2000);
		}	
    }
    fclose(fp);
}


