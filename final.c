// TotientRance.c - Sequential Euler Totient Function (C Version)
// compile: gcc -Wall -O -o TotientRange TotientRange.c
// run:     ./TotientRange lower_num uppper_num

// Greg Michaelson 14/10/2003
// Patrick Maier   29/01/2010 [enforced ANSI C compliance]

// This program calculates the sum of the totients between a lower and an
// upper limit using C longs. It is based on earlier work by:
// Phil Trinder, Nathan Charles, Hans-Wolfgang Loidl and Colin Runciman



// Author of the modified code: Georgios Chiotis


#include <stdio.h>
#include <mpi.h>
#include <time.h>

/**********************************************************************************************
 *
 * hcf computes the gcd of the two numbers.
 *
 **********************************************************************************************/
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

/**********************************************************************************************
 *
 * relprime check if the two given numbers are relative primes.
 *
 **********************************************************************************************/
int relprime(long x, long y)
{
  return hcf(x, y) == 1;
}


/**********************************************************************************************
 *
 *  euler receives a load of work from the rank #0, makes the actual computation
 *  and return the result to the sumTotient funtion.
 *
 *  @param: pieceOfWork is the work load of the current process.
 *  @param: extra is the potential extra work of load that rank #1 need to compute.
 *
 *  return the sub-sum of the current rank.
 *
 **********************************************************************************************/
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
      for( i=0; i<work[index]; i++)
      {
          if( relprime( work[index], i+1 ) )
            length++;
      }

      subSum+=length;
    }


  MPI_Send(&subSum, 1 , MPI_LONG, 0 , 0 , MPI_COMM_WORLD);

}


/**********************************************************************************************
 *
 *  sumTotient computes the load of work of each process and sent it by calling the MPI_Send
 *  function. Then it receives the results from each process, add this to a long variable
 *  and finaly return it to the main fucntion, where it print out.
 *
 *  @param: lower is the lower limit of the calculation of the sum.
 *  @param: upper is the upper limit of the calculation of the sum.
 *  @param: processes is the number of the processes.
 *  @param: pieceOfWork is the work load of each process.
 *  @param: extra is the potential extra work load, that is added to the first rank only.
 *
 * return the result of the whole computation.
 *
 **********************************************************************************************/
long sumTotient(long lower, long upper, int processes, long pieceOfWork, int extra )
{
  int proc_num = 0;
  long i = 0;
  long j = 0;
  long sum = 0;
  long result = 0;

  long arrayOfWork[pieceOfWork+extra+1];

  for(i=0; i<=pieceOfWork+extra; i++)   // Initializes the array.
    arrayOfWork[i] = 0;


  MPI_Status status;

  // for each rank Initialize the array of its work load
  // and send it to the process (fucntion euler).
  // Example: for 3 processes and for the sum from 1 to 10
  // rank_1: computes 1,2,3,10
  // rank_2: computes 4,5,6
  // rank_3: computes 7,8,9
  for( proc_num=1; proc_num<processes; proc_num++ )
  {

    j=0;

    if (proc_num == 1)
    {

      for( i=lower; i<upper; i+=(processes-1) )
      {
        arrayOfWork[j] = i;
        //printf("[%ld], ", arrayOfWork[j] );
        j++;

      }

      if (extra!=0) {

        int temp = extra;

        for (i=upper; temp!=0; i--) {
          arrayOfWork[j] = i;
          //printf("[%ld], ", arrayOfWork[j] );
          temp--;
          j++;
        }

      }
      MPI_Send(&arrayOfWork[0], pieceOfWork+extra+1, MPI_LONG, proc_num/*rank*/, 0 , MPI_COMM_WORLD);

    }
    else {

      for( i=proc_num; i<=upper; i+=(processes-1) )
      {
        arrayOfWork[j] = i;
        //printf("[%ld], ", arrayOfWork[j] );
        j++;
      }

      MPI_Send(&arrayOfWork[0], pieceOfWork, MPI_LONG, proc_num/*rank*/, 0 , MPI_COMM_WORLD);
    }


    MPI_Recv(&result, 1, MPI_LONG, proc_num/*rank*/ , 0 , MPI_COMM_WORLD, &status);


    sum = sum + result;
  }

 return sum;

}



/******************************************************************************************
 *
 *  validateResult compare the result to the ones given by WolframAlpha.
 *  The comparison take place only for values 10, 1000, 15000, 30000 and 100000.
 *
 *  @param: upper is the upper limit of the calculation of the sum.
 *  @param: result is the actual result of the computation.
 *
 ******************************************************************************************/
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



/******************************************************************************************
 *
 *  main is the main function of the program that Initializes the MPI, computes the
 *  size of the workload of each process and call them with the appropriate arguments.
 *  Also, it measures the time needed for the whole computation and validates the result.
 *
 *  @param: 2 arguments are expected. The lower and the upper limit of the sum of totients.
 *
 ******************************************************************************************/
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

  int extra = upper % (processes-1);      // computes the extra work load
                                          // in case that the work is not divided
                                          // exact from the number of processes.
  pieceOfWork = (upper) / (processes-1);  // The work load of each process.



  if (rank == 0)    // rank #0 call the sumTotient function that computes the sum
  {                 // of the totients. It also cal the time needed for the whole
                    // computation and finaly validates the result.

    clock_t start, end;
    printf("=============================================\n");
    printf("Num of processes = %d\n", processes );
    printf("Load Of Work for each process = %ld\n", pieceOfWork);
    printf("Extra work for the process #1 = %d\n", extra );

    start = clock();
    sumTotients = sumTotient (lower, upper, processes, pieceOfWork, extra) ;
    end = clock();
    printf("\n\t + Sum of Totients  between [%ld..%ld] is %ld\n\n", lower, upper, sumTotients);

    validateResult(upper,sumTotients);

    printf( "\tNumber of seconds: %f\n", (end-start)/(double)CLOCKS_PER_SEC );
    printf("=============================================\n");
  }
  else if ( rank == 1 ) {   // rank #1 computes its own work load plus the extra work.

      euler(pieceOfWork, extra);
  }
  else {    // All the other ranks compute just their own parts of work load.

      euler(pieceOfWork, 0);
  }

  MPI_Finalize();

  return 0;
}
