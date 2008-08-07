

struct board_status{
	/* flags indicate connected devices on above,below,left,right sensor*/
	unsigned char is_connect;
	
	/* ip info of connected devices on above,below,left,right side.*/
	unsigned char *ip_info[4];

	/* ip information after mastering has been finished.
	 * left above device's index is 0
	 * right above device's index is 1
	 * left below device's index is 2
	 * right below device's index is 3 */

	unsigned char *mastered_ip_info[4];

	/* location index after mastering. following the same indexing rule */
	unsigned int my_location;

	/* master's location index after mastering. */
	unsigned int master_location;

	/* number of device got together */
	unsigned int device_attached;
};

struct board_struct bs;
/*
 *  control functions
 *
 *                                     */

unsigned char get_connection_status();
unsigned char *get_ip_info(unsigned int ir_loc);
unsigned char *get_mastered_ip_info(unsigned int loc);
unsigned int get_my_location();
unsigned int get_master_location();
unsigned int get_device_attached();

void set_connection_status(unsigned char connect);
void set_ip_info(unsigned char *ip,unsigned int ir_loc);
void set_mastered_ip_info(unsigned char *ip,unsigned int loc);
void set_my_location(unsigned int loc);
void set_master_location(unsigned int loc);
void set_device_attached(unsigned int num_device);
