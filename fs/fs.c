/*
  Filename   : fs.c
  Author     : Christian Shepperson
  Course     : CSCI 380-01
  Assignment : FileSystems
  Description: A filesystem that can create, delete, write, and read
*/


//Local includes
/*******/
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "fs.h"

#define FS_NUM_BLOCKS    128
#define FS_MAX_FILE_SIZE 8
#define FS_MAX_FILENAME  16
#define FS_MAX_FILES     16
#define FS_BLOCK_SIZE    1024
#define INODE_SIZE       56

const int MAX_ARGS = 5;


struct fs_t
{
  int fd;
};
struct inode {
  char name[16];        //file name
  int size;             // file size (in number of blocks)
  int blockPointers[8]; // direct block pointers
  int used;             // 0 => inode is free; 1 => in use
};

void
tokenize (char* args[], char* command);
// open the file with the above name
void
fs_open (struct fs_t *fs, char diskName[16])
{
  fs->fd = open(diskName, O_RDWR, 0600);
}

// close and clean up all associated things
void
fs_close (struct fs_t *fs)
{
  close(fs->fd);
}

// create a file with this name and this size
void
fs_create (struct fs_t *fs, char name[16], int size)
{
  if(size > FS_MAX_FILE_SIZE){
      printf("size too big");
      return;
  }
char fs_buff[FS_NUM_BLOCKS];

struct inode inode[FS_MAX_FILES];

  // high level pseudo code for creating a new file

  // Step 1: check to see if we have sufficient free space on disk by
  // reading in the free block list. To do this:
  // - move the file pointer to the start of the disk file.
  lseek(fs->fd, 0, SEEK_SET);
  // - Read the first 128 bytes (the free/in-use block information)
  read(fs->fd,fs_buff,FS_NUM_BLOCKS);
  // - Scan the list to make sure you have sufficient free blocks to
  //   allocate a new file of this size
  int free_blocks = 0;
  for(int i = 0; i < FS_NUM_BLOCKS; ++i){
      if(fs_buff[i] == 0){
        ++free_blocks; 
      }
  }
  if(free_blocks < size){
    printf("not enough space");
    return;
  }
  // Step 2: we look  for a free inode on disk
  // - Read in a inode
  lseek(fs->fd, FS_NUM_BLOCKS, SEEK_SET);
    read(fs->fd, inode, INODE_SIZE * FS_MAX_FILES);
  
  // - check for duplicate name
  for(int i = 0; i < FS_MAX_FILES; ++i){
    if(strcmp(inode[i].name, name) == 0){
      printf("duplicate name");
      return;
    }
  }
   // - check the "used" field to see if it is free
 int node_spot;
  for(node_spot = 0; node_spot < FS_MAX_FILES; ++node_spot){
      if(inode[node_spot].used == 0){
          inode[node_spot].used = 1;
        // - Copy the filename to the "name" field
        strncpy(inode[node_spot].name, name, 16);
        // - Copy the file size (in units of blocks) to the "size" field
        inode[node_spot].size = size;
        break;
      }
  }
  if(node_spot == FS_MAX_FILES){
    printf("no free inodes");
    return;
  }


  // Step 3: Allocate data blocks to the file
  // - Scan the block list that you read in Step 1 for a free block
  // - Once you find a free block, mark it as in-use (Set it to 1)
int blockptr = 0;
    for(int i = 0; i < FS_NUM_BLOCKS && blockptr < size; ++i)
    {
        if(fs_buff[i] == 0)
        {
            fs_buff[i] = 1;
            inode[node_spot].blockPointers[blockptr] = i;
            ++blockptr;
        }
    }
  // - repeat until you allocated "size" blocks

  // Step 4: Write out the inode and free block list to disk
  // - Move the file pointer to the start of the file
  lseek(fs->fd, 0, SEEK_SET);
  // - Write out the 128 byte free block list
  write(fs->fd, fs_buff, FS_NUM_BLOCKS);
  // - Move the file pointer to the position on disk where this inode was stored
  // - Write out the inode
  write(fs->fd, inode, INODE_SIZE * FS_MAX_FILENAME);
}

// Delete the file with this name
void
fs_delete (struct fs_t *fs, char name[16])
{
struct inode node;
char fs_buff[FS_NUM_BLOCKS];
int inode_loc = -1;
  // Step 1: Locate the inode for this file
  //   - Move the file pointer to the 1st inode (129th byte)
  lseek(fs->fd, FS_NUM_BLOCKS, SEEK_SET);
  //   - Read in a inode

  for(int i = 0; i < FS_NUM_BLOCKS; ++i){
      read(fs->fd, &node, INODE_SIZE);
     if(node.used == 1 && strcmp(node.name, name) == 0){
        inode_loc = FS_NUM_BLOCKS + i * INODE_SIZE;
      break;
    }
  }
  if(inode_loc == -1){
      printf("location not found");
      return;
  }
  //   - If the iinode is free, repeat above step.
  //   - If the iinode is in use, check if the "name" field in the
  //     inode matches the file we want to delete. IF not, read the next
  //     inode and repeat

  // Step 2: free blocks of the file being deleted
  // Read in the 128 byte free block list (move file pointer to start
  lseek(fs->fd, 0, SEEK_SET);
  read(fs->fd, fs_buff, FS_NUM_BLOCKS);
  //   of the disk and read in 128 bytes)
  // Free each block listed in the blockPointer fields
  for(int i = 0; i < node.size; ++i){
     fs_buff[node.blockPointers[i]] = 0;
  }
  // Step 3: mark inode as free
  // Set the "used" field to 0.
  node.used = 0;
  // Step 4: Write out the inode and free block list to disk
  // Move the file pointer to the start of the file
  lseek(fs->fd, 0, SEEK_SET);
  // Write out the 128 byte free block list
  write(fs->fd, fs_buff, FS_NUM_BLOCKS);
  // Move the file pointer to the position on disk where this inode was stored
  lseek(fs->fd, inode_loc, SEEK_SET);
  // Write out the inode
  write(fs->fd, &node, INODE_SIZE);
}

