#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
  
#define NIBBLE_SIZE 4
#define NUM_PARITY_BITS 3
#define ENCODING_LENGTH (NIBBLE_SIZE + NUM_PARITY_BITS)
#define BUFFER_SIZE 4096
#define NUM_FILES 7
#define FILENAME_BUFFER 64
#define NUM_ENCODINGS 16
#define TABLE_ROWS 8
#define MASK 128

char encodings[NUM_ENCODINGS] = { 
  0x0, 0x69, 0x2a, 0x43,
  0x4c, 0x25, 0x66, 0xf,
  0x70, 0x19, 0x5a, 0x33,
  0x3c, 0x55, 0x16, 0x7f
};
  
int main(int argc, char* argv[]) {

   char filename[FILENAME_BUFFER];
   FILE *file_input_stream;
   // get options
   int option;
   while ((option = getopt(argc, argv, "f:")) != -1) {
     switch(option) {
       case 'f':
         file_input_stream = fopen(optarg, "rb");
         strcpy(filename, optarg);
         break;
       case '?':
         exit(1);
         break;
       default:
         abort();
     }
   }

  if (file_input_stream == NULL) {
    printf("File Not Found. Shutting Down.\n");
    exit(1);
  }

  // open seven files to write to
  FILE *files [NUM_FILES];
  for (int i = 0; i < NUM_FILES; i++) {
    char* part = ".part";

    // prevent concerns of not enough space
    char fname[FILENAME_BUFFER + sizeof(part)];
    sprintf(fname, "%s%s%d", filename, part, i);
    
    if ((files[i] = fopen(fname, "wb")) == NULL) {
      printf("Error Creating File. Shutting Down.\n");
      exit(1);
    }
  }

  int bytes_processed = 0;
  int nibbles_processed = 0;
  char current_byte = 0;

  char file_output_buffers[NUM_FILES][BUFFER_SIZE] = {{0}};
  char bytes_array[TABLE_ROWS + 1];

  while ((current_byte = fgetc(file_input_stream)) != EOF) {

    if (bytes_processed > BUFFER_SIZE - 1){
      for (int n = 0; n < NUM_FILES; n++) {
        fwrite(
          file_output_buffers[n], sizeof(char), BUFFER_SIZE, files[n]
        );
      }
      bytes_processed = 0;
    }

    bytes_array[nibbles_processed++] = encodings[current_byte >> 4];
    bytes_array[nibbles_processed++] = encodings[current_byte & 0xf];
  
    if (nibbles_processed == ENCODING_LENGTH + 1) {

      int left_adjustment = 1;
      int current_file = 0;

      while (current_file < NUM_FILES) {
      
        //left shit all values by 1 bit
        for (int i = 0; i < TABLE_ROWS; i++) 
          bytes_array[i] <<= left_adjustment;

        char byte_to_write = 0x00;

        for (int i = 0; i < TABLE_ROWS; i++) 
          byte_to_write |= (bytes_array[i] >> i) & (MASK >> i);

        file_output_buffers[current_file][bytes_processed] = byte_to_write;

        current_file += 1;
      }
      
      bytes_processed += 1;
      nibbles_processed = 0;
    }
  }

  for (int i = 0; i < NUM_FILES; i++) {
    fwrite(
      file_output_buffers[i], sizeof(char), bytes_processed, files[i]
    );
  }

  fclose(file_input_stream);
  for (int i = 0; i < NUM_FILES; i++) {
    fclose(files[i]);
  }

  return 0;
}
