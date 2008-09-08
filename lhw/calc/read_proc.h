
#define TOP 	0x2
#define BOTTOM 	0x8
#define LEFT 	0x1
#define RIGHT 	0x4

// read_proc_fd
int IsConnect_fd_return(void);
int IpInfo_fd_return(int num);
int AfterMasterIp_fd_return(int num);
int MyLocation_fd_return(void);
int MasterLocation_fd_return(void);
int DeviceAttached_fd_return(void);

//read_proc_function
char get_IsConnect(void);
//***********************************************
//return value: 1 -> TOP
//							2 -> LEFT
//							4 -> BOTTOM
//							8 -> RIGHT
//***********************************************

int set_IsConnect(char IsConnect);
//***********************************************
//argument place: Connected Place
//return value: 1 (TRUE)
//							Others (FALSE)
//***********************************************

int get_IpInfo(int place,char* get_data);
//***********************************************
//argument 	place : Ip place
// 					get_data : got Ip
//return value : got Ip count
//***********************************************

int set_IpInfo(int place, char* ip);
//***********************************************
//argument 	place : IP set place
// 					ip : setting Ip
//return value : 1 (TRUE)
//							 Others (FALSE)
//***********************************************

int get_AfterMasterIp(int place, char* get_data);
//***********************************************
//argument 	place : Ip place
// 					get_data : got Ip
//return value : got Ip count
//***********************************************

int set_AfterMasterIp(int place, int ip);
//***********************************************
//argument 	place : Ip set place
// 					get_data : setting Ip
//return value : 1 (TRUE)
//							 Others (FALSE)
//***********************************************

int get_MyLocation(void);
//***********************************************
//return value : MyLocation INFO
//							1 -> TOP
//							2 -> LEFT
//							4 -> BOTTOM
//							8 -> RIGHT
//***********************************************

int set_MyLocation(int MyLocation);
//***********************************************
//argument 	MyLocation : MyLocation Info
//						  1 -> TOP
//							2 -> LEFT
//							4 -> BOTTOM
//							8 -> RIGHT
//return value : 1 (TRUE)
//							 Others (FALSE)
//***********************************************

int get_MasterLocation(void);
//***********************************************
//return value: 1 -> TOP
//							2 -> LEFT
//							4 -> BOTTOM
//							8 -> RIGHT
//***********************************************

int set_MasterLocation(int MasterLocation);
//***********************************************
//argument 	MasterLocation : MasterLocation INFO
//						  1 -> TOP
//							2 -> LEFT
//							4 -> BOTTOM
//							8 -> RIGHT
//***********************************************

int get_DeviceAttached(void);
//***********************************************
//return value : How many Device Attaced
//***********************************************

int set_DeviceAttached(int DeviceAttached);
//***********************************************
//argument 	DeviceAttaced : Attaced Device num
//return value : 1 (TRUE)
//							 Others (FALSE)
//***********************************************

