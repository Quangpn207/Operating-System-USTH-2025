#include <stdio.h>
#include <unistd.h>

int main() {
int j,i = 0;
switch (j=fork()) {
	case -1 : perror("fork"); break;
	case  0 : i++; printf("child :%d\n",i); break;
	default : 
		for (int t=0;t<1000000;t++);
		printf("father :%d\n",i);	
    }   
}