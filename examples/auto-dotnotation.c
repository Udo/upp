#binary exe "../bin/auto-dotnotation"
#include <stdio.h>
#include <stdlib.h>

struct PointList {
    int x, y;
    PointList *next;
};

int main() {
    PointList p0 = {y : 22};
    PointList* p1 = (PointList*)malloc(sizeof(PointList));
    PointList* p2 = (PointList*)malloc(sizeof(PointList));
    p1->next = p2;

    p1.x = 10;
    p1.y = 20;
    p2.x = 30;
    p2.y = 40;

    printf("Point 0: (%d, %d, %p)\n", p0.x, p0.y, (void*)&p0);
    printf("Point 1: (%d, %d, %p)\n", p1.x, p1.y, (void*)&p1);
    printf("Point 2: (%d, %d, %p)\n", p1.next.x, p1.next.y, (void*)&p2);
    
    return 0;
}
