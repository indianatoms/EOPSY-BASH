#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
// for warnings with memcpy
//#include <stdlib.h> // for malloc

int read_write(int file1,int file2){
	size_t copied_bytes;
	char buf[512*1024]; // set 512KB buffer
	size_t NumberOfbyte = sizeof(buf);
	while ((copied_bytes = read(file1, &buf, NumberOfbyte)) > 0){
	//write(int fildes, const void *buf, size_t nbytes);
	//int fildes	The file descriptor of where to write the output. You can either use a file descriptor obtained from the open system call, or you can use 0, 1, or 2, to refer to standard input, standard output, or standard error, respectively.
    //const void *buf	A pointer to a buffer of at least nbytes bytes, which will be written to the file.
    //size_t nbytes	The number of bytes to write. If smaller than the provided buffer, the output is truncated.
    //return value	Returns the number of bytes that were written. If value is negative, then the system call returned an error.
    if (write(file2, &buf, copied_bytes) < 0)
		printf("\nERROR: Error writing into output file!\n");
	if (copied_bytes < 0)
		printf("\nERROR: Error reading input file!\n");
    }
}
int map(int file1, int file2){
	struct stat fileStat;
	void *in_map, *out_map;
	//These functions return information about a file. No permissions are required on the file itself
	if(fstat(file1,&fileStat) < 0){
		printf(stderr,"\nERROR: fstat on the source file failed!\n");
		return 1;
	}
	// truncate a file to a specified length (initially it's 0 bytes length)
	ftruncate(file2,fileStat.st_size);
	//truncated to a size of precisely length bytes.
	//mmap() creates a new mapping in the virtual address space of the
    //calling process.  The starting address for the new mapping is
    //specified in addr.  The length argument specifies the length of the
    //mapping (which must be greater than 0).
	if ((in_map = mmap(0, fileStat.st_size, PROT_READ, MAP_SHARED, file1, 0)) == MAP_FAILED)
		printf("\nERROR: mmap for the source file failed!\n");
	if ((out_map = mmap(0, fileStat.st_size, PROT_WRITE, MAP_SHARED, file2, 0)) == MAP_FAILED)
		printf("\nERROR: mmap for the output file failed!\n");
	if (memcpy(out_map, in_map, fileStat.st_size) == NULL)
		printf("\nERROR: memcpy() function failed!\n");
	if (munmap(in_map, fileStat.st_size))
		printf("\nERROR: munmap for the input file failed!\n");
	if (munmap(out_map, fileStat.st_size))
		printf("\nERROR: munmap for the output file failed!\n");
}
void printHelp()
{
    printf("The program used for copying files.\nUse Cases:\ncopy [-m] <file_name> <new_file_name>\nIf user runs program without m, program will copy the content of the file by reading and writing it to other newfile.\nIf user runs program with m,, program will copy the content by maping the maping to the same place as copied filed.\ncopy [-h]\nRunning -h parameters prints out help which displays use cases and program descripiton.\nDescription: Program goal is to allow user copy files, however give user has a choise whether the read a write data or map files to memory regions.\n If program is run withoout any parameters it will show the content of -h parameter. program uses read() and write() functions to copy file contents.\nIf the option -m is given, both files are mapped to memory regions with mmap() and are copied the file with memcpy() instead.\n\n");
	return;
}

int main (int argc, char **argv){
	char parameter, use_mmap = 0; //use_map to -m parameter -h for help
	int file1 = 0, file2 = 0;
	while((parameter = getopt (argc, argv, "mh")) != -1){
	//function parses the command-line arguments.  Its
    //   arguments argc and argv are the argument count and array as passed to
    //   the main() function on program invocation.
    //   parameter  = arguments provided
		switch (parameter){
			case 'm':
				use_mmap = 1;
				break;
			case 'h':
				printHelp();
				return 0;
				break;
			default:
				printf("\nERROR: wrong parameter input");
				return 0;
		}
	}
//go here if we want to copy
//open file
        if ((file1 = open(argv[optind], O_RDONLY)) < 0)//O_RDONLY Open for reading only.
        {
			printf("\nERROR: Can't open %s for reading!\n", argv[optind]);
			printHelp();
			//The variable optind is the index of the next element to be processed in argv. The system initializes this value to 1.
			return 1;
        }
		//O_CREAT	If the file does not exist, create it. If the O_CREAT option is used, then you must include the third parameter.
		//O_TRUNC	Initially clear all data from the file.
		//O_RDWR	Open the file so that it can be read from and written to.
		//open file 2
		if ((file2 = open(argv[optind+1], O_RDWR | O_CREAT | O_TRUNC, 0666)) < 0){
			printf("\nERROR: Can't open %s for writing!\n", argv[optind+1]);
			return 1;
		}
		if (use_mmap){
			printf("\nCopying '%s' into '%s' using maping of memory.\n\n",argv[optind],argv[optind+1]);
			map(file1, file2);
		}
		else{
			printf("\nCopying '%s' into '%s' using read() and write() functions.\n\n",argv[optind],argv[optind+1]);
			read_write(file1, file2);
		}
		//check if file has been opened correctly
		if (close(file1))
			printf("\nERROR while closing file 1!\n");
		if (close(file2))
			printf("\nERROR while closing file 2!\n");
	return 0;
}



