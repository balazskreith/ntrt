#include <stdio.h>

int main()
{
int i, szum=0;
printf("A program elszamol 0 tol 100 ig és összeadja a paros szamokat\n");

for(i=1;i<101;++i){

if(i % 2 ==0)
szum=szum+i;
}

printf("%d",szum);
// system("PAUSE");
return 0;
}
