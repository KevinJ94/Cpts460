9. Assume: INODE *ip points at a DIRectory INODE

9-1: print entries of this DIRectory

 INODE *ip ---> a DIR INODE
 DIR *dp;
 char *cp;
 char dbuf[BLKSIZE], temp[256];
 int i;

 for (i=0; i < 12; i++){  // ASSUME DIRs have at most 12 direct data blocks
   if (ip->i_block[i]==0)
      break;
   get_block(fd, ip->i_block[i], dbuf);
   dp = (DIR *)dbuf;
   cp = dbuf;

   while(sp < dbuf + BLKSIZE){
      // copy dp->name into temp[] and make it a string
      printf("%d  %d  %d  %s\n", dp->inode, dp->rec_len, dp->name_len, temp);
     
      cp += dp->rec_len;
      dp = (DIR *)cp;
   }
 }

9-2. For each pair of (inode number, name) of a DIR entry, 

(1). Write an INODE *iget(fd, ino) function, which gets its INODE in memory 
     and return the INODE pointer


(2). Print information of INODE as in ls -l:

  int ls_ip(INODE *ip, char *name);
  {
    u16 mode;
    int mask, k;

    mode = ip->i_mode;
    if ((mode & 0xF000) == 0x4000)
        putchar('d');

    if ((mode & 0xF000) == 0xA000)
        putchar('l');
    else if ((mode & 0xF000) == 0x8000)
        puchar('-');

    mask = 000400;
    for (k=0; k<3; k++){
      if (mode & mask)
         kputc('r');
      else
         kputc('-');
      mask = mask >> 1;
      if (mode & mask)
         kputc('w');
      else
         kputc('-');
      mask = mask >> 1;
      if (mode & mask)
         kputc('x');
      else
         kputc('-');
      mask = mask >> 1;
    }    
    printf(" %d ", ip->i_links_count);
    printf(" %d  %d", ip->i_uid, ip->i_gid);
    printf("%d ", ip->i_size);
    printf("%s ", ctime(&ip->i_ctime));
    printf("%s", name);

  printf("\n");     
}
