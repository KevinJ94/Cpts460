/*******************************************************
 *                      t.c file                        *
 *******************************************************/
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

#define TRK 18
#define CYL 36
#define BLK 1024

#include "ext2.h"
typedef struct ext2_group_desc GD;
typedef struct ext2_inode INODE;
typedef struct ext2_dir_entry_2 DIR;
GD* gp;
INODE* ip;
DIR* dp;

char buf1[BLK], buf2[BLK];
int color = 0x0A;
u8 ino;

main()
{
    u16 i, iblk;
    char c, temp[64], buf[BLK];
    char *p, inode;
    char intochar[11];

    prints("read block# 2 (GD)\n\r");
    getblk(2, buf1);

    // 1. WRITE YOUR CODE to get iblk = bg_inode_table block number
    gp = (GD*)buf1;
    iblk = (u16*)gp->bg_inode_table;
    prints("inode_block=");
    putc(iblk + '0');
    prints("\n\r");

    // 2. WRITE YOUR CODE to get root inode
    prints("read inodes begin block to get root inode\n\r");
    getblk(iblk, buf2);
    ip = (INODE*)buf2;
    ip = ip + 1;

    // 3. WRITE YOUR CODE to step through the data block of root inode
    prints("read data block of root DIR\n\r");
    getblk((u16)(ip->i_block[0]), buf);
    dp = (DIR*)buf;
    p = buf;
    while (p < buf + 1024) {
        concat((u16)dp->name_len, dp->name);
        prints(dp->name);
        if (strcmp(dp->name, "boot")) {
            getc();
            prints(" <----- This is boot.");
            //inode = &dp->inode;
            //itoa(dp->inode, intochar );
            //prints(intochar);
        }
        prints("\n\r");
        getc();
        p += dp->rec_len;
        dp = (DIR*)p;
    }

    // 4. print file names in the root directory /
}

int prints(char* s)
{
    char* temp;
    for (temp = s; *temp != '\0'; temp++) {
        putc(*temp);
    }
}
/*
   int itoa(u32 num, char *a){
   u32 base = 10;
   u32 rem;
   int i = 0;int j=0;
   char temp;
   char* end;
   while (num != 0) {
   rem = num % base;
   a[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
   num = num/base;
   }
   a[i] = '\0';
   }
   */
int gets(char* s)
{
    char* temp = s;
    char get = (char)getc();
    while (get != 0x0D) {
        *temp = get;
        putc(get);
        get = (char)getc();
        temp++;
    }
    *temp = '\0';
}

int getblk(u16 blk, char* buf)
{
    // readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);
    readfd(blk / 18, ((blk) % 18) / 9, (((blk) % 18) % 9) << 1, buf);
}

int concat(u16 length, char* buf)
{
    buf[length] = '\0';
}

int strcmp(char* str1, char* str2)
{
    char* p1 = str1;
    char* p2 = str2;
    int k = 0;
    while (1) {
        if (*p1 == '\0' && *p2 == '\0') {
            return 1;
        }
        if (*p1 != *p2) {
            return 0;
        }

        p1++;
        p2++;
    }
}
