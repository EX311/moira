

struct board_status{
	unsigned char is_connect;
	unsigned char *ip_info[4];
	unsigned char *mastered_ip_info[4];
	unsigned int my_location;
	unsigned int master_location;
	unsigned int device_attached;
};
