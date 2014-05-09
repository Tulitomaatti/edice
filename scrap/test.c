#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

static uint8_t leading_zero_suppression = 1;


void printNumbers(uint8_t *numbers, uint8_t len) {
    uint8_t i;
    for (i=0; i<8;i++) {
        printf("%i ", numbers[i]);
    }

    printf("\n");

}

int maxDisplayFigure(uint32_t figure, uint8_t *display, uint8_t start_digit, uint8_t len) {
    // Mod to: figure, current_figure, startpoint, len

    printf("Figure: %i ", figure);

    printf("Display: ");
    printNumbers(display, 8);



    uint8_t i;
    uint32_t limit;

    // sanity checks and trimming
    if (start_digit + len > 9) {
        return 1;
    }

    start_digit -= 1;



    limit = 1;
    for (i = 0; i < len; i++) {
        limit *= 10;
    }


    printf("Limit: %i ", limit);

    figure = figure % limit;
    printf("Figure: %i \n", figure);



    // Insert the figure to the display var and update display

    for (i = start_digit + len; i > start_digit; i--) {
        printf("start_digit: %i len: %i i: %i figure: %i\n", start_digit, len, i, figure);
        if (!figure) {

            if (leading_zero_suppression) {
                display[i - 1] = 10; // 10 is blank index
            } else {
                display[i - 1] = figure;
            }

            
        } else {
            display[i - 1] = figure % 10;
            figure /= 10;                                                                         
        }
        // TODO: Leading zero supression
    }

    printf("Figure: %i ", figure);
    printf("Display: ");
    printNumbers(display, 8);



    return 0;
}

int main() {
    uint8_t display[8] = {1,1,1,1,1,1,1,1};

    printf("Befroe func: ");
    printNumbers(display, 8);
    printf("\n");

    maxDisplayFigure(3, display, 1, 2);
        printf("\n");
        printf("after function : ");

     printNumbers(display, 8);
         printf("\n");
    return 1;
}

