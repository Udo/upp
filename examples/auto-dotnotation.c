#binary exe "../bin/auto-dotnotation"
#include <stdio.h>
#include <stdlib.h>

struct PointList {
    int x, y;
    PointList *next;
};

int main() {
    PointList* p = (PointList*)malloc(sizeof(PointList));
    PointList* p2 = (PointList*)malloc(sizeof(PointList));
    p->next = p2;

    p.x = 10;
    p.y = 20;
    p2.x = 30;
    p2.y = 40;

    printf("Point 1: (%d, %d)\n", p.x, p.y);
    printf("Point 2: (%d, %d)\n", p.next.x, p.next.y);
    
    return 0;
}
