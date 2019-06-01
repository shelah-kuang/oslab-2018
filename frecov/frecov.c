#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>    
#include <fcntl.h>
#include <sys/mman.h>
#include <assert.h>
#include <unistd.h>
#include <stdint.h>

int fd;
char *buf;
struct BPB{
  //.... have more but we dont need it
  unsigned short bytsPerSec; //0xb
  char SecPerClus;   //0xd
  unsigned short ReservedSec; //0xe
  char FatNum; //0x10
  unsigned short RootEnt;//0x11  should be 0
  unsigned short SmallSec;//0x13 be 0;
  char MediaDescri;
  unsigned short SecPerFat_0; //0x16 always be 0
  unsigned short SecPerTrack; //0x18
  unsigned short NumHeads;//0x1a
  unsigned int HiddenSec; //0x1c
  unsigned int LargeSec; //0x20
  unsigned int SecPerFat; //0x24
  //.... have more but we dont need it
};
struct Dir{
  char FileName[8];
  char Extension[3];
  char Attr;
  char Reserved;
  char CrTimeLen;
  unsigned short CrTime;
  unsigned short CrDate;
  unsigned short LastVisTime;
  unsigned short HAddr;
  unsigned short LastTime;
  unsigned short LastDate;
  unsigned short LAddr;
  unsigned int Length; 
};

struct BPB bpb;
uintptr_t ResevByte;
uintptr_t BytsPerClus;
uintptr_t DirPerClus;
int NumClus;

void get_info(){
  memcpy(&bpb,buf+0xb,sizeof(bpb));
  /*printf("each sec has byte %d\n",bpb.bytsPerSec);
  printf("each Cluster has %d sec\n",bpb.SecPerClus);
  printf("you reserve %d sec\n",bpb.ReservedSec);
  printf("you have %d Fat\n",bpb.FatNum);*/
  ResevByte = (bpb.bytsPerSec)*(bpb.SecPerFat*bpb.FatNum+ bpb.ReservedSec);
  BytsPerClus = bpb.SecPerClus * bpb.bytsPerSec;
  NumClus = (bpb.LargeSec- ResevByte/bpb.bytsPerSec)/bpb.SecPerClus;
  DirPerClus = BytsPerClus/32;
}

void find_bmp(){
  struct Dir dir;
  for(int i=0;i<NumClus;i++){
    for(int j=0;j<DirPerClus;j++){
      memcpy(&dir,buf+ResevByte+i*BytsPerClus+j*32,sizeof(dir));
      if(!strcmp(dir.Extension,"BMP")){
        printf("this is a yellow pic\n");
      }
    }
  }
}




int main(int argc, char *argv[]) {
  //*******read file img
  fd = open(argv[1],O_RDONLY);
  //printf("You will recover %s\n",argv[1]);
  if(fd<0){
    printf("ERROR: YOU entered an bad addr!\n");
    assert(0);
  }
  buf = (char*) mmap(NULL,1<<27,PROT_READ,MAP_SHARED,fd,0);
  //*******recovery
  get_info();
  find_bmp();
  //*******Stop work
  munmap((void*)buf,1<<27);
  close(fd);
  return 0;
}