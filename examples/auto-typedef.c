#binary exe "../bin/auto-typedef"
#include <stdio.h>

/* Define a named struct - automatic typedef will be created */
struct Point {
    int x, y;
};

/* Define a named enum - automatic typedef will be created */
enum Color {
    RED, GREEN, BLUE
};

/* Define a named union - automatic typedef will be created */
union Value {
    int i;
    float f;
    char c;
};

int main() {
    /* Use the struct without 'struct' keyword thanks to auto typedef */
    Point p = {10, 20};
    
    /* Use the enum without 'enum' keyword thanks to auto typedef */
    Color c = RED;
    
    /* Use the union without 'union' keyword thanks to auto typedef */
    Value v;
    v.i = 42;
    
    printf("Point: (%d, %d)\n", p.x, p.y);
    printf("Color: %d\n", c);
    printf("Value as int: %d\n", v.i);
    
    v.f = 3.14f;
    printf("Value as float: %.2f\n", v.f);
    
    return 0;
}
