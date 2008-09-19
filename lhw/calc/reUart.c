#include "./UART.h"
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#define RX_DATA_M 1
#define RX_DATA_T 2
#define RX_DATA_B 4
#define RX_DATA_L 8
#define RX_DATA_R 16
#define SENSER_DATA 32
#define DISCON_MSG_T 64
#define DISCON_MSG_B 65
#define DISCON_MSG_L 66
#define DISCON_MSG_R 67


#define DEBUG 1

#define FIRST 16
#define SECOND 8
#define THIRD 4
#define FOURTH 2
#define FIFTH 1	

char 	t_check[15][15]={0,};
char 	b_check[15][15]={0,};
char 	l_check[15][15]={0,};
char 	r_check[15][15]={0,};

int 	t_check_index=0;
int 	b_check_index=0;
int 	r_check_index=0;
int 	l_check_index=0;
int 	t_ok_cnt[15]={0,};
int 	b_ok_cnt[15]={0,};
int 	r_ok_cnt[15]={0,};
int 	l_ok_cnt[15]={0,};

int 	flag;
int 	t_flag;
int 	b_flag;
int 	l_flag;
int 	r_flag;


int t_check_Ip(void)
{
	int a;
	int b;
	int max=0;
	int max_index;

	printf("t_check==\n");
	for(a=0;a<15;a++)
	{
		for(b=0;b<15;b++)
		{
			printf("%c",t_check[a][b]);
		}
		printf("\n");	
	}

	for(a=0;a<15;a++)
	{
		for(b=0;b<15;b++)
		{
			if(strncmp(t_check[a],t_check[b],15)==0)
				t_ok_cnt[a]++;
		}	
	}

	printf("t_ok_cnt=");
	for(a=0;a<15;a++)
		printf("%d",t_ok_cnt[a]);
	printf("\n");

	for(b=0;b<15;b++)
	{
		if(max<t_ok_cnt[b])
		{
			max=t_ok_cnt[b];		
			max_index=b;
		}
	}

	printf("max_index=%d",max_index);
	return max_index;
}

int b_check_Ip(void)
{
	int a;
	int b;
	int max=0;
	int max_index;

	printf("b_check==\n");
	for(a=0;a<15;a++)
	{
		for(b=0;b<15;b++)
		{
			printf("%c",b_check[a][b]);
		}
		printf("\n");	
	}

	for(a=0;a<15;a++)
	{
		for(b=0;b<15;b++)
		{
			if(strncmp(b_check[a],b_check[b],15)==0)
				b_ok_cnt[a]++;
		}	
	}

	printf("b_ok_cnt=");
	for(a=0;a<15;a++)
		printf("%d",b_ok_cnt[a]);
	printf("\n");

	for(b=0;b<15;b++)
	{
		if(max<b_ok_cnt[b])
		{
			max=b_ok_cnt[b];		
			max_index=b;
		}
	}

	printf("max_index=%d",max_index);
	return max_index;
}
int r_check_Ip(void)
{
	int a;
	int b;
	int max=0;
	int max_index;

	printf("r_check==\n");
	for(a=0;a<15;a++)
	{
		for(b=0;b<15;b++)
		{
			printf("%c",r_check[a][b]);
		}
		printf("\n");	
	}

	for(a=0;a<15;a++)
	{
		for(b=0;b<15;b++)
		{
			if(strncmp(r_check[a],r_check[b],15)==0)
				r_ok_cnt[a]++;
		}	
	}

	printf("r_ok_cnt=");
	for(a=0;a<15;a++)
		printf("%d",r_ok_cnt[a]);
	printf("\n");

	for(b=0;b<15;b++)
	{
		if(max<r_ok_cnt[b])
		{
			max=r_ok_cnt[b];		
			max_index=b;
		}
	}

	printf("max_index=%d",max_index);
	return max_index;
}

