arm-linux-gcc -Wall -o tilt_clnt tiltsend.c fbutils.c font_8x8.c read_proc_fd.c read_proc_function.c -lts &&
arm-linux-gcc -Wall -o tilt_serv tiltserver.c fbutils.c font_8x8.c read_proc_fd.c read_proc_function.c -lts -lpthread&&
cp tilt_clnt tilt_serv /tftpboot/myrootdir_26/root/&&
cp tilt* /svn/theMeal/trunk/&&
cp tilt_clnt tilt_serv /tftpboot/myrootdir_27/root/
