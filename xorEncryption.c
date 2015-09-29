#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#define BUFF_SIZE 4096

int main(int argc, char *argv[])
{
	if (argc != 4) /* Check for the appropriate number of arguments */
	{
		printf("usage: %s inputFile keyFile outputFile\n", argv[0]);
		return 0;
	};
	// Open the input files
	int in_desc = open(argv[1], O_RDONLY);
	int ke_desc = open(argv[2], O_RDONLY);
	// If the output file doesn't exist, create it
	int ou_desc = open(argv[3], O_RDWR | O_TRUNC | O_CREAT, 0444);

	// Make sure there were no errors with the file arguments
	if (in_desc < 0 || ke_desc < 0)
	{
		printf("There was a problem reading one of the input files.\n");
		return -1;
	}

	ssize_t inBytes = 0; // used to keep track of the amount of bytes read per reading

	// Create arrays for holding the file byte information
	char inBuf[BUFF_SIZE], keBuf[BUFF_SIZE], ouBuf[BUFF_SIZE];

	while((inBytes = read(in_desc, inBuf, BUFF_SIZE)) > 0)
	{
		ssize_t keBytes = read(ke_desc, keBuf, BUFF_SIZE);

		while(keBytes < inBytes)
		{
			lseek(ke_desc, 0, SEEK_SET);
			keBytes += read(ke_desc, &(keBuf[keBytes]), inBytes - keBytes);
		}

		int i;
		for(i = 0; i < inBytes; i++)
		{
			ouBuf[i] = inBuf[i] ^ keBuf[i];
		}

		int write_success = write(ou_desc, ouBuf, inBytes);
		if (write_success < 0)
		{
			printf("There was a problem writing to the output file.\n");
		}
	}
	
	close(in_desc);
	close(ke_desc);
	close(ou_desc);
	
	return 0;
}