int l_check_Ip(void)
{
	int a;
	int b;
	int max=0;
	int max_index;

	printf("l_check==\n");
	for(a=0;a<15;a++)
	{
		for(b=0;b<15;b++)
		{
			printf("%c",l_check[a][b]);
		}
		printf("\n");	
	}

	for(a=0;a<15;a++)
	{
		for(b=0;b<15;b++)
		{
			if(strncmp(l_check[a],l_check[b],15)==0)
				l_ok_cnt[a]++;
		}	
	}

	printf("l_ok_cnt=");
	for(a=0;a<15;a++)
		printf("%d",l_ok_cnt[a]);
	printf("\n");

	for(b=0;b<15;b++)
	{
		if(max<l_ok_cnt[b])
		{
			max=l_ok_cnt[b];		
			max_index=b;
		}
	}

	printf("max_index=%d",max_index);
	return max_index;
}




void cal_process_start(void)
{
	system("/root/calc_proc");
}

int main( void)
{  
	int 	res;
	int 	i,j;
	int 	rxbuf_cnt=0;
	int 	ip;
	int   fd;
	int   cnt;
	char 	IsConnectBuf;
	char  buf[5]={0,};
	char 	s_rxdata[15]={0,};
	char 	t_rxdata[15]={0,};
	char 	b_rxdata[15]={0,};
	char 	l_rxdata[15]={0,};
	char 	r_rxdata[15]={0,};



	char 	check0[3]={'0','0','0'};
	char 	check1[3]={'1','5','5'};
	char 	check2[3]={'1','5','6'};
	char 	check3[3]={'1','5','7'};
	char 	check4[3]={'1','5','8'};

	int 	senser=0;
	int 	t_cnt=0;
	int 	b_cnt=0;
	int 	l_cnt=0;
	int 	r_cnt=0;
	int 	m_cnt=0;

	int 	place=0;
	int 	poll_state;
	
	int 	index;

	fd=initUart();
	initPoll(fd);

	while ( 1)
	{
		poll_state = poll(                               // poll()을 호출하여 event 발생 여부 확인     
				(struct pollfd*)&poll_events, // event 등록 변수
				1,  // 체크할 pollfd 개수
				1000   // time out 시간
				);
		if ( 0 < poll_state)                             // 발생한 event 가 있음
		{     
			if ( poll_events.revents & POLLIN)            // event 가 자료 수신?
			{

				memset(buf,0,sizeof(buf));

				cnt = read( fd, buf, sizeof(buf));
				//					printf("cnt=%d",cnt);
				switch(buf[0])
				{
//					case 's': flag=SENSER_DATA;
//						  s_rxdata[0]=buf[1];	s_rxdata[1]=buf[2];	s_rxdata[2]=buf[3];
//						  break;
					case 't':
						  flag=RX_DATA_T;
						  if(buf[1]=='0')	
						  {
							  if(strncmp(buf+2,check0,3)!=0 && strncmp(buf+2,check1,3)!=0 && strncmp(buf+2,check2,3)!=0 && strncmp(buf+2,check3,3)!=0 && strncmp(buf+2,check4,3)!=0 ) break;	
							  t_flag=0;
							  t_rxdata[0]=buf[2]; t_rxdata[1]=buf[3]; t_rxdata[2]=buf[4];
							  t_flag|=FIRST;	

						  }
						  else if(buf[1]=='1')	
						  {
							  if(strncmp(buf+2,check0,3)!=0 && strncmp(buf+2,check1,3)!=0 && strncmp(buf+2,check2,3)!=0 && strncmp(buf+2,check3,3)!=0 && strncmp(buf+2,check4,3)!=0 ) break;	
							  t_rxdata[3]=buf[2]; t_rxdata[4]=buf[3]; t_rxdata[5]=buf[4];
							  t_flag|=SECOND;

						  }
						  else if(buf[1]=='2')	
						  {
							  if(strncmp(buf+2,check0,3)!=0 && strncmp(buf+2,check1,3)!=0 && strncmp(buf+2,check2,3)!=0 && strncmp(buf+2,check3,3)!=0 && strncmp(buf+2,check4,3)!=0 ) break;	
							  t_rxdata[6]=buf[2]; t_rxdata[7]=buf[3]; t_rxdata[8]=buf[4];
							  t_flag|=THIRD;


						  }
						  else if(buf[1]=='3')	
						  {
							  if(strncmp(buf+2,check0,3)!=0 && strncmp(buf+2,check1,3)!=0 && strncmp(buf+2,check2,3)!=0 && strncmp(buf+2,check3,3)!=0 && strncmp(buf+2,check4,3)!=0 ) break;	
							  t_rxdata[9]=buf[2]; t_rxdata[10]=buf[3]; t_rxdata[11]=buf[4];
							  t_flag|=FOURTH;

						  }
						  else if(buf[1]=='4')	
						  {
							  if(strncmp(buf+2,check0,3)!=0 && strncmp(buf+2,check1,3)!=0 && strncmp(buf+2,check2,3)!=0 && strncmp(buf+2,check3,3)!=0 && strncmp(buf+2,check4,3)!=0 ) break;	
							  t_rxdata[12]=buf[2]; t_rxdata[13]=buf[3]; t_rxdata[14]=buf[4];
							  t_flag|=FIFTH;
						  }
						  //										t_cnt++;
						  break;
					case 'b':
						  flag=RX_DATA_B;
						  if(buf[1]=='0')	
						  {
							  if(strncmp(buf+2,check0,3)!=0 && strncmp(buf+2,check1,3)!=0 && strncmp(buf+2,check2,3)!=0 && strncmp(buf+2,check3,3)!=0 && strncmp(buf+2,check4,3)!=0 ) break;	
							  b_flag=0; 
							  b_rxdata[0]=buf[2]; b_rxdata[1]=buf[3]; b_rxdata[2]=buf[4];
							  b_flag|=FIRST;	

						  }
						  else if(buf[1]=='1')	
						  {
							  if(strncmp(buf+2,check0,3)!=0 && strncmp(buf+2,check1,3)!=0 && strncmp(buf+2,check2,3)!=0 && strncmp(buf+2,check3,3)!=0 && strncmp(buf+2,check4,3)!=0 ) break;	
							  b_rxdata[3]=buf[2]; b_rxdata[4]=buf[3]; b_rxdata[5]=buf[4];
							  b_flag|=SECOND;	

						  }
						  else if(buf[1]=='2')	
						  {
							  if(strncmp(buf+2,check0,3)!=0 && strncmp(buf+2,check1,3)!=0 && strncmp(buf+2,check2,3)!=0 && strncmp(buf+2,check3,3)!=0 && strncmp(buf+2,check4,3)!=0 ) break;	
							  b_rxdata[6]=buf[2]; b_rxdata[7]=buf[3]; b_rxdata[8]=buf[4];
							  b_flag|=THIRD;	

						  }
						  else if(buf[1]=='3')	
						  {
							  if(strncmp(buf+2,check0,3)!=0 && strncmp(buf+2,check1,3)!=0 && strncmp(buf+2,check2,3)!=0 && strncmp(buf+2,check3,3)!=0 && strncmp(buf+2,check4,3)!=0 ) break;	
							  b_rxdata[9]=buf[2]; b_rxdata[10]=buf[3]; b_rxdata[11]=buf[4];
							  b_flag|=FOURTH;	

						  }
						  else if(buf[1]=='4')	
						  {
							  if(strncmp(buf+2,check0,3)!=0 && strncmp(buf+2,check1,3)!=0 && strncmp(buf+2,check2,3)!=0 && strncmp(buf+2,check3,3)!=0 && strncmp(buf+2,check4,3)!=0 ) break;	
							  b_rxdata[12]=buf[2]; b_rxdata[13]=buf[3]; b_rxdata[14]=buf[4];
							  b_flag|=FIFTH;	

						  }
						  //										b_cnt++;
						  break;
					case 'l':
						  flag=RX_DATA_L;
						  if(buf[1]=='0')	
						  {
							  if(strncmp(buf+2,check0,3)!=0 && strncmp(buf+2,check1,3)!=0 && strncmp(buf+2,check2,3)!=0 && strncmp(buf+2,check3,3)!=0 && strncmp(buf+2,check4,3)!=0 ) break;	
							  l_flag=0;
							  l_rxdata[0]=buf[2]; l_rxdata[1]=buf[3]; l_rxdata[2]=buf[4];
							  l_flag|=FIRST;	

						  }
						  else if(buf[1]=='1')	
						  {
							  if(strncmp(buf+2,check0,3)!=0 && strncmp(buf+2,check1,3)!=0 && strncmp(buf+2,check2,3)!=0 && strncmp(buf+2,check3,3)!=0 && strncmp(buf+2,check4,3)!=0 ) break;	
							  l_rxdata[3]=buf[2]; l_rxdata[4]=buf[3]; l_rxdata[5]=buf[4];
							  l_flag|=SECOND;	

						  }
						  else if(buf[1]=='2')	
						  {
							  if(strncmp(buf+2,check0,3)!=0 && strncmp(buf+2,check1,3)!=0 && strncmp(buf+2,check2,3)!=0 && strncmp(buf+2,check3,3)!=0 && strncmp(buf+2,check4,3)!=0 ) break;	
							  l_rxdata[6]=buf[2]; l_rxdata[7]=buf[3]; l_rxdata[8]=buf[4];
							  l_flag|=THIRD;	

						  }
						  else if(buf[1]=='3')	
						  {
							  if(strncmp(buf+2,check0,3)!=0 && strncmp(buf+2,check1,3)!=0 && strncmp(buf+2,check2,3)!=0 && strncmp(buf+2,check3,3)!=0 && strncmp(buf+2,check4,3)!=0 ) break;	
							  l_rxdata[9]=buf[2]; l_rxdata[10]=buf[3]; l_rxdata[11]=buf[4];
							  l_flag|=FOURTH;	

						  }
						  else if(buf[1]=='4')	
						  {
							  if(strncmp(buf+2,check0,3)!=0 && strncmp(buf+2,check1,3)!=0 && strncmp(buf+2,check2,3)!=0 && strncmp(buf+2,check3,3)!=0 && strncmp(buf+2,check4,3)!=0 ) break;	
							  l_rxdata[12]=buf[2]; l_rxdata[13]=buf[3]; l_rxdata[14]=buf[4];
							  l_flag|=FIFTH;	

						  }
						  //										l_cnt++;
						  break;
					case 'r':
						  flag=RX_DATA_R;
						  if(buf[1]=='0')	
						  {
							  if(strncmp(buf+2,check0,3)!=0 && strncmp(buf+2,check1,3)!=0 && strncmp(buf+2,check2,3)!=0 && strncmp(buf+2,check3,3)!=0 && strncmp(buf+2,check4,3)!=0 ) break;	
							  r_flag=0;
							  r_rxdata[0]=buf[2]; r_rxdata[1]=buf[3]; r_rxdata[2]=buf[4];
							  r_flag|=FIRST;	

						  }
						  else if(buf[1]=='1')	
						  {
							  if(strncmp(buf+2,check0,3)!=0 && strncmp(buf+2,check1,3)!=0 && strncmp(buf+2,check2,3)!=0 && strncmp(buf+2,check3,3)!=0 && strncmp(buf+2,check4,3)!=0 ) break;	
							  r_rxdata[3]=buf[2]; r_rxdata[4]=buf[3]; r_rxdata[5]=buf[4];
							  r_flag|=SECOND;	

						  }
						  else if(buf[1]=='2')	
						  {
							  if(strncmp(buf+2,check0,3)!=0 && strncmp(buf+2,check1,3)!=0 && strncmp(buf+2,check2,3)!=0 && strncmp(buf+2,check3,3)!=0 && strncmp(buf+2,check4,3)!=0 ) break;	
							  r_rxdata[6]=buf[2]; r_rxdata[7]=buf[3]; r_rxdata[8]=buf[4];
							  r_flag|=THIRD;	

						  }
						  else if(buf[1]=='3')	
						  {
							  if(strncmp(buf+2,check0,3)!=0 && strncmp(buf+2,check1,3)!=0 && strncmp(buf+2,check2,3)!=0 && strncmp(buf+2,check3,3)!=0 && strncmp(buf+2,check4,3)!=0 ) break;	
							  r_rxdata[9]=buf[2]; r_rxdata[10]=buf[3]; r_rxdata[11]=buf[4];
							  r_flag|=FOURTH;	

						  }
						  else if(buf[1]=='4')	
						  {
							  if(strncmp(buf+2,check0,3)!=0 && strncmp(buf+2,check1,3)!=0 && strncmp(buf+2,check2,3)!=0 && strncmp(buf+2,check3,3)!=0 && strncmp(buf+2,check4,3)!=0 ) break;	
							  r_rxdata[12]=buf[2]; r_rxdata[13]=buf[3]; r_rxdata[14]=buf[4];
							  r_flag|=FIFTH;	

						  }
						  //										r_cnt++;
						  break;

				}
//				if(flag==SENSER_DATA)
//				{
//					//							printf("senser_data=%s\n",s_rxdata);
//
///					senser=atoi(s_rxdata);
//					set_Senser(senser);
//				}
				if( t_flag==31 || b_flag==31 || l_flag==31 || r_flag==31 )
				{

					if(flag==RX_DATA_T)
					{
						strncpy(t_check[t_check_index++],t_rxdata,15);

						if(t_check_index==14)
						{	
							index=t_check_Ip();
							printf("sendIp=");
							for(i=0;i<15;i++)
								printf("%c",t_check[index][i]);

							printf("\n");
						}
					}

					if(flag==RX_DATA_B)
					{
						strncpy(b_check[b_check_index++],b_rxdata,15);

						if(b_check_index==14)
						{	
							index=b_check_Ip();
							printf("sendIp=");
							for(i=0;i<15;i++)
								printf("%c",b_check[index][i]);

							printf("\n");
						}

					}

					if(flag==RX_DATA_L)
					{
						strncpy(l_check[l_check_index++],l_rxdata,15);

						if(l_check_index==14)
						{	
							index=l_check_Ip();
							printf("sendIp=");
							for(i=0;i<15;i++)
								printf("%c",l_check[index][i]);

							printf("\n");
						}

					}

					if(flag==RX_DATA_R)
					{
						strncpy(r_check[r_check_index++],r_rxdata,15);

						if(r_check_index==14)
						{	
							index=r_check_Ip();
							printf("sendIp=");
							for(i=0;i<15;i++)
								printf("%c",r_check[index][i]);

							printf("\n");
						}

					}
				}
			}
			else if ( poll_events.revents & POLLERR)      // event 가 에러?
			{
				printf( "통신 라인에 에러가 발생, 프로그램 종료");
				break;
			}
		}
	}
	close(fd);
	return 0;
}

					/*
					   switch(flag )
					   {
					   case RX_DATA_T:
#if DEBUG
printf("rx_data_top=");
for(j=0;j<15;j++)
{
printf("%c",t_rxdata[j]);
}
printf("\n");
t_flag=0;
//								t_cnt=0;
#endif
break;
case RX_DATA_B:
#if DEBUG
printf("rx_data_bottom=");
for(j=0;j<15;j++)
{
printf("%c",b_rxdata[j]);
}
printf("\n");
b_flag=0;
//								b_cnt=0;
#endif
break;
case RX_DATA_L:
#if DEBUG
printf("rx_data_left=");
for(j=0;j<15;j++)
{
printf("%c",l_rxdata[j]);
}
printf("\n");
l_flag=0;
//							l_cnt=0;

#endif
break;
case RX_DATA_R:
#if DEBUG
printf("rx_data_right=");
for(j=0;j<15;j++)
{
printf("%c",r_rxdata[j]);
}
printf("\n");
r_flag=0;
//							r_cnt=0;

#endif
break;

}
*/

