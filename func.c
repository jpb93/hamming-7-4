#include <stdio.h>
#include <stdlib.h>

#define NUM_ENCODINGS 16
#define CORRUPT_MSG -1

int get_bit(int num, int pos);
int byte_from_nibbles(int high, int low);
int get_parity(int num, int b1, int b2, int b3, int b4);
int find_error_pos(int num);
int flip_bit(int num, int pos);
int correct_error(int num);
int decode(int num);

char encodings[NUM_ENCODINGS] = { 
  0x0, 
  0x69, 
  0x2a, 
  0x43,
  0x4c, 
  0x25, 
  0x66, 
  0xf,
  0x70, 
  0x19, 
  0x5a, 
  0x33,
  0x3c, 
  0x55, 
  0x16, 
  0x7f
};

int main() {

  int test;
  do {
    printf("Please enter a hex value between 0 and FF: ");
    scanf("%x", &test);
    if (test == -1) break;
    int decoded = decode(test);
    if (decoded != CORRUPT_MSG) {
      printf("The message is not corrupted.\nYou transmitted the code for %x.\n\n", decoded);
    } else {
      printf("The message was corrupted!\nWhen corrected, this is the code for %x.\n\n",
        decode(correct_error(test))
      );
    }
  } while (1);

  printf("Done.\n");

  return 0;
}

// e.g. 1 0 0 1 0 1 0 1 = 0x95
// pos  7 6 5 4 3 2 1 0
int get_bit(int num, int pos) {
  return (num >> pos) & 1;
}

int byte_from_nibbles(int high, int low) {
  return (high << 4) | low;
}

int get_parity(int num, int b1, int b2, int b3, int b4) {
  return get_bit(num, b1) ^ 
         get_bit(num, b2) ^ 
         get_bit(num, b3) ^ 
         get_bit(num, b4);
}

int find_error_pos(int num) {
  int c1 = get_parity(num, 6, 4, 2, 0);
  int c2 = get_parity(num, 5, 4, 1, 0);
  int c4 = get_parity(num, 3, 2, 1, 0);
  return c1 | (c2 << 1) | (c4 << 2);
}
// pos  (0) 1 2 3 4 5 6 7
// e.g. (0) 1 1 0 0 1 1 0
int flip_bit(int num, int pos) {
  return num ^ (0x80 >> pos);
}

int correct_error(int num) {
  return flip_bit(num, find_error_pos(num));
}

int decode(int num) {
  for (int i = 0; i < NUM_ENCODINGS; i++) {
    if (num == encodings[i])
      return i;
  }

  return CORRUPT_MSG;
}


 // int number;
  // int position;

  // printf("Enter a hex value between 0 and FF: ");
  // scanf("%x", &number);

  // printf("Enter a position to check: ");;
  // scanf("%d", &position);

  // printf("Value at position %d is %d\n", position, get_bit(number, position));
  // int high_nibble;
  // int low_nibble;

  // printf("Enter a nibble between 0 and F: ");
  // scanf("%x", &high_nibble);

  // printf("Enter another nibble between 0 and F: ");
  // scanf("%x", &low_nibble);

  // printf("When put together, these nibbles make the hex value %x\n", byte_from_nibbles(high_nibble, low_nibble));