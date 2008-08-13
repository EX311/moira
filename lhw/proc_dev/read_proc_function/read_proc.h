
// read_proc_fd
int IsConnect_fd_return(void);
int IpInfo_fd_return(int num);
int AfterMasterIp_fd_return(int num);
int MyLocation_fd_return(void);
int MasterLocation_fd_return(void);
int DeviceAttached_fd_return(void);

//read_proc_function
char get_IsConnect(void);
int set_IsConnect(int place);
int get_IpInfo(int place,char* get_data);
int set_IpInfo(int place, int ip);
int get_AfterMasterIp(int place, char* get_data);
int set_AfterMasterIp(int place, int ip);
int get_MyLocation(void);
int set_MyLocation(int MyLocation);
int get_MasterLocation(void);
int set_MasterLocation(int MasterLocation);
int get_DeviceAttached(void);
int set_DeviceAttached(int DeviceAttached);

