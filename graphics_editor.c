#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 60
#define HEIGHT 20
#define MAX_SHAPES 100

// Define shape type enum
typedef enum {
    SHAPE_LINE = 1,
    SHAPE_RECTANGLE,
    SHAPE_CIRCLE,
    SHAPE_TRIANGLE
} ShapeType;

// Coordinate details for shapes
typedef struct {
    int x1, y1;
    int x2, y2;
} LineParams;

typedef struct {
    int x, y;
    int width, height;
} RectParams;

typedef struct {
    int cx, cy;
    int radius;
} CircleParams;

typedef struct {
    int x1, y1;
    int x2, y2;
    int x3, y3;
} TriangleParams;

// Main shape registry struct
typedef struct {
    int id;
    ShapeType type;
    union {
        LineParams line;
        RectParams rect;
        CircleParams circle;
        TriangleParams triangle;
    } params;
} Shape;

// Global State
char canvas[HEIGHT][WIDTH];
Shape shapes[MAX_SHAPES];
int shape_count = 0;
int next_id = 1;

// Prototypes
void clear_canvas(void);
void render_canvas(void);
void display_canvas(void);
void draw_line_on_canvas(int x1, int y1, int x2, int y2);
void draw_rect_on_canvas(int x, int y, int w, int h);
void draw_circle_points(int cx, int cy, int x, int y);
void draw_circle_on_canvas(int cx, int cy, int radius);
void draw_triangle_on_canvas(int x1, int y1, int x2, int y2, int x3, int y3);

void safe_read_line(char *buffer, int size);
int get_int_input(const char *prompt, int min_val, int max_val, int *output);
void add_shape(void);
void delete_shape(void);
void modify_shape(void);
void list_shapes(void);
int find_shape_index_by_id(int id);
void clear_all_shapes(void);

// Implementations

void clear_canvas() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            canvas[y][x] = '_';
        }
    }
}

void draw_line_on_canvas(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1);
    int sy = y1 < y2 ? 1 : -1;
    int err = dx + dy;
    
    while (1) {
        if (x1 >= 0 && x1 < WIDTH && y1 >= 0 && y1 < HEIGHT) {
            canvas[y1][x1] = '*';
        }
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void draw_rect_on_canvas(int x, int y, int w, int h) {
    // Top and bottom edges
    for (int i = 0; i < w; i++) {
        int px = x + i;
        if (px >= 0 && px < WIDTH) {
            if (y >= 0 && y < HEIGHT) canvas[y][px] = '*';
            if (y + h - 1 >= 0 && y + h - 1 < HEIGHT) canvas[y + h - 1][px] = '*';
        }
    }
    // Left and right edges
    for (int i = 0; i < h; i++) {
        int py = y + i;
        if (py >= 0 && py < HEIGHT) {
            if (x >= 0 && x < WIDTH) canvas[py][x] = '*';
            if (x + w - 1 >= 0 && x + w - 1 < WIDTH) canvas[py][x + w - 1] = '*';
        }
    }
}

void draw_circle_points(int cx, int cy, int x, int y) {
    int px[] = {cx + x, cx - x, cx + x, cx - x, cx + y, cx - y, cx + y, cx - y};
    int py[] = {cy + y, cy + y, cy - y, cy - y, cy + x, cy + x, cy - x, cy - x};
    for (int i = 0; i < 8; i++) {
        if (px[i] >= 0 && px[i] < WIDTH && py[i] >= 0 && py[i] < HEIGHT) {
            canvas[py[i]][px[i]] = '*';
        }
    }
}

void draw_circle_on_canvas(int cx, int cy, int radius) {
    if (radius < 0) return;
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;
    
    draw_circle_points(cx, cy, x, y);
    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        draw_circle_points(cx, cy, x, y);
    }
}

void draw_triangle_on_canvas(int x1, int y1, int x2, int y2, int x3, int y3) {
    draw_line_on_canvas(x1, y1, x2, y2);
    draw_line_on_canvas(x2, y2, x3, y3);
    draw_line_on_canvas(x3, y3, x1, y1);
}

void render_canvas() {
    clear_canvas();
    for (int i = 0; i < shape_count; i++) {
        Shape s = shapes[i];
        switch (s.type) {
            case SHAPE_LINE:
                draw_line_on_canvas(s.params.line.x1, s.params.line.y1, s.params.line.x2, s.params.line.y2);
                break;
            case SHAPE_RECTANGLE:
                draw_rect_on_canvas(s.params.rect.x, s.params.rect.y, s.params.rect.width, s.params.rect.height);
                break;
            case SHAPE_CIRCLE:
                draw_circle_on_canvas(s.params.circle.cx, s.params.circle.cy, s.params.circle.radius);
                break;
            case SHAPE_TRIANGLE:
                draw_triangle_on_canvas(s.params.triangle.x1, s.params.triangle.y1,
                                       s.params.triangle.x2, s.params.triangle.y2,
                                       s.params.triangle.x3, s.params.triangle.y3);
                break;
        }
    }
}

