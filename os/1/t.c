/*******************************************************
*                  @t.c file                          *
*******************************************************/

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#define TRK 18
#define CYL 36
#define BLK 1024

#include "ext2.h"
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

int prints(char *s)
{
    /*
    char *temp;
    for (temp = s; *temp != '\0'; temp++){
        putc(*temp);
    }
    */
    while(*s){
        putc(*s++);
    }

}

int gets(char *s)
{
    /*
    char *temp = s;
    char get = (char)getc();
    while(get != 0x0D){
        *temp = get;
        putc(get);
        get = (char)getc();
        temp++;
    }
    *temp = '\0';
    */
    while((*s=getc()) != '\r')
    {
        putc(*s++);
    }
    *s = 0;
}


u16 NSEC = 2;
char buf1[BLK], buf2[BLK];




u16 getblk(u16 blk, char *buf)
{
    readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);

    // readfd( blk/18, ((blk)%18)/9, ( ((blk)%18)%9)<<1, buf);
}
//u32?u16?
u16 search(INODE *ip, char *name)
{
 // search for name in the data block of INODE; 
 // return its inumber if found
 // else error();
	char c; int i; DIR *dp;

    for(i=0 ; i<12 ;i++)
    {
        if((u16)ip->i_block[i])
        {
            getblk((u16)(ip->i_block[i]), buf2);
            dp = (DIR *)buf2;
         
            while((char *)dp < &buf2[BLK]){
                c = dp->name[dp->name_len];
                 dp->name[dp->name_len] = 0;
               // concat((u16)dp->name_len, dp->name);
                prints(dp->name);putc(' ');  
                   
                if(strcpy(dp->name, name) == 0){
                // prints("found :");   
                // prints(dp->name);
                 prints("\n\r");
                return ((u16)dp->inode);
           
                }
                dp->name[dp->name_len] = c;//restore byte
                dp = (char *)dp + dp->rec_len;             
            } 
       }
       
    }
    error();
}
/*
int concat(u16 length, char *buf){
    
    
    buf[length] = '\0';
    
}
*/
int strcmp(char *str1 , char *str2){
 
   // char *s1;

   //  s1 = str1;
   /* 
    while(1)
    {
        if(*str1 == 0 && *str2 == 0)
        {
            return 1;
        }
        if(*str1 != *str2)
        {
            return 0;
        }

        str1++;
        str2++;
    }
    */
    while(*str1==*str2)
	{
		if(*str1== 0)
			return 0;

			str1++;
			str2++;

	}
    
}


main()
{ 
    u16 i=0,iblk;
    u16 inumber;

    u32 *up;
    GD    *gp;
    INODE *ip;
    INODE *tp;
    DIR   *dp;
//    char inode;
//1. Write YOUR C code to get the INODE of /boot/mtx
//   INODE *ip --> INOD

    getblk(2, buf1);
    gp = (GD*)buf1;
    iblk =(u16*)gp->bg_inode_table;   
    getblk(iblk, buf2);
    ip = (INODE*)buf2;
    ip = ip + 1;//get root inode
   // tp = ip;
   // prints("start search boot\n\r");
    
    inumber = search(ip,"boot")-1;//count from 1
    getblk(iblk + (inumber/8),buf1);
    ip = (INODE *)buf1 + (inumber%8);//mailman algorithn
   // prints("start search mtx\n\r");
    inumber = search(ip,"mtx")-1;
    getblk(iblk + (inumber/8),buf1);
    ip = (INODE *)buf1 + (inumber%8);
   // prints("found mtx and got inumber!\n\r");



//   if INODE has indirect blocks: get i_block[12] int buf2[  ]

    if((u16)ip->i_block[12])
    {
        getblk((u16)ip->i_block[12],buf2);
       // prints("have indirect blocks");
    }
    
   
    setes(0x1000);  // MTX loading segment = 0x1000

//3. load 12 DIRECT blocks of INODE into memory
    for (i=0; i<12; i++){
        
        getblk((u16)ip->i_block[i], 0);
        putc('*');
        inces();
        
    }
    

//4. load INDIRECT blocks, if any, into memory
   if (tp->i_block[12]){
     up = (u32 *)buf2;      
     while(*up){
        getblk((u16)*up, 0); putc('.');
        inces();
        up++;
     }
  }
  
  prints("go?"); getc();
}  
