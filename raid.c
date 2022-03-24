#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
  
#define CHUNK_SIZE 4
#define NUM_PARITY_BITS 3
#define ENCODING_LENGTH (CHUNK_SIZE + NUM_PARITY_BITS)
#define BUFFER_SIZE 4096
#define NUM_FILES 7
#define FILENAME_BUFFER 64
#define NUM_ENCODINGS

char* get_encoding(int nibble);
int bin_to_dec(char* bin_num);
int expt(int base, int power);

char encodings[NUM_ENCODINGS][ENCODING_LENGTH + 1] = {
  "0000000", // 0
  "1101001", // 1
  "0101010", // 2
  "1000011", // 3
  "1001100", // 4
  "0100101", // 5
  "1100110", // 6
  "0001111", // 7
  "1110000", // 8
  "0011001", // 9
  "1011010", // 10
  "0110011", // 11
  "0111100", // 12
  "1010101", // 13
  "0010110", // 14
  "1111111"  // 15
};

// 

int main(int argc, char* argv[]) {

  // open files to read from
  char* filename = argv[2];
  FILE *file_input_stream = fopen(filename, "rb");

  if (file_input_stream == NULL) {
    printf("File Not Found. Shutting Down.\n");
    exit(1);
  }

  // open seven files to write to
  FILE *files [NUM_FILES];
  printf("Creating files...\n");
  for (int i = 0; i < NUM_FILES; i++) {
    char fname[FILENAME_BUFFER];
    sprintf(fname, "%s._part%d", filename, i);
    if ((files[i] = fopen(fname, "wb")) == NULL) {
      printf("Error Creating File. Shutting Down.\n");
      exit(1);
    }
  }

  char current_byte = 0;
  long lines_processed = 0;
  long bytes_processed = 0;

  // table for accumulating bytes
  char encoding_table[2 * CHUNK_SIZE + 10][ENCODING_LENGTH + 10] = {{0}};

  // array of buffers for writing to the files. file_buffer[i] corresponds to
  // files[i]
  char file_output_buffers[NUM_FILES][BUFFER_SIZE] = {{0}};

  // might be able to speed this up with fgets? then we wouldn't have to worry
  // about "Lines Processed"
  printf("Beginning iteration through %s...\n", argv[2]);
  while ((current_byte = fgetc(file_input_stream)) != EOF) {

    // split byte into two nibbles and put in table
    strcpy(encoding_table[lines_processed], get_encoding(current_byte >> 4));
    lines_processed += 1;

    strcpy(encoding_table[lines_processed], get_encoding(current_byte & 0xf));
    lines_processed += 1;

    // every 8 nibbles processed means we "dump" a column after converting it to
    // a byte
    if (lines_processed == ENCODING_LENGTH + 1) {

      // if the buffers are full, write their contents to each file
      if (bytes_processed > BUFFER_SIZE - 1){
        for (int n = 0; n < NUM_FILES; n++) {
          fwrite(
            file_output_buffers[n], sizeof(char), BUFFER_SIZE, files[n]
          );
        }
        bytes_processed = 0;
      }

      for (int i = 0; i < ENCODING_LENGTH; i++) {

        char* byte_to_write = malloc(9 * sizeof(char));

        // iterate through table column by column
        for (int j = 0; j < (2 * CHUNK_SIZE) + 1; j++) {
          byte_to_write[j] = encoding_table[j][i];
        }  
        // add a byte to each buffer
        file_output_buffers[i][bytes_processed] = (unsigned char)bin_to_dec(byte_to_write);

        free(byte_to_write);
      }

      // this is kind of a lie. the real amount is 7 times this
      bytes_processed += 1;

      // reset for the next processing
      lines_processed = 0;
    }
  }
  printf("Writing remaining bytes...\n");
  for (int i = 0; i < NUM_FILES; i++) {
    fwrite(
      file_output_buffers[i], sizeof(char), bytes_processed, files[i]
    );
  }

  // close files
  printf("Closing files...\n");
  fclose(file_input_stream);
  for (int i = 0; i < NUM_FILES; i++) {
   fclose(files[i]);
  }

  printf("Done!\n");
  return 0;
}

// convert binary string to decimal
int bin_to_dec(char* bin_num) {
  unsigned int res = 0;
  for (int i = ENCODING_LENGTH, offset = 0; i >= 0; i--, offset++) {
    res += (bin_num[offset] - '0') * expt(2, i);
  }
  return res;
}

// get binary string for nibble
char* get_encoding(int nibble) {
  char* encoded_nibble = encodings[nibble];
  return encoded_nibble;
}

// incase math library's pow isn't working on clang
int expt(int base, int power){
  return power == 0 ? 1 : base * expt(base, power - 1);
}

