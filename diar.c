#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

#define CHUNK_SIZE 4
#define NUM_PARITY_BITS 3
#define ENCODING_LENGTH (CHUNK_SIZE + NUM_PARITY_BITS)
#define BUFFER_SIZE 4096
#define NUM_FILES 7
#define FILENAME_BUFFER 64
#define NUM_ENCODINGS 16
#define CORRUPT_MSG -1
#define MASK 128
#define NUM_ENCODED_MSG 4

char get_bit(char num, int pos);
char byte_from_nibbles(char high, char low);
char get_parity(char num, int b1, int b2, int b3, int b4);
char find_error_pos(char num);
char flip_bit(char num, int pos);
char correct_error(char num);
char decode(char num);
char decode_nibble(char bytes[]);
void shift_bytes(char bytes[]);

char encodings[NUM_ENCODINGS] = { 
  0x0, 0x69, 0x2a, 0x43,
  0x4c, 0x25, 0x66, 0xf,
  0x70, 0x19, 0x5a, 0x33,
  0x3c, 0x55, 0x16, 0x7f
};

int main(int argc, char* argv[]) {

  FILE *decoded_file;
  int decoded_size = 0;
  char* filename;
  // get options
  int option;
  while ((option = getopt(argc, argv, "f:s:")) != -1) {
    switch(option) {
      case 'f':
        filename = optarg;
        char restored_filename[FILENAME_BUFFER];
        sprintf(restored_filename, "%s.2", filename);
        decoded_file = fopen(restored_filename, "wb");
        break;
      case 's':
        decoded_size = atoi(optarg);
        break;
      case '?':
        exit(1);
        break;
      default:
        abort();
    }
  }

  // open seven files to read from
  FILE *files [NUM_FILES];

  for (int i = 0; i < NUM_FILES; i++) {
    char fname[FILENAME_BUFFER];
    sprintf(fname, "%s.part%d", filename, i);
    if ((files[i] = fopen(fname, "rb")) == NULL) {
      printf("Error Reading File. Shutting Down.\n");
      exit(1);
    }
  }

  int current_byte = 0;
  int bytes_processed = 0;
  int current_encoding = 0;

  char encoded_bytes[NUM_FILES];
  char decoded_buffer[BUFFER_SIZE];

  // argv[4] gives us the size from '... -s 5694080', representing total of 
  // bytes for the restored file
  while (bytes_processed < decoded_size) {
    
    // check if buffer is full, and write it to main file if it is
    if (current_byte > BUFFER_SIZE - 1) {
      fwrite(decoded_buffer, sizeof(char), BUFFER_SIZE, decoded_file);
      current_byte = 0;
    }

    // the byte in shakespeare.parti is placed into encoded_bytes[i]
    for (int i = 0; i < NUM_FILES; i++) 
      encoded_bytes[i] = fgetc(files[i]);
    
    while (current_encoding < NUM_ENCODED_MSG){
      char high_nibble = decode_nibble(encoded_bytes);
      shift_bytes(encoded_bytes);

      char low_nibble = decode_nibble(encoded_bytes);
      shift_bytes(encoded_bytes);

      char original_byte = byte_from_nibbles(high_nibble, low_nibble);
      decoded_buffer[current_byte++] = original_byte;
      current_encoding++;
    }

    // every iteration we restore 8 nibbles = 4 bytes
    bytes_processed += 4;
    current_encoding = 0;
  }

  // write whatever is remaining in the buffer
  if (current_byte >= 0) 
    fwrite(decoded_buffer, sizeof(char), current_byte, decoded_file);  
    
  // close files
  fclose(decoded_file);
  for (int i = 0; i < NUM_FILES; i++) 
    fclose(files[i]);
  
  return 0;
}

// e.g. 1 | 0 0 1 0 1 0 1 = 0x95
// pos  7 | 6 5 4 3 2 1 0
char get_bit(char num, int pos) {
  return (num >> pos) & 1;
}

char byte_from_nibbles(char high, char low) {
  return (high << 4) | low;
}

char get_parity(char num, int b1, int b2, int b3, int b4) {
  return get_bit(num, b1) ^ 
         get_bit(num, b2) ^ 
         get_bit(num, b3) ^ 
         get_bit(num, b4);
}

char find_error_pos(char num) {
  return get_parity(num, 6, 4, 2, 0)  
      | (get_parity(num, 5, 4, 1, 0) << 1)
      | (get_parity(num, 3, 2, 1, 0) << 2);
}
// pos  (0) | 1 2 3 4 5 6 7
// e.g. (0) | 1 1 0 0 1 1 0
char flip_bit(char num, int pos) {
  return num ^ (0x80 >> pos);
}

char correct_error(char num) {
  return flip_bit(num, find_error_pos(num));
}

char decode(char num) {
  for (int i = 0; i < NUM_ENCODINGS; i++) 
    if (num == encodings[i])
      return i;

  return decode(correct_error(num));
}

char decode_nibble(char bytes[]) {
  char nibble = 0x00;
  for (int i = 0; i < ENCODING_LENGTH; i++) 
    nibble |= (bytes[i] >> i) & (MASK >> i);
  
  nibble >>= 1;

  return decode(nibble);
}

void shift_bytes(char bytes[]) {
  for (int i = 0; i < NUM_FILES; i++) 
    bytes[i] <<= 1;
}