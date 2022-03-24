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
#define NUM_ENCODINGS 16

int main(int argc, char* argv[]) {
  char* filename = argv[2];
  // create and open the file for which we will decode
  char restored_filename[FILENAME_BUFFER];
  sprintf(restored_filename, "%s.2", filename);
  FILE *decoded_file = fopen(restored_filename, "wb");

  // open seven files to read from
  FILE *files [NUM_FILES];
  printf("Opening files...\n");
  for (int i = 0; i < NUM_FILES; i++) {
    char fname[FILENAME_BUFFER];
    sprintf(fname, "%s._part%d", filename, i);
    if ((files[i] = fopen(fname, "rb")) == NULL) {
      printf("Error Reading File. Shutting Down.\n");
      exit(1);
    }
  }

  char current_byte = 0;
  char lines_processed = 0;
  char bytes_processed = 0;

  //do {
    char file_bytes[NUM_FILES];
    for (int i = 0; i < NUM_FILES; i++){
      file_bytes[i] = fgetc(files[i]);
    }
 // }

  fwrite(file_bytes, sizeof(char), NUM_FILES, decoded_file);
  return 0;

}



// void charToBin(char c) {
//   for (int i = CHAR_BIT; i > 0; i--) {
//     printf("%d", (unsigned int) ((c & (1 << i)) ? 1 : 0));
//   }
// }