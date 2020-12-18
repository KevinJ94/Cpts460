/*********  t.c file *********************/

//extern void start();

extern void getc();

extern void putc();

//extern void readfd();

int prints(char *s)
{
    char *temp;
    for (temp = s; *temp != '\0'; temp++){
        putc(*temp);
    }
}

int gets(char *s)
{
    char *temp = s;
    char get = (char)getc();
    while(get != 0x0D){
        *temp = get;
        putc(get);
        get = (char)getc();
        temp++;
    }
    *temp = '\0';
}

char ans[64];

main()
{
  while(1){
    prints("What's your name? ");
    gets(ans);  prints("\n\r");

    if (ans[0]==0){
      prints("return to assembly and hang\n\r");
      return;
    }
    prints("Welcome "); prints(ans); prints("\n\r");
  }
}
  
