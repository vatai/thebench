#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#ifdef DO_MPI
#include <mpi.h>
#endif

int main(int argc, char *argv[]) {
  printf("Hello!\n");
#ifdef DO_MPI
  MPI_Init(&argc, &argv);
  printf("MPI VERSION\n");
#endif
  double *buf;
  size_t n;
  int nbits = sizeof(n) * CHAR_BIT;
  printf("double *buf;\n");
  printf("size_t n; // sizeof(n)=%lu (= %lu bits)\n", sizeof(n), nbits);

  for (int c = 0; c < 2; c++) {
    printf("\n=== CASE %d ===\n\n", c);
    for (int i = 0; i < nbits; i++) {
      n = 1ul << i;
      switch (c) {

      case 0: // Allocate

        printf("buf = malloc(0x%lx * sizeof(*buf)); // i == %d\n", n, i);
        buf = malloc(n * sizeof(*buf));
        break;

      case 1: // Allocate and Read

        // printf("buf = malloc(0x%lx * sizeof(*buf)); // i == %d\n", n, i);
        buf = malloc(n * sizeof(*buf));
        if (n * sizeof(*buf) < n) {
          printf("For i = %d we have size_t overflow! (skipping)\n", i);
          break;
        }
        if (buf == NULL) break;
        printf("Reading (i = %d): \n", i);
        double tmp = 0;
        for (int j = 1; j <= i; j++) {
          n = 1ul << j;
          tmp += buf[n - 1];
          if (i == 32) {
            printf("reading from 0x%lx\n", n - 1);
          }
          printf("*");
        }
        printf("\n");
        break;
      }

      // Free
      if (buf != NULL) {
        printf("++ OK! (Doing free(buf);)\n");
        free(buf);
      } else {
        printf("-- Failed! (But still alive!)\n");
      }
    }
  }

  // Allocate and read
  for (int i = 0; i < nbits; i++) {

  }
#ifdef DO_MPI
  printf("MPI VERSION\n");
  MPI_Finalize();
#endif
  printf("Bye!\n");
}
