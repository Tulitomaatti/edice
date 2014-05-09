#define _BV(x) (1 << x)
#define bitmask (_BV(3) | _BV 4)

int main() {
    int i = 0;
    i += bitmask;
}