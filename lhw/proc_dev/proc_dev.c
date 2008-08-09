#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>

#include <asm/uaccess.h>
#include <asm/io.h>

#include <linux/stat.h>
#include <linux/proc_fs.h>

#define DATASIZE 100

struct proc_dir_entry *board_status_fp=NULL;
struct proc_dir_entry *IsConnect_fp=NULL;
struct proc_dir_entry *IpInfo_fp=NULL;
struct proc_dir_entry *AfterMasterIp_fp=NULL;
struct proc_dir_entry *MyLocation_fp=NULL;
struct proc_dir_entry *MasterLocation_fp=NULL;
struct proc_dir_entry *DeviceAttached_fp=NULL;

char IsConnect_data[DATASIZE]={0,};
char IpInfo_data[DATASIZE]={0,};
char AfterMasterIp_data[DATASIZE]={0,};
char MyLocation_data[DATASIZE]={0,};
char MasterLocation_data[DATASIZE]={0,};
char DeviceAttached_data[DATASIZE]={0,};
// page: 저장할 페이지
// off: 정보기록 시작위치
// count: 읽어올 바이트수
// eof: 파일의 끝일경우 1로 셋팅된다.
// start: 커널 어느곳에도 사용되지 않으며, 
// data_unused: 여러파일들에 대한 하나의 콜백함수를 생성하는데 쓰일수 있다. 
int get_IsConnect(char *page, char **start, off_t off, int count, int *eof, void *data_unused)
{

	char len;
	len = sprintf(page, "IsConnect is %s\n",(char*)data_unused);

	return len;
}


int set_IsConnect(struct file *file, const char __user *buffer, unsigned long count, void *data)
{
	int len;
	char *realdata;

	realdata = (char *)data;

	if(copy_from_user(realdata,buffer,count)) return -EFAULT;

	realdata[count]='\0';
	len=strlen(realdata);
	if(realdata[len-1]=='\n')
		realdata[--len]=0;
	return count;

}

int get_IpInfo(char *page, char **start, off_t off, int count, int *eof, void *data_unused)
{

	char len;
	len = sprintf(page, "IpInfo is %s\n",(char*)data_unused);

	return len;
}


int set_IpInfo(struct file *file, const char __user *buffer, unsigned long count, void *data)
{
	int len;
	char *realdata;

	realdata = (char *)data;

	if(copy_from_user(realdata,buffer,count)) return -EFAULT;

	realdata[count]='\0';
	len=strlen(realdata);
	if(realdata[len-1]=='\n')
		realdata[--len]=0;
	return count;

}

int get_AfterMasterIp(char *page, char **start, off_t off, int count, int *eof, void *data_unused)
{

	char len;
	len = sprintf(page, "AfterMasterIp is %s\n",(char*)data_unused);

	return len;
}


int set_AfterMasterIp(struct file *file, const char __user *buffer, unsigned long count, void *data)
{
	int len;
	char *realdata;

	realdata = (char *)data;

	if(copy_from_user(realdata,buffer,count)) return -EFAULT;

	realdata[count]='\0';
	len=strlen(realdata);
	if(realdata[len-1]=='\n')
		realdata[--len]=0;
	return count;

}

int get_MyLocation(char *page, char **start, off_t off, int count, int *eof, void *data_unused)
{

	char len;
	len = sprintf(page, "MyLocation is %s\n",(char*)data_unused);

	return len;
}


int set_MyLocation(struct file *file, const char __user *buffer, unsigned long count, void *data)
{
	int len;
	char *realdata;

	realdata = (char *)data;

	if(copy_from_user(realdata,buffer,count)) return -EFAULT;

	realdata[count]='\0';
	len=strlen(realdata);
	if(realdata[len-1]=='\n')
		realdata[--len]=0;
	return count;

}

int get_MasterLocation(char *page, char **start, off_t off, int count, int *eof, void *data_unused)
{

	char len;
	len = sprintf(page, "MasterLocation is %s\n",(char*)data_unused);

	return len;
}


int set_MasterLocation(struct file *file, const char __user *buffer, unsigned long count, void *data)
{
	int len;
	char *realdata;

	realdata = (char *)data;

	if(copy_from_user(realdata,buffer,count)) return -EFAULT;

	realdata[count]='\0';
	len=strlen(realdata);
	if(realdata[len-1]=='\n')
		realdata[--len]=0;
	return count;

}

int get_DeviceAttached(char *page, char **start, off_t off, int count, int *eof, void *data_unused)
{

	char len;
	len = sprintf(page, "DeviceAttached is %s\n",(char*)data_unused);

	return len;
}


int set_DeviceAttached(struct file *file, const char __user *buffer, unsigned long count, void *data)
{
	int len;
	char *realdata;

	realdata = (char *)data;

	if(copy_from_user(realdata,buffer,count)) return -EFAULT;

	realdata[count]='\0';
	len=strlen(realdata);
	if(realdata[len-1]=='\n')
		realdata[--len]=0;
	return count;

}

int board_status_init(void)
{
	board_status_fp=proc_mkdir("board_status",0);

	IsConnect_fp = create_proc_entry("IsConnect", S_IFREG | S_IRWXU, board_status_fp);
	if(IsConnect_fp)
	{
		IsConnect_fp->data=IsConnect_data;
		IsConnect_fp->read_proc=get_IsConnect;
		IsConnect_fp->write_proc=set_IsConnect;
	}

	IpInfo_fp = create_proc_entry("IpInfo", S_IFREG | S_IRWXU, board_status_fp);
	if(IpInfo_fp)
	{
		IpInfo_fp->data=IpInfo_data;
		IpInfo_fp->read_proc=get_IpInfo;
		IpInfo_fp->write_proc=set_IpInfo;
	}

	AfterMasterIp_fp = create_proc_entry("AfterMasterIp", S_IFREG | S_IRWXU, board_status_fp);
	if(AfterMasterIp_fp)
	{
		AfterMasterIp_fp->data=AfterMasterIp_data;
		AfterMasterIp_fp->read_proc=get_AfterMasterIp;
		AfterMasterIp_fp->write_proc=set_AfterMasterIp;
	}

	MyLocation_fp = create_proc_entry("MyLocation", S_IFREG | S_IRWXU, board_status_fp);
	if(MyLocation_fp)
	{
		MyLocation_fp->data=MyLocation_data;
		MyLocation_fp->read_proc=get_MyLocation;
		MyLocation_fp->write_proc=set_MyLocation;
	}
	
	MasterLocation_fp = create_proc_entry("MasterLocation", S_IFREG | S_IRWXU, board_status_fp);
	if(MasterLocation_fp)
	{
		MasterLocation_fp->data=MasterLocation_data;
		MasterLocation_fp->read_proc=get_MasterLocation;
		MasterLocation_fp->write_proc=set_MasterLocation;
	}
	
	DeviceAttached_fp = create_proc_entry("DeviceAttached", S_IFREG | S_IRWXU, board_status_fp);
	if(DeviceAttached_fp)
	{
	 	DeviceAttached_fp->data=DeviceAttached_data;
		DeviceAttached_fp->read_proc=get_DeviceAttached;
		DeviceAttached_fp->write_proc=set_DeviceAttached;
	}

	return 0;
}

void board_status_exit(void)
{
remove_proc_entry("IsConnect",board_status_fp);
remove_proc_entry("board_status",0);

}

module_init(board_status_init);
module_exit(board_status_exit);

MODULE_LICENSE("Dual BSD/GPL");
