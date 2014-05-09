#include <stdint.h>
#include <stdlib.h>

int foo(int *bar) {
    int aux = 2;
    bar[2] = aux;
}

int main(){return 1;}



int maxDisplayFigure(uint8_t figure, uint8_t *display, uint8_t start_digit, uint8_t len) {
    // Mod to: figure, current_figure, startpoint, len
    uint8_t i=3, limit = 1;
    uint8_t aux = 123;




    // // check for overflow 
    // if (start_digit + len > 9) {
    //     return 1;
    // }

    // // aux = (uint8_t *) malloc(sizeof(uint32_t) * len);
    // // if (!aux) return 1;

    // // Get max number displayable for a given number length
    // for (i = 0; i < len; i++) {
    //     limit *= 10;
    // }

    // figure = figure % limit;

    // for (i = start_digit - 1; i < len + start_digit - 1; i++) {
    //     aux = figure % 10;
    //     figure /= 10;

        display[i] = aux;
    // }


    // free(aux);
    return 0;
    }

