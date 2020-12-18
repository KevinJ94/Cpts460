#define BLKSIZE 1024
#define BLK 1024
char* RAMdisk = (char*)0x4000000;

char *buf1[BLKSIZE],*buf2[BLKSIZE],*buf[BLKSIZE];
GD *gp;
SUPER *sp;   
INODE *ip;
u32 iblk;
DIR   *dp;
u32 inumber;
char *name = 0;

int get_block(u32 blk, char *buf)
{
    char* cp = RAMdisk + blk * BLKSIZE;
    memcpy(buf, cp, BLKSIZE);
}

u32 search(INODE *ip, char *myname, char* mybuf)
{
 // search for name in the data block of INODE; 
 // return its inumber if found
 // else error();
	char c; int i; DIR *dp;
	name =myname;
    //kprintf("name = %s",name);
    for(i=0 ; i<12 ;i++)
    {
        if(ip->i_block[i])
        {
            get_block((ip->i_block[i]), mybuf);
            dp = (DIR *)mybuf;
         
            while((char *)dp < &mybuf[BLK]){
                c = dp->name[dp->name_len];
                 dp->name[dp->name_len] = 0;
              
                //kprintf("%s\n",dp->name);  
                //kprintf("strcmp %d",strcmp(dp->name, name));   
                if(strcmp(dp->name, name) == 0){
                kprints("found :");   
                kprints(dp->name);
                kprints("\n\r");
                return (dp->inode);
           
                }
                dp->name[dp->name_len] = c;//restore byte
                dp = (char *)dp + dp->rec_len;             
            } 
       }
       
    }

}

int loader(char *filename, PROC *p)
{
   char *addr = (char *)(0x800000 + (p->pid - 1)*0x100000);
   char *cp, *cq;
   int i;
   /*
   get_block(1, buf1);
   sp = (SUPER*)buf1;
   if(sp->s_magic == 0xEF53)
   {
        kprintf("magic number = %x EXT2 FS\n",sp->s_magic);
   }
   else
   {
        kprintf("FS TYPE ERROR");
        return 0;
   }
   */
   get_block(2,buf1);
   gp = (GD*)buf1;
   //kprintf("got GD\n");
   iblk = gp->bg_inode_table;
   get_block(iblk, buf2);
   ip = (INODE*)buf2;
   ip = ip + 1;//get root inode
   //search dir
   inumber = search(ip,"bin",buf2)-1;//count from 1
   get_block(iblk + (inumber/8),buf1);
   ip = (INODE *)buf1 + (inumber%8);//mailman algorithn
   inumber = search(ip,filename,buf2)-1;
   get_block(iblk + (inumber/8),&buf1);
   ip = (INODE *)buf1 + (inumber%8);
   //get inide -> find datablock i_block[15]
    
   //copy to process addr
   cp = RAMdisk + ip->i_block[0] * BLKSIZE;//file start address 
   cq = addr;//umode process start address
   for (i=0; i< ip->i_size; i++){
         *cq++ = *cp++;
   }
}



