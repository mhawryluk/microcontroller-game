#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <bits/stdint-uintn.h>

int max_width = 480;
int max_height = 272;

const int figure_size = 80;
const int circle_radius = figure_size / 2;
const int square_size = figure_size;
const int triangle_height = figure_size;
const int max_figures_number = 10;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t type;
    int16_t score;
    int16_t time_to_live;
    u_int32_t color;
} Figure;

void draw_shape(int x, int y, int shape, u_int32_t color) { // draw it on FG
    // BSP_LCD_SetTextColor(color);
    if (shape == 0) {
        // generate circle
        // BSP_LCD_FillCircle(x, y , circle_radius);
    } else if (shape == 1) {
        // generate square
        // BSP_LCD_FillCircle(x, y , square_size, square_size);
    } else {
        // generate triangle
        // Point points [3] = {{x, y}, {x+triangle_height/2, y+triangle_height},  {x-triangle_height/2, y+triangle_height}};
        // BSP_LCD_FillPolygon(points, 3);
    }
}

int inside_rect(int rect_x, int rect_y, int x, int y) {
    if (x > rect_x && x < rect_x + square_size && y > rect_y && y < rect_y + square_size) {
        return 1;
    }
    return 0;
}

int inside_circle(int circle_x, int circle_y, int x, int y) {
    if(abs(x - circle_x) > circle_radius) return 0;
    if(abs(y - circle_y) > circle_radius) return 0;
    return 1;
}

int sign(int p1_x, int p1_y, int p2_x, int p2_y, int p3_x, int p3_y) {
    return (p1_x - p3_x) * (p2_y - p3_y) - (p2_x - p3_x) * (p1_y - p3_y);
}

int inside_triangle(int triangle_x, int triangle_y, int x, int y) {
    int p1_x = triangle_x;
    int p1_y = triangle_y;
    int p2_x = x + triangle_height / 2;
    int p2_y = y + triangle_height;
    int p3_x = x - triangle_height / 2;
    int p3_y = y + triangle_height;

    int d1, d2, d3;

    d1 = sign(x, y, p1_x, p1_y, p2_x, p2_y);
    d2 = sign(x, y, p2_x, p2_y, p3_x, p3_y);
    d3 = sign(x, y, p3_x, p3_y, p1_x, p1_y);

    int has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    int has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}


// returns index of touched figure, -1 otherwise
int point_inside(int x, int y, Figure figures[max_figures_number], int max_figures_idx) {

    printf("Checking if point (%d, %d) is inside some figure\n", x, y);

    for (int i = max_figures_idx - 1; i >= 0; i--) {
        int type = figures[i].type;
        int fig_x = figures[i].x;
        int fig_y = figures[i].y;

        if (type == 0) {
            if (inside_circle(fig_x, fig_y, x, y)) {
                printf("Inside circle!\n");
                return i;
            }
        } else if (type == 1) {
            if (inside_rect(fig_x, fig_y, x, y)) {
                printf("Inside square!\n");
                return i;
            }
        } else {
            if (inside_triangle(fig_x, fig_y, x, y)) {
                printf("Inside triangle!\n");
                return i;
            }
        }
    }
    printf("Outside!\n");
    return -1;
}

// time to live => we can calculate it using number of main loop iterations

int main() {

    int shape, color_code;
    time_t t;
    srand((unsigned) time(&t));
    int current_figure_idx = 0;
    u_int32_t color;
    Figure figures[max_figures_number];
    int x, y;
    int score_number;
    int score_sum = 0;
    int time_to_live;

    // in each iteration of the main loop we check if user touched one of the figures
    // in each iteration we redraw all figures on the foreground
    // in each two iterations we add new figure

    for (int i = 0; i < 20; i++) { //should be infinite loop


        printf("-------------------------------------------\n");
        printf("Iteration number: %d \n", i);
        printf("Curr idx: %d \n", current_figure_idx);


        printf("Drawing figures:\n");
        for (int j = 0; j < current_figure_idx; j++) {
            printf("  *  x: %d, y: %d, type: %d, ttl: %d\n", figures[j].x, figures[j].y, figures[j].type, figures[j].time_to_live);
        }
        for (int j = 0; j < current_figure_idx; j++) {
            if(figures[j].time_to_live <= 0){
                for (int k = j; k < current_figure_idx - 1; k++) {
                    figures[k] = figures[k + 1];
                }
                current_figure_idx -=1;
                j -=1;
            } else{
                draw_shape(figures[j].x, figures[j].y, figures[j].type, figures[j].color);
                figures[j].time_to_live -= 1;
            }
        }
        printf("\n");


        int cursorX = 100;
        int cursorY = 100;

        int idx = point_inside(cursorX, cursorY, figures, current_figure_idx);
        if (idx != -1) {
            printf("Removing figure with idx %d \n", idx);
            score_sum += figures[idx].score;
            for (int k = idx; k < current_figure_idx - 1; k++) {
                figures[k] = figures[k + 1];
            }
            current_figure_idx -= 1;
        }

        printf("score: %d \n", score_sum);

        if (i % 2 == 0 && current_figure_idx < max_figures_number) { // generate new figure
            printf("\nGenerating new figure: \n");
            shape = rand() % 3;
            color_code = rand() % 2;
            time_to_live = rand() % 10;
            if (color_code == 1) {
                color = (uint32_t) 0xFFFF0000; // red
            } else {
                color = (uint32_t) 0xFF00FF00; // green
            }
            printf(" - shape number: %d \n", shape);
            printf(" - color code: %d \n", color_code);

            x = rand() % max_width;
            y = rand() % max_height;

            printf(" - x cord: %d \n", x);
            printf(" - y cord: %d \n", y);

            score_number = 3 - shape;

            if (color_code == 1) {
                score_number *= (-1);
            }
            Figure new_figure = {x, y, shape, score_number, time_to_live, color};
            figures[current_figure_idx] = new_figure;
            current_figure_idx += 1;
        }
    }

    return 0;
}