// List names of all files on disk
void
fs_ls (struct fs_t *fs)
{
  struct inode node;
  // Step 1: read in each inode and print!
  lseek(fs->fd, FS_NUM_BLOCKS, SEEK_SET);
  // Move file pointer to the position of the 1st inode (129th byte)
  for(int i = 1; i <= FS_MAX_FILES; ++i){

    read(fs->fd, &node, INODE_SIZE);
    if(node.used == 1){
      printf ("%16s %6dB\n", node.name, node.size * FS_BLOCK_SIZE);
    }
  }
  // for each inode:
  //   read it in
  //   if the inode is in-use
  //     print the "name" and "size" fields from the inode
  // end for
}

// read this block from this file
void
fs_read (struct fs_t *fs, char name[16], int blockNum, char buf[1024])
{
  struct inode node;
  int inode_loc = -1;
  // Step 1: locate the inode for this file
  //   - Move file pointer to the position of the 1st inode (129th byte)
  //   - Read in a inode
  //   - If the inode is in use, compare the "name" field with the above file
  //   - If the file names don't match, repeat
   lseek(fs->fd, FS_NUM_BLOCKS, SEEK_SET);
  //   - Read in a inode

    for(int i = 0; i < FS_NUM_BLOCKS; ++i){
      read(fs->fd, &node, INODE_SIZE);
     if(node.used == 1 && strcmp(node.name, name) == 0){
        inode_loc = FS_NUM_BLOCKS + i * INODE_SIZE;
      break;
    }
  }
   if(inode_loc == -1){
      printf("location not found");
      return;
  }
  // Step 2: Read in the specified block
  // Check that blockNum < inode.size, else flag an error
  if(blockNum >= node.size ){
    return;
  }
  // Get the disk address of the specified block
  // move the file pointer to the block location
  lseek(fs->fd, 0, SEEK_SET);
  lseek(fs->fd, node.blockPointers[blockNum] * FS_BLOCK_SIZE, SEEK_SET);
  // Read in the block! => Read in 1024 bytes from this location into the buffer
  // "buf"
  read(fs->fd, buf, FS_BLOCK_SIZE);
}

// write this block to this file
void
fs_write (struct fs_t *fs, char name[16], int blockNum, char buf[1024])
{

  // Step 1: locate the inode for this file
  // Move file pointer to the position of the 1st inode (129th byte)
  // Read in a inode
  // If the inode is in use, compare the "name" field with the above file
  // If the file names don't match, repeat
    struct inode node;
    int inode_loc = -1;
  // Step 1: locate the inode for this file
  //   - Move file pointer to the position of the 1st inode (129th byte)
  //   - Read in a inode
  //   - If the inode is in use, compare the "name" field with the above file
  //   - If the file names don't match, repeat
   lseek(fs->fd, FS_NUM_BLOCKS, SEEK_SET);
  //   - Read in a inode

    for(int i = 0; i < FS_NUM_BLOCKS; ++i){
      read(fs->fd, &node, INODE_SIZE);
     if(node.used == 1 && strcmp(node.name, name) == 0){
        inode_loc = FS_NUM_BLOCKS + i * INODE_SIZE;
      break;
    }
  }
   if(inode_loc == -1){
      printf("location not found");
      return;
  }
  // Step 2: Write to the specified block
  // Check that blockNum < inode.size, else flag an error
  if(blockNum >= node.size ){
    return;
  }
  // Get the disk address of the specified block
  // move the file pointer to the block location
  lseek(fs->fd, 0, SEEK_SET);
  lseek(fs->fd, node.blockPointers[blockNum] * FS_BLOCK_SIZE, SEEK_SET);
  // Write the block! => Write 1024 bytes from the buffer "buf" to this location
  write(fs->fd, buf, FS_BLOCK_SIZE);
}

// REPL entry point
void
fs_repl ()
{
    struct fs_t myfs;
  char in[60];
  char buff[1024];
  FILE* src = fopen("input.txt", "r");
  if (src < 0)
  {
    perror("Open Error");
    exit(1);
  }

  char* diskname = fgets(in, sizeof(in), src);
  fs_open(&myfs, diskname);

  while (fgets(in, sizeof(in), src))
  {
    char* args[60];
    tokenize(args, in);
    
    if (strncmp(args[0], "C", 1))
    {
      fs_create(&myfs, args[1], *args[2]);
    }
    else if (strncmp(args[0], "D", 1))
    {
      fs_delete(&myfs, args[1]);
    }
    else if (strncmp(args[0], "L", 1))
    {
      fs_ls(&myfs);
    }
    else if (strncmp(args[0], "R", 1))
    {
      fs_read(&myfs, args[1], *args[2], buff);
    }
    else if (strncmp(args[0], "W", 1))
    {
      fs_write(&myfs, args[1], *args[2], buff);
    }
  }

 fs_close(&myfs);
}


void
tokenize (char* args[], char* command){

	const char* delim = "\t \n";

	char* token = strtok(command, delim);

	int i = 0;

	while (token && i < MAX_ARGS - 1){
		args[i] = token;

		token = strtok(NULL, delim);

		++i;
	}
	args[i] = NULL; 
}