#include "hash.h"

namespace Checksum
{
	// Print the MD5 sum as hex-digits.
	void print_md5_sum(unsigned char* md)
	{
		int i;
		for (i = 0; i <MD5_DIGEST_LENGTH; i++) {
			printf("%02x", md[i]);
		}
	}

	// Get the size of the file by its file descriptor
	unsigned long get_size_by_fd(int fd)
	{
		struct stat statbuf;
		if (fstat(fd, &statbuf) < 0) exit(-1);
		return statbuf.st_size;
	}
}