#include <math.h>
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]);

int main(int argc, char *argv[])
{
  int n = 10;
  int a[n];
  int a_row;
  int ans;
  int b[n];
  int dest;
  int dummy;
  int ierr;
  int master = 0;
  int my_id;
  int num_procs;
  int num_rows;
  int num_workers;
  MPI_Status status;
  int tag;
  int tag_done;
  int x;

  ierr = MPI_Init(&argc, &argv);

  if (ierr != 0)
  {
    printf("\n");
    printf("MATVEC_MPI - Fatal error!\n");
    printf("  MPI_Init returns nonzero IERR.\n");
    exit(1);
  }

  ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

  ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

  if (my_id == 0)
  {
    printf("\n");
    printf("VECESC - Proceso master:\n");
    printf("  version C\n");
    printf("  un producto de escalar - vector b = A * x.\n");
    printf("\n");
    printf("  Compilado el %s a las %s.\n", __DATE__, __TIME__);
    printf("\n");
    printf("  Los numeros de procesadores son %d.\n", num_procs);
  }
  printf("\n");
  printf("El procesador %d esta activo.\n", my_id);

  tag_done = n + 1;

  if (my_id == 0)
  {
    printf("\n");
    printf("  The number of columns is %d.\n", n);
  }

  if (my_id == master)
  {

    for (int i = 0; i <= n; i++)
    {
      a[i] = rand() % 100;
    }

    x = rand() % 100;

    printf("\n");
    printf("MATVEC - Master process:\n");
    printf("  Vector:\n");
    printf("\n");
    for (int i = 0; i < n; i++)
    {
      printf("%d %d\n", i, a[i]);
    }
    printf("  Escalar:\n");
    printf("\n");
    printf("%d\n", x);
  }

  ierr = MPI_Bcast(&x, n, MPI_DOUBLE, master, MPI_COMM_WORLD);

  if (my_id == master)
  {
    num_rows = 0;

    for (int i = 1; i <= num_procs - 1; i++)
    {
      dest = i;
      tag = num_rows;

      ierr = MPI_Send(&a[num_rows], 1, MPI_DOUBLE, dest, tag, MPI_COMM_WORLD);

      num_rows = num_rows + 1;
    }

    num_workers = num_procs - 1;

    for (;;)
    {
      ierr = MPI_Recv(&ans, 1, MPI_DOUBLE, MPI_ANY_SOURCE,
                      MPI_ANY_TAG, MPI_COMM_WORLD, &status);

      tag = status.MPI_TAG;
      b[tag] = ans;

      if (num_rows < n)
      {
        dest = status.MPI_SOURCE;
        tag = num_rows;

        ierr = MPI_Send(&a[num_rows], 1, MPI_DOUBLE, dest, tag, MPI_COMM_WORLD);
        num_rows = num_rows + 1;
      }
      else
      {
        num_workers = num_workers - 1;
        dummy = 0;
        dest = status.MPI_SOURCE;
        tag = tag_done;

        ierr = MPI_Send(&dummy, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);

        if (num_workers == 0)
        {
          break;
        }
      }
    }
  }
  else
  {
    ans = 0.0;
    for (;;)
    {
      ierr = MPI_Recv(&a_row, 1, MPI_DOUBLE, master, MPI_ANY_TAG,
                      MPI_COMM_WORLD, &status);

      tag = status.MPI_TAG;

      if (tag == tag_done)
      {
        printf("  Process %d shutting down.\n", my_id);
        break;
      }

      ans = a_row * x;

      ierr = MPI_Send(&ans, 1, MPI_DOUBLE, master, tag, MPI_COMM_WORLD);
    }
  }
  if (my_id == master)
  {
    printf("\n");
    printf("MATVEC - Master process:\n");
    printf("  Product vector b = A * x\n");
    printf("\n");
    for (int i = 0; i < n; i++)
    {
      printf("%d %d\n", i, b[i]);
    }
  }
  ierr = MPI_Finalize();

  if (my_id == master)
  {
    printf("\n");
    printf("MATVEC - Master process:\n");
    printf("  Normal end of execution.\n");
    printf("\n");
  }
  return 0;
}