/********* super.c code ***************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

// define shorter TYPES, save typing efforts
typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;    // need this for new version of e2fs

GD    *gp;
SUPER *sp;
INODE *ip;
DIR   *dp; 

#define BLKSIZE 1024

/******************* in <ext2fs/ext2_fs.h>*******************************
struct ext2_super_block {
  u32  s_inodes_count;       // total number of inodes
  u32  s_blocks_count;       // total number of blocks
  u32  s_r_blocks_count;     
  u32  s_free_blocks_count;  // current number of free blocks
  u32  s_free_inodes_count;  // current number of free inodes 
  u32  s_first_data_block;   // first data block in this group
  u32  s_log_block_size;     // 0 for 1KB block size
  u32  s_log_frag_size;
  u32  s_blocks_per_group;   // 8192 blocks per group 
  u32  s_frags_per_group;
  u32  s_inodes_per_group;    
  u32  s_mtime;
  u32  s_wtime;
  u16  s_mnt_count;          // number of times mounted 
  u16  s_max_mnt_count;      // mount limit
  u16  s_magic;              // 0xEF53
  // A FEW MORE non-essential fields
};
**********************************************************************/

char buf[BLKSIZE];
char b1[BLKSIZE];
char b2[BLKSIZE];
char b3[BLKSIZE];
int fd;
int iblk;
//char path[64];
int nn;
char *name[32];  // at most 32 component names

int get_block(int fd, int blk, char buf[ ])
{
  lseek(fd, (long)blk*BLKSIZE, 0);
  read(fd, buf, BLKSIZE);
}

  int ls_ip(INODE *ip, char *name)
  {
    u16 mode;
    int mask, k;

    mode = ip->i_mode;
    if ((mode & 0xF000) == 0x4000)
        putchar('d');

    if ((mode & 0xF000) == 0xA000)
        putchar('l');
    else if ((mode & 0xF000) == 0x8000)
        putchar('-');

    mask = 000400;
    for (k=0; k<3; k++){
      if (mode & mask)
         putchar('r');
      else
         putchar('-');
      mask = mask >> 1;
      if (mode & mask)
         putchar('w');
      else
         putchar('-');
      mask = mask >> 1;
      if (mode & mask)
         putchar('x');
      else
         putchar('-');
      mask = mask >> 1;
    }    
    printf(" %d ", ip->i_links_count);
    printf(" %d  %d", ip->i_uid, ip->i_gid);
    printf("%d ", ip->i_size);
    printf("%.24s ", ctime(&ip->i_ctime));
    printf("%s ", name);
    printf("\n");     
}

int tokenize(char *path)
{
  int i;
  char *cp;
  cp = path;
  nn = 0;
  
  while (*cp != 0){
       while (*cp == '/') *cp++ = 0;        
       if (*cp != 0)
           name[nn++] = cp;         
       while (*cp != '/' && *cp != 0) cp++;                  
       if (*cp != 0)   
           *cp = 0;                   
       else 
            break; 
       cp++;
  }
  
  //printf("n = %d : ", nn);
  //for (i=0; i<nn; i++){}
       //printf("  %s  ", name[i]);
  //printf("\n");
}

int search(INODE *ip, char *name)
{
   int i; INODE *tp;
   char c, *cp;
   DIR  *dp; 
   for (i=0; i<12; i++){
       if ( ip->i_block[i] ){
         //printf("i_block[%d] = %d\n", i, ip->i_block[i]);
	      get_block(fd,ip->i_block[i], b2);
          cp = b2;
          dp = (DIR *)b2;

          while (cp < b2 + BLKSIZE){
              c = dp->name[dp->name_len];  // save last byte
              dp->name[dp->name_len] = 0;
              get_block(fd,iblk+((dp->inode -1) / 8), b3);      // read block inode of me 
      		  tp = (INODE *)b3 + ((dp->inode -1) % 8);
              ls_ip(tp,dp->name);
  			  //if (!(tp->i_mode >> 12 ^ 0x4))
	 			//printf("---this is dir\n");   
	          //printf("%s \n", dp->name); 
              //if ( strcmp(dp->name, name) == 0 ){
              //   return(dp->inode);
              //}
              dp->name[dp->name_len] = c; // restore that last byte
              cp += dp->rec_len;
			  
              dp = (DIR *)cp;
	}
     }
   }
   return 0;
}


int dir()
{
  int me;int i;


  //tokenize("/");
  // read SUPER block
  get_block(fd, 1, buf);  
  sp = (SUPER *)buf;
  // check for EXT2 magic number:
  printf("s_magic = %x\n", sp->s_magic);
  if (sp->s_magic != 0xEF53){
    printf("NOT an EXT2 FS\n");
    exit(1);
  }

  printf("EXT2 FS OK\n");

  get_block(fd, 2, buf);  
  gp = (GD *)buf;
  iblk = gp->bg_inode_table;
  get_block(fd,iblk, buf);
  ip = (INODE *)buf + 1;
  if (!(ip->i_mode >> 12 ^ 0x8))
	 printf("this is dir\n");
  printf("mode = %d\n",ip->i_mode);
  search(ip,"");
  // serach for system name
  /* 
  for (i=0; i<nn; i++){
      me = search(ip, name[i]);
      if (me == 0){
          printf("can't find %s\n", name[i]); 
          return(0);
      }
      me--;
      get_block(fd,iblk+(me / 8), b1);      // read block inode of me 
      ip = (INODE *)b1 + (me % 8);
   }
*/
}

char *disk = "mydisk";

int main(int argc, char *argv[ ])
{ 
  if (argc > 1)
     disk = argv[1];
  fd = open(disk, O_RDONLY);
  if (fd < 0){
    printf("open failed\n");
    exit(1);
  }
  dir();
}
