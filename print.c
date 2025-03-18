#include <stdio.h>
#include <stdlib.h>

int main() {
    char buffer[256];
 // Simulate printing by reading lines from stdin and outputting them
    printf("Print: Waiting for document from gendoc...\n");
    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        printf("Print: %s", buffer);  // Output what we "print"
    }
    printf("Print: Document finished printing!\n");
    return 0;
}
