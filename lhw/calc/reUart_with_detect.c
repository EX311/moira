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

#define CHECK_OK 1
#define DANGER 0

void cal_process_start()//(int sig)
{
//	printf("cal process start\n");
	pid_t pid;

	pid=fork();
	switch(pid)
	{
		case -1:
			printf("cal process fork fail\n");
			break;

		case 0:
//			printf("fork calc start\n");
				execl("./calc_proc","calc_proc",0);
//			printf("fork calc end\n");
			break;
			
		default:
			break;
	
	}

}


int main( void)
{  
		int 	flag;
		int 	check=DANGER;
		int 	t_flag;
		int 	b_flag;
		int 	l_flag;
		int 	r_flag;
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

		char rxtemp0[15]={0};
		char rxtemp1[15]={0};
		char rxtemp2[15]={0};
		char rxtemp3[15]={0};

		int 	t_cnt=0;
		int 	b_cnt=0;
		int 	l_cnt=0;
		int 	r_cnt=0;
		int 	m_cnt=0;
		
		int 	place=0;
  	int 	poll_state;

//		struct sigaction act;
//		act.sa_handler=cal_process_start;
//		sigaction(SIGUSR1, &act, 0);
	
   	fd=initUart();
initPoll(fd);
	
	 // 자료 송수신:
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
					switch(buf[0])
					{
						case 's': flag=SENSER_DATA;
											s_rxdata[0]=buf[1];	s_rxdata[1]=buf[2];	s_rxdata[2]=buf[3];
											break;
						case 'w':	flag=DISCON_MSG_T;
											system("echo "" > /proc/board_status/IpInfo/IpInfo0");
											break;
						case 'a':	flag=DISCON_MSG_L;
											system("echo "" > /proc/board_status/IpInfo/IpInfo1");
											break;
						case 'd':	flag=DISCON_MSG_R;
											printf("disconnect R\n");
											system("echo "" > /proc/board_status/IpInfo/IpInfo3");
											break;
						case 'x':	flag=DISCON_MSG_B;
											system("echo "" > /proc/board_status/IpInfo/IpInfo2");
											break;
			
						case 't':
										flag=RX_DATA_T;
										if(buf[1]=='0')	
											{ 
												t_flag=0;
												t_rxdata[0]=buf[2]; t_rxdata[1]=buf[3]; t_rxdata[2]=buf[4];
												t_flag|=FIRST;	
											}
										else if(buf[1]=='1')	
											 {
												 t_rxdata[3]=buf[2]; t_rxdata[4]=buf[3]; t_rxdata[5]=buf[4];
											 	 t_flag|=SECOND;
											 }
										else if(buf[1]=='2')	
											 {
												 t_rxdata[6]=buf[2]; t_rxdata[7]=buf[3]; t_rxdata[8]=buf[4];
											 	 t_flag|=THIRD;

											 }
										else if(buf[1]=='3')	
											 {
												 t_rxdata[9]=buf[2]; t_rxdata[10]=buf[3]; t_rxdata[11]=buf[4];
											 	 t_flag|=FOURTH;
										 
											 }
										else if(buf[1]=='4')	
											 {
												 t_rxdata[12]=buf[2]; t_rxdata[13]=buf[3]; t_rxdata[14]=buf[4];
											 	 t_flag|=FIFTH;
										 	 }
							break;
						case 'b':
										flag=RX_DATA_B;
										if(buf[1]=='0')	
											 {
												 b_flag=0; 
												 b_rxdata[0]=buf[2]; b_rxdata[1]=buf[3]; b_rxdata[2]=buf[4];
													b_flag|=FIRST;	
										 
											 }
										else if(buf[1]=='1')	
											 {
												 b_rxdata[3]=buf[2]; b_rxdata[4]=buf[3]; b_rxdata[5]=buf[4];
												 b_flag|=SECOND;	
										 
											 }
										else if(buf[1]=='2')	
											 {
												 b_rxdata[6]=buf[2]; b_rxdata[7]=buf[3]; b_rxdata[8]=buf[4];
													b_flag|=THIRD;	
										 
											 }
										else if(buf[1]=='3')	
											 {
												 b_rxdata[9]=buf[2]; b_rxdata[10]=buf[3]; b_rxdata[11]=buf[4];
													b_flag|=FOURTH;	
										 
											 }
										else if(buf[1]=='4')	
											 {
												 b_rxdata[12]=buf[2]; b_rxdata[13]=buf[3]; b_rxdata[14]=buf[4];
													b_flag|=FIFTH;	
										 
											 }
							break;
						case 'l':
										flag=RX_DATA_L;
										if(buf[1]=='0')	
											 {
												 	l_flag=0;
												 l_rxdata[0]=buf[2]; l_rxdata[1]=buf[3]; l_rxdata[2]=buf[4];
													l_flag|=FIRST;	
										 
											 }
										else if(buf[1]=='1')	
											 {
												 l_rxdata[3]=buf[2]; l_rxdata[4]=buf[3]; l_rxdata[5]=buf[4];
													l_flag|=SECOND;	
										 
											 }
										else if(buf[1]=='2')	
											 {
												 l_rxdata[6]=buf[2]; l_rxdata[7]=buf[3]; l_rxdata[8]=buf[4];
													l_flag|=THIRD;	
										 
											 }
										else if(buf[1]=='3')	
											 {
												 l_rxdata[9]=buf[2]; l_rxdata[10]=buf[3]; l_rxdata[11]=buf[4];
													l_flag|=FOURTH;	
										 
											 }
										else if(buf[1]=='4')	
											 {
												 l_rxdata[12]=buf[2]; l_rxdata[13]=buf[3]; l_rxdata[14]=buf[4];
													l_flag|=FIFTH;	
										 
											 }
							break;
						case 'r':
										flag=RX_DATA_R;
										if(buf[1]=='0')	
											 {
													r_flag=0;
												 r_rxdata[0]=buf[2]; r_rxdata[1]=buf[3]; r_rxdata[2]=buf[4];
													r_flag|=FIRST;	
										 
											 }
										else if(buf[1]=='1')	
											{
												r_rxdata[3]=buf[2]; r_rxdata[4]=buf[3]; r_rxdata[5]=buf[4];
													r_flag|=SECOND;	
										
											}
										else if(buf[1]=='2')	
											{
												r_rxdata[6]=buf[2]; r_rxdata[7]=buf[3]; r_rxdata[8]=buf[4];
													r_flag|=THIRD;	
										
											}
										else if(buf[1]=='3')	
											{
												r_rxdata[9]=buf[2]; r_rxdata[10]=buf[3]; r_rxdata[11]=buf[4];
													r_flag|=FOURTH;	
										
											}
										else if(buf[1]=='4')	
											{
												r_rxdata[12]=buf[2]; r_rxdata[13]=buf[3]; r_rxdata[14]=buf[4];
													r_flag|=FIFTH;	
									
											}
							break;

					}
#if DEBUG
//					printf("cnt= %d, rxbuf_cnt= %d,t_cnt = %d, b_cnt = %d, l_cnt = %d,  r_cnt= %d\n",cnt,rxbuf_cnt,t_cnt,b_cnt,l_cnt,r_cnt);
#endif
/*					for(i=0;i<cnt;i++)
					{
						if(buf[i]=='t')	{ flag=RX_DATA_T;	t_cnt++; 
															//printf("%s\n",buf);
															continue; }
						if(buf[i]=='b')	{	flag=RX_DATA_B;	b_cnt++; 
																//printf("%s\n",buf);
																continue; }
						if(buf[i]=='l') {	flag=RX_DATA_L;	l_cnt++; 
																//printf("%s\n",buf);
																continue; }
						if(buf[i]=='r')	{	flag=RX_DATA_R; r_cnt++; 
																//printf("%s\n",buf);
																continue; }
						if(buf[i]=='s')	{ flag=SENSER_DATA;	printf("senser data=%s\n",buf); 	break;	}
						if(buf[i]=='d')	{	flag=CONNECT_MSG;	printf("disconnect\n");	continue;	}
					
						if(flag==RX_DATA_M)	{ m_rxdata[rxbuf_cnt++]=buf[i];	}
						if(flag==RX_DATA_T)	{	t_rxdata[rxbuf_cnt++]=buf[i];	}
						if(flag==RX_DATA_B)	{	b_rxdata[rxbuf_cnt++]=buf[i];	}
						if(flag==RX_DATA_L)	{	l_rxdata[rxbuf_cnt++]=buf[i];	}
						if(flag==RX_DATA_R)	{	r_rxdata[rxbuf_cnt++]=buf[i]; }

						if(rxbuf_cnt>=15)
						{
*/
					if(flag==SENSER_DATA)
							printf("senser_data=%s\n",s_rxdata);
					if(flag==DISCON_MSG_T)
							printf("Top disconnect\n");
					if(flag==DISCON_MSG_B)
							printf("Bottom disconnect\n");
					if(flag==DISCON_MSG_R)
							printf("Right disconnect\n");
					if(flag==DISCON_MSG_L)
							printf("Left disconnect\n");


					if( t_flag==31 || b_flag==31 || l_flag==31 || r_flag==31 )
					{
							switch(flag )
							{
								case RX_DATA_T:
									strncpy(rxtemp1,t_rxdata,15);
									t_cnt++;
										break;
								case RX_DATA_B:
									strncpy(rxtemp1,b_rxdata,15);
									b_cnt++;
										break;
								case RX_DATA_R:
									strncpy(rxtemp1,r_rxdata,15);
									r_cnt++;
										break;
								case RX_DATA_L:
									strncpy(rxtemp1,l_rxdata,15);
									l_cnt++;
										break;
							}
							if( strcmp(rxtemp0,rxtemp1)!=0 && strcmp(rxtemp1,rxtemp2)==0 && strcmp(rxtemp2,rxtemp3)==0)
							{
								check=CHECK_OK;
							}
							else
							{
								check=DANGER;
								strncpy(rxtemp3,rxtemp2,15);
								strncpy(rxtemp2,rxtemp1,15);
							}
													


	
//					if(t_cnt==5 || b_cnt==5 || l_cnt==5 || r_cnt==5 )
//					if( t_flag==31 || b_flag==31 || l_flag==31 || r_flag==31 )
							if(check == CHECK_OK)
							{
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
					if(flag==RX_DATA_T)
					{
						set_IpInfo(0,t_rxdata);
						IsConnectBuf=get_IsConnect();
						IsConnectBuf |= 1;
						set_IsConnect(IsConnectBuf);
						cal_process_start();
					}

					if(flag==RX_DATA_B)
					{
						set_IpInfo(2,b_rxdata);
						IsConnectBuf=get_IsConnect();
						IsConnectBuf |= 4;
						set_IsConnect(IsConnectBuf);
						cal_process_start();
					}

					if(flag==RX_DATA_L)
					{
						set_IpInfo(1,l_rxdata);
						IsConnectBuf=get_IsConnect();
						IsConnectBuf |= 2;
						set_IsConnect(IsConnectBuf);
						cal_process_start();
					}

					if(flag==RX_DATA_R)
					{
						set_IpInfo(3,r_rxdata);
						IsConnectBuf=get_IsConnect();
						IsConnectBuf |= 8;
						set_IsConnect(IsConnectBuf);
						cal_process_start();
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
