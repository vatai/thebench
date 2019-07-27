#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#ifdef DO_MPI
#include <mpi.h>
#endif

void get_cft(int *cfrom, int *cto, int argc, char *argv[]) {
  if (argc == 2) {
    sscanf(argv[1], "%d", cto);
    printf("Set cto: %d\n", *cto);
  }
  if (argc == 3) {
    sscanf(argv[1], "%d", cfrom);
    sscanf(argv[2], "%d", cto);
    printf("Set cfrom: %d cto: %d\n", *cfrom, *cto);
  }
}

double *my_malloc(size_t s) {
  double *buf;
#ifdef DO_MPI
  MPI_Info info;
  int ierr;
  MPI_Errhandler_set(MPI_COMM_WORLD, MPI_ERRORS_RETURN);
  MPI_Info_create(&info);
  ierr = MPI_Alloc_mem(s, info, &buf);
  if (ierr != MPI_SUCCESS)
    buf = NULL;
#else
  buf = malloc(s);
#endif
  return buf;
}

void my_free(double *buf) {
#ifdef DO_MPI
  MPI_Free_mem(buf);
#else
  free(buf);
#endif
}

int main(int argc, char *argv[]) {
  printf("Hello!\n");
#ifdef DO_MPI
  MPI_Init(&argc, &argv);
  printf("MPI VERSION\n");
#endif
  double *buf = NULL;
  size_t n;
  int nbits = sizeof(n) * CHAR_BIT;
  double tmp = 0;

  int cfrom = 0;
  int cto = 4;
  get_cft(&cfrom, &cto, argc, argv);

  printf("double *buf;\n");
  printf("size_t n; // sizeof(n)=%lu (= %lu bits)\n", sizeof(n), nbits);

  for (int c = cfrom; c < cto; c++) {
    printf("\n=== CASE %d ===\n\n", c);

    for (int i = 0; i < nbits; i++) {
      n = 1ul << i;

      switch (c) {

      case 0: // Allocate
        printf("buf = malloc(0x%lx * sizeof(*buf)); // i == %d", n, i);
        buf = my_malloc(n * sizeof(*buf));

        break;

      case 1: // Allocate and Read
        buf = my_malloc(n * sizeof(*buf));
        if (n * sizeof(*buf) < n) {
          printf("For i = %d we have size_t overflow! (skipping)\n", i);
          break;
        }
        if (buf == NULL) break;
        printf("Reading (i = %d): ", i);
        tmp = 0;
        for (int j = 0; j < n; j++)
          tmp += buf[j];

        break;

      case 2: // Allocate and Write
        buf = my_malloc(n * sizeof(*buf));
        if (n * sizeof(*buf) < n) {
          printf("For i = %d we have size_t overflow! (skipping)\n", i);
          break;
        }
        if (buf == NULL) break;
        printf("Writing (i = %d): \n", i);
        for (int j = 0; j < n; j++)
          buf[j] = j % 7;
        printf("\n");

        break;

      }

      // Free
      if (buf != NULL) {
        printf("++ OK! (Doing free(buf);)\n");
        my_free(buf);
      } else {
        printf("-- Failed! (But still alive!)\n");
      }
    }
  }

#ifdef DO_MPI
  printf("MPI VERSION\n");
  MPI_Finalize();
#endif
  printf("Bye!\n");
}
