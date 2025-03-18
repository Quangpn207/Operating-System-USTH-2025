#include <stdio.h>
int Bcompute1() {
    return 10000;
}
void Bcompute2(int resA) {
    fprintf(stderr,"Bcompute2 with A = %d\n", resA);
}
int main() {
    fprintf(stderr, "prgB: started\n");
    int resB = Bcompute1();
    fprintf(stderr, "prgB: computed resB = %d\n", resB);
    fflush(stdout);
    fprintf(stderr, "prgB: sent resB, waiting for resA...\n");
    int resA;
    if (scanf("%d", &resA) != 1) {
        fprintf(stderr, "prgB: failed to read resA!\n");
        return 1;
    }
    fprintf(stderr, "prgB: received resA = %d\n", resA);
    Bcompute2(resA);
    fprintf(stderr, "prgB: finished\n");
    return 0;
}
