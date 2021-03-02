#include <stdio.h>

struct intSet {
    int sz, capacity;
    int *list;
};

int main() {
    printf("%d\n", sizeof(struct intSet));
    return 0;
}