void display_canvas() {
    printf("\n");
    // Print column header numbers (every 5 steps)
    printf("     ");
    for (int x = 0; x < WIDTH; x++) {
        if (x % 5 == 0) {
            printf("%-5d", x);
        }
    }
    printf("\n");

    // Print ticks
    printf("     ");
    for (int x = 0; x < WIDTH; x++) {
        if (x % 5 == 0) {
            printf("|    ");
        }
    }
    printf("\n");

    // Top border line
    printf("   +");
    for (int x = 0; x < WIDTH; x++) {
        printf("-");
    }
    printf("+\n");

    // Canvas contents row by row
    for (int y = 0; y < HEIGHT; y++) {
        printf("%2d |", y);
        for (int x = 0; x < WIDTH; x++) {
            printf("%c", canvas[y][x]);
        }
        printf("|\n");
    }

    // Bottom border line
    printf("   +");
    for (int x = 0; x < WIDTH; x++) {
        printf("-");
    }
    printf("+\n\n");
}

void safe_read_line(char *buffer, int size) {
    if (fgets(buffer, size, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    }
}

int get_int_input(const char *prompt, int min_val, int max_val, int *output) {
    char buffer[100];
    int value;
    while (1) {
        printf("%s", prompt);
        safe_read_line(buffer, sizeof(buffer));
        // Empty input handling
        if (strlen(buffer) == 0) {
            continue;
        }
        if (sscanf(buffer, "%d", &value) == 1) {
            if (value >= min_val && value <= max_val) {
                *output = value;
                return 1;
            } else {
                printf("Error: Input must be between %d and %d.\n", min_val, max_val);
            }
        } else {
            printf("Error: Invalid input. Please enter an integer.\n");
        }
    }
}

void list_shapes() {
    if (shape_count == 0) {
        printf("No shapes in the picture currently.\n");
        return;
    }
    printf("\n=================== ACTIVE SHAPES ===================\n");
    for (int i = 0; i < shape_count; i++) {
        Shape s = shapes[i];
        printf("ID %2d | ", s.id);
        switch (s.type) {
            case SHAPE_LINE:
                printf("Line      | Point 1: (%d, %d), Point 2: (%d, %d)\n",
                       s.params.line.x1, s.params.line.y1,
                       s.params.line.x2, s.params.line.y2);
                break;
            case SHAPE_RECTANGLE:
                printf("Rectangle | Top-Left: (%d, %d), Width: %d, Height: %d\n",
                       s.params.rect.x, s.params.rect.y,
                       s.params.rect.width, s.params.rect.height);
                break;
            case SHAPE_CIRCLE:
                printf("Circle    | Center: (%d, %d), Radius: %d\n",
                       s.params.circle.cx, s.params.circle.cy,
                       s.params.circle.radius);
                break;
            case SHAPE_TRIANGLE:
                printf("Triangle  | P1: (%d, %d), P2: (%d, %d), P3: (%d, %d)\n",
                       s.params.triangle.x1, s.params.triangle.y1,
                       s.params.triangle.x2, s.params.triangle.y2,
                       s.params.triangle.x3, s.params.triangle.y3);
                break;
        }
    }
    printf("=====================================================\n");
}

int find_shape_index_by_id(int id) {
    for (int i = 0; i < shape_count; i++) {
        if (shapes[i].id == id) {
            return i;
        }
    }
    return -1;
}

void add_shape() {
    if (shape_count >= MAX_SHAPES) {
        printf("Error: Maximum shape limit reached (%d shapes).\n", MAX_SHAPES);
        return;
    }
    
    printf("\n--- Add Shape Menu ---\n");
    printf("1. Line\n");
    printf("2. Rectangle\n");
    printf("3. Circle\n");
    printf("4. Triangle\n");
    printf("5. Cancel and return\n");
    
    int choice;
    get_int_input("Enter choice (1-5): ", 1, 5, &choice);
    if (choice == 5) return;
    
    Shape s;
    s.id = next_id++;
    s.type = (ShapeType)choice;
    
    switch (s.type) {
        case SHAPE_LINE: {
            int x1, y1, x2, y2;
            get_int_input("Enter x1 (0 to 59): ", 0, WIDTH - 1, &x1);
            get_int_input("Enter y1 (0 to 19): ", 0, HEIGHT - 1, &y1);
            get_int_input("Enter x2 (0 to 59): ", 0, WIDTH - 1, &x2);
            get_int_input("Enter y2 (0 to 19): ", 0, HEIGHT - 1, &y2);
            s.params.line.x1 = x1;
            s.params.line.y1 = y1;
            s.params.line.x2 = x2;
            s.params.line.y2 = y2;
            break;
        }
        case SHAPE_RECTANGLE: {
            int x, y, w, h;
            get_int_input("Enter top-left x (0 to 59): ", 0, WIDTH - 1, &x);
            get_int_input("Enter top-left y (0 to 19): ", 0, HEIGHT - 1, &y);
            get_int_input("Enter width (1 to 60): ", 1, WIDTH, &w);
            get_int_input("Enter height (1 to 20): ", 1, HEIGHT, &h);
            s.params.rect.x = x;
            s.params.rect.y = y;
            s.params.rect.width = w;
            s.params.rect.height = h;
            break;
        }
        case SHAPE_CIRCLE: {
            int cx, cy, radius;
            get_int_input("Enter center x (0 to 59): ", 0, WIDTH - 1, &cx);
            get_int_input("Enter center y (0 to 19): ", 0, HEIGHT - 1, &cy);
            // Limit radius to a reasonable size to prevent full canvas wipeout
            get_int_input("Enter radius (1 to 30): ", 1, 30, &radius);
            s.params.circle.cx = cx;
            s.params.circle.cy = cy;
            s.params.circle.radius = radius;
            break;
        }
        case SHAPE_TRIANGLE: {
            int x1, y1, x2, y2, x3, y3;
            get_int_input("Enter x1 (0 to 59): ", 0, WIDTH - 1, &x1);
            get_int_input("Enter y1 (0 to 19): ", 0, HEIGHT - 1, &y1);
            get_int_input("Enter x2 (0 to 59): ", 0, WIDTH - 1, &x2);
            get_int_input("Enter y2 (0 to 19): ", 0, HEIGHT - 1, &y2);
            get_int_input("Enter x3 (0 to 59): ", 0, WIDTH - 1, &x3);
            get_int_input("Enter y3 (0 to 19): ", 0, HEIGHT - 1, &y3);
            s.params.triangle.x1 = x1;
            s.params.triangle.y1 = y1;
            s.params.triangle.x2 = x2;
            s.params.triangle.y2 = y2;
            s.params.triangle.x3 = x3;
            s.params.triangle.y3 = y3;
            break;
        }
    }
    
    shapes[shape_count++] = s;
    printf("\n>>> Shape %d added successfully! <<<\n", s.id);
    render_canvas();
}

void delete_shape() {
    if (shape_count == 0) {
        printf("No shapes available to delete.\n");
        return;
    }
    list_shapes();
    int id;
    get_int_input("Enter the ID of the shape to delete (or -1 to cancel): ", -1, next_id - 1, &id);
    if (id == -1) return;
    
    int index = find_shape_index_by_id(id);
    if (index == -1) {
        printf("Error: Shape with ID %d not found.\n", id);
        return;
    }
    
    // Shift elements
    for (int i = index; i < shape_count - 1; i++) {
        shapes[i] = shapes[i + 1];
    }
    shape_count--;
    printf("\n>>> Shape %d deleted successfully! <<<\n", id);
    render_canvas();
}

void modify_shape() {
    if (shape_count == 0) {
        printf("No shapes available to modify.\n");
        return;
    }
    list_shapes();
    int id;
    get_int_input("Enter the ID of the shape to modify (or -1 to cancel): ", -1, next_id - 1, &id);
    if (id == -1) return;
    
    int index = find_shape_index_by_id(id);
    if (index == -1) {
        printf("Error: Shape with ID %d not found.\n", id);
        return;
    }
    
    Shape *s = &shapes[index];
    printf("\nModifying Shape %d...\n", s->id);
    switch (s->type) {
        case SHAPE_LINE: {
            printf("Current Line: Point 1 (%d, %d), Point 2 (%d, %d)\n",
                   s->params.line.x1, s->params.line.y1,
                   s->params.line.x2, s->params.line.y2);
            int x1, y1, x2, y2;
            get_int_input("Enter new x1 (0 to 59): ", 0, WIDTH - 1, &x1);
            get_int_input("Enter new y1 (0 to 19): ", 0, HEIGHT - 1, &y1);
            get_int_input("Enter new x2 (0 to 59): ", 0, WIDTH - 1, &x2);
            get_int_input("Enter new y2 (0 to 19): ", 0, HEIGHT - 1, &y2);
            s->params.line.x1 = x1;
            s->params.line.y1 = y1;
            s->params.line.x2 = x2;
            s->params.line.y2 = y2;
            break;
        }
        case SHAPE_RECTANGLE: {
            printf("Current Rectangle: Top-Left (%d, %d), Width %d, Height %d\n",
                   s->params.rect.x, s->params.rect.y,
                   s->params.rect.width, s->params.rect.height);
            int x, y, w, h;
            get_int_input("Enter new top-left x (0 to 59): ", 0, WIDTH - 1, &x);
            get_int_input("Enter new top-left y (0 to 19): ", 0, HEIGHT - 1, &y);
            get_int_input("Enter new width (1 to 60): ", 1, WIDTH, &w);
            get_int_input("Enter new height (1 to 20): ", 1, HEIGHT, &h);
            s->params.rect.x = x;
            s->params.rect.y = y;
            s->params.rect.width = w;
            s->params.rect.height = h;
            break;
        }
        case SHAPE_CIRCLE: {
            printf("Current Circle: Center (%d, %d), Radius %d\n",
                   s->params.circle.cx, s->params.circle.cy,
                   s->params.circle.radius);
            int cx, cy, radius;
            get_int_input("Enter new center x (0 to 59): ", 0, WIDTH - 1, &cx);
            get_int_input("Enter new center y (0 to 19): ", 0, HEIGHT - 1, &cy);
            get_int_input("Enter new radius (1 to 30): ", 1, 30, &radius);
            s->params.circle.cx = cx;
            s->params.circle.cy = cy;
            s->params.circle.radius = radius;
            break;
        }
        case SHAPE_TRIANGLE: {
            printf("Current Triangle: P1(%d, %d), P2(%d, %d), P3(%d, %d)\n",
                   s->params.triangle.x1, s->params.triangle.y1,
                   s->params.triangle.x2, s->params.triangle.y2,
                   s->params.triangle.x3, s->params.triangle.y3);
            int x1, y1, x2, y2, x3, y3;
            get_int_input("Enter new x1 (0 to 59): ", 0, WIDTH - 1, &x1);
            get_int_input("Enter new y1 (0 to 19): ", 0, HEIGHT - 1, &y1);
            get_int_input("Enter new x2 (0 to 59): ", 0, WIDTH - 1, &x2);
            get_int_input("Enter new y2 (0 to 19): ", 0, HEIGHT - 1, &y2);
            get_int_input("Enter new x3 (0 to 59): ", 0, WIDTH - 1, &x3);
            get_int_input("Enter new y3 (0 to 19): ", 0, HEIGHT - 1, &y3);
            s->params.triangle.x1 = x1;
            s->params.triangle.y1 = y1;
            s->params.triangle.x2 = x2;
            s->params.triangle.y2 = y2;
            s->params.triangle.x3 = x3;
            s->params.triangle.y3 = y3;
            break;
        }
    }
    printf("\n>>> Shape %d modified successfully! <<<\n", s->id);
    render_canvas();
}

void clear_all_shapes() {
    shape_count = 0;
    render_canvas();
    printf("\n>>> Canvas cleared of all shapes! <<<\n");
}

int main() {
    clear_canvas();
    
    printf("==================================================\n");
    printf("          Welcome to the 2D Graphics Editor        \n");
    printf("==================================================\n");
    printf(" Canvas size: %dx%d. Coordinate indices: x [0..%d], y [0..%d].\n",
           WIDTH, HEIGHT, WIDTH - 1, HEIGHT - 1);
    printf(" Empty pixels are represented by '_', drawn pixels by '*'.\n");
    
    int choice = 0;
    while (choice != 6) {
        printf("\n================ MAIN MENU ================\n");
        printf("1. Add a shape\n");
        printf("2. Delete a shape\n");
        printf("3. Modify a shape\n");
        printf("4. Display picture\n");
        printf("5. Clear all shapes\n");
        printf("6. Exit program\n");
        printf("===========================================\n");
        
        get_int_input("Select menu option (1-6): ", 1, 6, &choice);
        
        switch (choice) {
            case 1:
                add_shape();
                break;
            case 2:
                delete_shape();
                break;
            case 3:
                modify_shape();
                break;
            case 4:
                display_canvas();
                break;
            case 5:
                clear_all_shapes();
                break;
            case 6:
                printf("\nExiting. Thank you for using 2D Graphics Editor!\n");
                break;
        }
    }
    return 0;
}
