#include<stdlib.h>
#include<stdio.h>

int func(int n) {
	{
		if (n == 0) return 0;
		else return n - func(func(func(n - 1)));
	}
}

int main(void) {
	int n = 8;
	printf("%i", func(n));
	return;
}