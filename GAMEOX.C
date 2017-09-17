#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>

char a[101][101];
int wid=0,turn=0,out=0,ny=0,aiturn=3,aix=0,aiy=0,ai_org_x,ai_org_y;
int ERROR_CODE=-44;
int directs[24][2]={
  {-1,0},
  {1,0},
  {0,-1},
  {0,1},
  {-1,-1},
  {-1,1},
  {1,-1},
  {1,1},
  {2,0},{2,1},{2,2},{1,2},
  {0,2},{-1,2},{-2,2},{-2,1},
  {-2,0},{-2,-1},{-2,-2},{-1,-2},
  {0,-2},{1,-2},{2,-2},{2,-1}
};


void clear();
int read_file();
int write_file();
void clrscr();
int aifight(int x1,int y1,int org_x,int org_y);
void ai();
int scan();
void fight(int x1,int y1);
void display(int n);
void game();
void reset(int x);
void start();
int safe_cvrt_num(char *raw);
int safe_get_num(FILE *stream,int n, ...);
int inMap(int x,int y);
int process_input(int x,int y,int org_x,int org_y);
int change_turn();
int legal_pos(int org_x,int org_y,int x,int y,char ch_flag);

int legal_pos(int org_x,int org_y,int x,int y,char ch_flag)
{
  if ((a[org_x][org_y]==ch_flag)&&(abs(org_x-x)<=2)&&(abs(org_y-y)<=2))
    return 0;
  return -1;
}

int change_turn()
{
  if ((turn+1)%2==0) turn=0;
  else turn=1;
  return 0;
}

int process_input(int x,int y,int org_x,int org_y)
{
  char ch_flag=0;
  if (turn==0) ch_flag='X';
  if (turn==1) ch_flag='O';
  if ((inMap(x,y)==1)||(inMap(org_x,org_y)==1)) return -1;

  if ((a[x][y]==' ')&&(legal_pos(org_x,org_y,x,y,ch_flag)==0))
  {
    if ((abs(org_x-x)<=1)&&(abs(org_y-y)<=1))
    {
      a[x][y]=ch_flag;
      fight(x,y);
      return 0;
    }
    if ((abs(org_x-x)==2)||(abs(org_y-y)==2))
    {
      a[x][y]=ch_flag;
      a[org_x][org_y]=' ';
      fight(x,y);
      return 0;
    }
  }
  return -1;
}

int inMap(int x,int y)
{
	if (x<1||x>wid||y<1||y>wid)
		return 1;
	return 0;
}

int safe_get_num(FILE *stream,int n, ...)
{
  int *targets[100]={NULL};
  int temp=0,num=0,count=-1,i=0;
  char raw[100]={0};
  char *last=NULL,*p=NULL;
  int ch=0;

  num=n;
  if ((n>100)||(n<=0)) return ERROR_CODE;
  va_list args;
  va_start(args,n);
  for (i=0;i<n;i++)
  {
    targets[i]=va_arg(args,int *);
  }
  va_end(args);
  fgets(raw,100,stream);
  if (raw[strlen(raw)-1]!='\n')
    while ((ch=fgetc(stream))!='\n'&&ch!=EOF);
  if (strcmp(raw,"")==0) return ERROR_CODE;
  p=strtok_r(raw," ",&last);
  while (p!=NULL)
  {
    temp=safe_cvrt_num(p);
    if (temp!=ERROR_CODE)
    {
      count++;
      *targets[count]=temp;
      if (count==num-1) break;
    }
    p=strtok_r(NULL," ",&last);
  }
  return count+1;
}

int safe_cvrt_num(char *raw)
{
  char *endptr=NULL;
  long val=0;
  errno=0;
  val=strtol(raw,&endptr,10);
  if ((errno==ERANGE&&(val==LONG_MAX||val==LONG_MIN))||(errno!=0&&val==0)) return ERROR_CODE;
  if ((val>INT_MAX)||(val<INT_MIN)) return ERROR_CODE;
  if (endptr == raw) return ERROR_CODE;
  return (int) val;
}

void clear()
{
  int i=0,j=0;
  for (i=0;i<=100;i++)
    for (j=0;j<=100;j++)
      a[i][j]='!';
}

