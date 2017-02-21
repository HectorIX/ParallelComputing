// TotientRance.c - Sequential Euler Totient Function (C Version)
// compile: gcc -Wall -O -o TotientRange TotientRange.c
// run:     ./TotientRange lower_num uppper_num

// Greg Michaelson 14/10/2003
// Patrick Maier   29/01/2010 [enforced ANSI C compliance]

// This program calculates the sum of the totients between a lower and an
// upper limit using C longs. It is based on earlier work by:
// Phil Trinder, Nathan Charles, Hans-Wolfgang Loidl and Colin Runciman

#include <stdio.h>
#include <mpi.h>
#include <time.h>


long hcf(long x, long y)
{
  long t;

  while (y != 0) {
    t = x % y;
    x = y;
    y = t;
  }
  return x;
}



int relprime(long x, long y)
{
  return hcf(x, y) == 1;
}


// euler n = length (filter (relprime n) [1 .. n-1])

long euler( long pieceOfWork, int extra )
{

  long subSum = 0;
  long index = 0;
  long length, i;

  long work[pieceOfWork+extra+1];


    for(i=0; i<=pieceOfWork+extra; i++)
      work[i] = 0;

    MPI_Status status;
    MPI_Recv(&work[0], pieceOfWork+extra+1, MPI_LONG, 0, 0, MPI_COMM_WORLD, &status);


    for( index=0; index<=(pieceOfWork+extra); index++ )
    {

      length = 0;
      for( i=0; i< *(work+index); i++)
      {
          if( relprime( *(work+index), i+1 ) )
            length++;
      }

      subSum+=length;
    }


  MPI_Send(&subSum, 1 , MPI_LONG, 0 , 0 , MPI_COMM_WORLD);

}


// sumTotient lower upper = sum (map euler [lower, lower+1 .. upper])

long sumTotient(long lower, long upper, int processes, long pieceOfWork, int extra )
{
  int proc_num = 0;
  long i = 0;
  long j = 0;
  long sum = 0;
  long result = 0;

  long arrayOfWork[pieceOfWork+extra+1];

  for(i=0; i<=pieceOfWork+extra; i++)
    arrayOfWork[i] = 0;


  MPI_Status status;


  for( proc_num=1; proc_num<processes; proc_num++ )
  {

    j=0;

    if (proc_num == 1)
    {

      for( i=lower; i<(lower+pieceOfWork); i++ )
      {
        arrayOfWork[j] = i;
        j++;

      }

      if (extra!=0) {

        int temp = extra;

        for (i=upper; temp!=0; i--) {
          arrayOfWork[j] = i;
          temp--;
          j++;
        }

      }
      MPI_Send(&arrayOfWork[0], pieceOfWork+extra+1, MPI_LONG, proc_num/*rank*/, 0 , MPI_COMM_WORLD);

    }
    else {

      for( i=lower+((proc_num-1)*pieceOfWork); i<(lower+(proc_num*pieceOfWork)); i++ )
      {
        arrayOfWork[j] = i;
        j++;
      }

      MPI_Send(&arrayOfWork[0], pieceOfWork, MPI_LONG, proc_num/*rank*/, 0 , MPI_COMM_WORLD);
    }


    MPI_Recv(&result, 1, MPI_LONG, proc_num/*rank*/ , 0 , MPI_COMM_WORLD, &status);


    sum = sum + result;
  }

 return sum;

}


void validateResult( int upper, long result ) {

  switch ( upper ) {

    case 10:

      if( result == 32 ) {

        printf("\t + WolframAlpha approves this result!\n");
        break;

      } else {

        printf("\t + Wrong result: %ld instead of 32.\n\n", result);
        break;
      }

    case 1000:

      if( result == 304192 ) {

        printf("\t + WolframAlpha approves this result!\n\n");
        break;

      } else {

        printf("\t + Wrong result: %ld instead of 304192.\n\n", result);
        break;
      }

    case 15000:

      if( result == 68394316 ) {

        printf("\t + WolframAlpha approves this result!\n\n");
        break;

      } else {

        printf("\t + Wrong result: %ld instead of 68394316.\n\n", result);
        break;
      }

    case 30000:

      if( result == 273571774 ) {

        printf("\t + WolframAlpha approves this result!\n\n");
        break;

      } else {

        printf("\t + Wrong result: %ld instead of 273571774.\n\n", result);
        break;
      }

    case 100000:

      if( result == 3039650754 ) {

        printf("\t + WolframAlpha approves this result!\n\n");
        break;

      } else {

        printf("\t + Wrong result: %ld instead of 3039650754.\n\n", result);
        break;
      }

    default:
      break;
  }

  return;
}

int main(int argc, char ** argv)
{

  long lower, upper;
  long sumTotients = 0;
  long pieceOfWork = 0;

  int processes, rank;


  if (argc != 3) {
    printf("not 2 arguments\n");
    return 1;
  }

  sscanf(argv[1], "%ld", &lower);
  sscanf(argv[2], "%ld", &upper);


  MPI_Init(&argc , &argv );

  MPI_Comm_size (MPI_COMM_WORLD, &processes);
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);

  int extra = upper % (processes-1);
  pieceOfWork = (upper) / (processes-1);



  if (rank == 0)
  {
    printf("pieceOfWork = %ld\n", pieceOfWork);
    printf("extra = %d\n", extra );

    sumTotients = sumTotient (lower, upper, processes, pieceOfWork, extra) ;

    printf("\n\t + Sum of Totients  between [%ld..%ld] is %ld\n\n", lower, upper, sumTotients);

    validateResult(upper,sumTotients);
  }
  else if ( rank == 1 ) {

      euler(pieceOfWork, extra);
  }
  else {

      euler(pieceOfWork, 0);
  }

  MPI_Finalize();

  return 0;
}
