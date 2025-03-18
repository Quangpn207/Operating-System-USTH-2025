#include <stdio.h>
int Acompute1() {
    return 100;
}
void Acompute2(int resB) {
    fprintf(stderr,"Acompute2 with B = %d\n", resB);
}
int main() {
    fprintf(stderr, "prgA: started\n");
    int resA = Acompute1();
    fprintf(stderr, "prgA: computed resA = %d\n", resA);
    fflush(stdout);
    fprintf(stderr, "prgA: sent resA, waiting for resB...\n");
    int resB;
    if (scanf("%d", &resB) != 1) {
        fprintf(stderr, "prgA: failed to read resB!\n");
        return 1;
    }
    fprintf(stderr, "prgA: received resB = %d\n", resB);
    Acompute2(resB);
    fprintf(stderr, "prgA: finished\n");
    return 0;
}