int read_file()
{
  int i=0,j=0,wid_old=0,aiturn_old=0,turn_old=0;
  char fpath[100]={0};
  int ch=0,r=0;
  FILE *fp;
  clrscr();
  printf("[OPEN] Please input the path of the file:\n");
  if (fgets(fpath,100,stdin)==NULL)
  {
    printf("[OPEN] Please input the file path!\n");
    return -3;
  }
  if (fpath[strlen(fpath)-1]!='\n') while ((ch=getchar())!='\n'&&ch!=EOF);
  if (fpath[strlen(fpath)-1]=='\n') fpath[strlen(fpath)-1]=0;
  if ((fp=fopen(fpath,"r"))==NULL)
  {
    printf("[OPEN] ERROR! Can't open the file!\n");
    getchar();
    clrscr();
    return -1;
  }
  wid_old=wid;
  r=safe_get_num(fp,1,&wid);
  if ((wid<3)||(r!=1)||(wid>100))
  {
    printf("[OPEN] Digital ERROR!\n");
    wid=wid_old;
    getchar();
    clrscr();
    return -2;
  }

  aiturn_old=aiturn;
  r=safe_get_num(fp,1,&aiturn);
  if (((aiturn!=0)&&(aiturn!=1)&&(aiturn!=3))||(r!=1))
  {
    printf("[OPEN] Digital ERROR!\n");
    wid=wid_old;
    aiturn=aiturn_old;
    getchar();
    clrscr();
    return -2;
  }

  turn_old=turn;
  r=safe_get_num(fp,1,&turn);
  if (((turn!=0)&&(turn!=1))||(r!=1)||(aiturn==turn))
  {
    printf("[OPEN] Digital ERROR!\n");
    wid=wid_old;
    aiturn=aiturn_old;
    turn=turn_old;
    getchar();
    clrscr();
    return -2;
  }

  clear();
  for (i=1;i<=wid;i++)
  {
    for (j=1;j<=wid;j++)
      a[i][j]=(char) fgetc(fp);
    fgetc(fp);
  }
  fclose(fp);
  clrscr();
  return 0;
}

int write_file()
{
  int i=0,j=0;
  char fpath[100]={0};
  int ch=0;
  FILE *fp;
  clrscr();
  printf("[SAVE] Please input the path of the file:\n");
  if (fgets(fpath,100,stdin)==NULL)
  {
    printf("[SAVE] Please input the file path!\n");
    return -3;
  }
  if (fpath[strlen(fpath)-1]!='\n') while ((ch=getchar())!='\n'&&ch!=EOF);
  if (fpath[strlen(fpath)-1]=='\n') fpath[strlen(fpath)-1]=0;
  if ((fp=fopen(fpath,"w"))==NULL)
  {
    printf("[SAVE] ERROR! Can't open the file!\n");
    getchar();
    clrscr();
    return -1;
  }
  fprintf(fp,"%d \n",wid);
  fprintf(fp,"%d \n",aiturn);
  fprintf(fp,"%d \n",turn);
  for (i=1;i<=wid;i++)
  {
    for (j=1;j<=wid;j++)
      fprintf(fp,"%c",a[i][j]);
    fprintf(fp,"\n");
  }
  fclose(fp);
  clrscr();
  return 0;
}

void clrscr()
{
  printf("\033c");
}

int aifight(int x1,int y1,int org_x,int org_y)
{
  int sum=0,i=0;
  char ch_flag=0;
  if (aiturn==0) ch_flag='O';
  else ch_flag='X';

  for (i=0;i<8;i++)
    if (inMap(x1+directs[i][0],y1+directs[i][1])==0)
      if (a[x1+directs[i][0]][y1+directs[i][1]]==ch_flag)
        sum++;
  if ((abs(org_x-x1)==2)||(abs(org_y-y1)==2)) sum--;
  return sum;
}

void ai()
{
  int i=0,j=0,sum=0,max=-1,k=0;
  char ch_flag=0;
  if (aiturn==0) ch_flag='X';
  if (aiturn==1) ch_flag='O';
  for (i=1;i<=wid;i++)
  	for (j=1;j<=wid;j++)
  	{
  	  if (a[i][j]==ch_flag)
      {
        for (k=0;k<24;k++)
          if (inMap(i+directs[k][0],j+directs[k][1])==0)
            if (a[i+directs[k][0]][j+directs[k][1]]==' ')
            {
              sum=aifight(i+directs[k][0],j+directs[k][1],i,j);
              if (sum>max)
              {
                max=sum;
                aix=i+directs[k][0];
                aiy=j+directs[k][1];
                ai_org_x=i;
                ai_org_y=j;
              }
            }
      }
  	}
}

int scan()
{
  int t1=0,t2=0;
  int i=0,j=0,o=0,x=0,k=0;
  for (i=1;i<=wid;i++)
  	for (j=1;j<=wid;j++)
  	{
      for (k=0;k<24;k++)
        if (inMap(i+directs[k][0],j+directs[k][1])==0)
          if (a[i+directs[k][0]][j+directs[k][1]]==' ')
          {
            if (a[i][j]=='X') t1=1;
            if (a[i][j]=='O') t2=1;
          }
      if (a[i][j]=='X') x++;
      if (a[i][j]=='O') o++;
  	}
  if ((t1==1)&&(t2==1)) return 0;
  if ((t1==0)||(t2==0))
  {
    if ((t1==0)&&(t2==1)) o=wid*wid-x;
    if ((t1==1)&&(t2==0)) x=wid*wid-o;
    clrscr();
    display(wid);
    if (x>o) printf("X is winner!(press any key to continue)\n");
    if (x<o) printf("O is winner!(press any key to continue)\n");
    if (x==o) printf("tie!(press any key to continue)\n");
    return 1;
  }
  return 0;
}