/*
   if(flag==RX_DATA_T)
   {
   int a;
   int b;
   strncpy(t_check[t_check_index++],t_rxdata,15);




   set_IpInfo(0,t_rxdata);
   IsConnectBuf=get_IsConnect();
   IsConnectBuf |= 1;
   set_IsConnect(IsConnectBuf);
   cal_process_start();

   if(t_check_index==9)
   {
   }
//						kill(0,SIGUSR1);
}

if(flag==RX_DATA_B)
{
set_IpInfo(2,b_rxdata);
IsConnectBuf=get_IsConnect();
IsConnectBuf |= 4;
set_IsConnect(IsConnectBuf);
cal_process_start();
//						kill(0,SIGUSR1);
}

if(flag==RX_DATA_L)
{
set_IpInfo(1,l_rxdata);
IsConnectBuf=get_IsConnect();
IsConnectBuf |= 2;
set_IsConnect(IsConnectBuf);
cal_process_start();
//						kill(0,SIGUSR1);
}

if(flag==RX_DATA_R)
{
set_IpInfo(3,r_rxdata);
IsConnectBuf=get_IsConnect();
IsConnectBuf |= 8;
set_IsConnect(IsConnectBuf);
cal_process_start();
//						kill(0,SIGUSR1);
}
*/
