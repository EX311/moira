CC = arm-linux-gcc
//CC = gcc
all:
	@make Master
	@make reUart
	@make random_write
	@make calc_proc
	@make UI
Master : MasterIpSet.c read_proc_fd.c read_proc_function.c
	$(CC) -o $@ $^

reUart : reUart.c read_proc_fd.c read_proc_function.c
	$(CC) -o $@ $^

reUart_det : reUart_with_detect.c read_proc_fd.c read_proc_function.c
		$(CC) -o $@ $^


random_write : random_write.c read_proc_fd.c read_proc_function.c
	$(CC) -o $@ $^ 

calc_proc : read_proc_fd.c read_proc_function.c calc_proc.c 
	$(CC) -o $@ $^ 

UI : read_proc_fd.c read_proc_function.c start_UI.c 
	$(CC) -o $@ $^ 

clean : 
	@rm -rf *.o read_proc
	@rm -rf Master
	@rm -rf reUart
	@rm -rf random_write
	@rm -rf calc_proc
	@rm -rf UI