void fight(int x1,int y1)
{
  int i=0;
  char ch_flag=0,another_ch_flag=0;
  if (turn==0) {ch_flag='O';another_ch_flag='X';}
  else {ch_flag='X';another_ch_flag='O';}
  for (i=0;i<8;i++)
    if (inMap(x1+directs[i][0],y1+directs[i][1])==0)
      if (a[x1+directs[i][0]][y1+directs[i][1]]==ch_flag)
        a[x1+directs[i][0]][y1+directs[i][1]]=another_ch_flag;
}

void display(int n)
{
  int i=0,j=0;
  clrscr();
  printf("\n");
  for (i=1;i<=n+1;i++)
  {
  	if ((i==1)||(i==n+1))
  	{
  	  if (i==1) printf("%c%c%c%c",'=','=','=','=');
  	  if (i==n+1) printf("%c  ",'=');
  	}
  	else printf("%c%c%c%c",'=','=','=','=');
  }
  printf("\n");
  for (i=1;i<=n*2-1;i++)
  {
  	for (j=1;j<=n+1;j++)
  	{
  	  if (i%2==0)
  	  {
    		if ((j==1)||(j==n+1))
    		{
    		  if (j==1) printf("%c%c%c%c",'=','=','=','=');
    		  if (j==n+1) printf("%c",'=');
    		}
        else
    		  printf("%c%c%c%c",'=','=','=','=');
  	  }
  	  else
  	  {
    		if (j==n+1) printf("%c",'|');
        else
    		  printf("%c %c ",'|',a[(i+1)/2][j]);
  	  }
  	}
  	printf("\n");
  }
  for (i=1;i<=n+1;i++)
  {
  	if ((i==1)||(i==n+1))
  	{
  	  if (i==1) printf("%c%c%c%c",'=','=','=','=');
  	  if (i==n+1) printf("%c  ",'=');
  	}
	  else printf("%c%c%c%c",'=','=','=','=');
  }
  printf("\n");
}

void game()
{
  int org_x=0,org_y=0,x=0,y=0,ch=0;

  x=-5;y=-5;org_x=-5;org_y=-5;
	clrscr();
	display(wid);
	if (turn==0) printf("Now is X\n");
	else printf("Now is O\n");
  if (turn!=aiturn)
  {
    printf("please input four numbers of original_x,original_y,x,y:");
    safe_get_num(stdin,4,&org_x,&org_y,&x,&y);
  }
  else
  {
    printf("AI!\n");ai();
    x=aix;y=aiy;
    org_x=ai_org_x;org_y=ai_org_y;
    printf("AI's original_x=%d original_y=%d x=%d y=%d\n",ai_org_x,ai_org_y,aix,aiy);
    printf("Input any key to continue\n");
    getchar();
    while ((ch=getchar())!='\n'&&ch!=EOF);
  }
	if ((x==0)&&(y==0)&&(org_x==0)&&(org_y==0)) {change_turn();return;}
  if ((x==-1)&&(y==-1)&&(org_x==-1)&&(org_y==-1)) {read_file();if (scan()==1) out=1;return;}
  if ((x==-2)&&(y==-2)&&(org_x==-2)&&(org_y==-2)) {write_file();if (scan()==1) out=1;return;}
	if ((x==-3)&&(y==-3)&&(org_x==-3)&&(org_y==-3)) {out=1;return;}
  if (process_input(x,y,org_x,org_y)==0) change_turn();
  if (scan()==1) {out=1;return;}
}
void reset(int x)
{
  int i=0,j=0;
  for (i=1;i<=x;i++)
  	for (j=1;j<=x;j++)
  	  a[i][j]=' ';
  a[1][1]='X';
  a[1][x]='O';
  a[x][1]='O';
  a[x][x]='X';
}
void start()
{
  int ch=0,r=0;
  clear();
  printf("Welcome to GAMEOX!\n");
  printf("Copyright (2015-2017) LEXUGE\n");
  printf("The software is using GPL(http://www.gnu.org/licenses/gpl.txt)\n");
  getchar();
  while ((ch=getchar())!='\n'&&ch!=EOF);
  clrscr();

  printf("Please input a number of width:");
  r=safe_get_num(stdin,1,&wid);
  if ((wid<3)||(r!=1)||(wid>100)) {printf("input again!\n");return;}
  reset(wid);
  clrscr();

  printf("Please input a number of first player(0:X/1:O):");
  r=safe_get_num(stdin,1,&turn);
  if (((turn!=0)&&(turn!=1))||(r!=1)) {printf("input again!\n");return;}
  clrscr();

  printf("Do you want to play with AI?(0:No/1:Yes):");
  r=safe_get_num(stdin,1,&ny);
  if (((ny!=0)&&(ny!=1))||(r!=1)) {printf("input again!\n");return;}
  if (ny==1) {aiturn=1-turn;}
  display(wid);

  while (1)
  {
    game();
    if (out==1) break;
  }
}

int main()
{
  clrscr();
  start();
  return 0;
}
