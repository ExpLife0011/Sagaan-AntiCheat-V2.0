#include "openssl\\md5.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


namespace Checksum
{
	extern void print_md5_sum(unsigned char* md);
	extern unsigned long get_size_by_fd(int fd);
}