#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>


int main() {
    int test_case;

    printf("Select Test Case (1-5):\n");
    printf("1: Basic Synchronization Test\n");
    printf("2: Deadlock Avoidance Test\n");
    printf("3: Fairness Test (Starvation Prevention)\n");
    printf("4: Concurrent Eating Test\n");
    printf("5: Efficiency and Performance Test\n");
    scanf("%d", &test_case);

    switch (test_case) {
        case 1:
            printf("Running Basic Synchronization Test...\n");
            run_test_case(1);
            break;
        case 2:
            printf("Running Deadlock Avoidance Test...\n");
            run_test_case(2);
            break;
        case 3:
            printf("Running Fairness Test...\n");
            run_test_case(3);
            break;
        case 4:
            printf("Running Concurrent Eating Test...\n");
            run_test_case(4);
            break;
        case 5:
            printf("Running Efficiency and Performance Test...\n");
            run_test_case(5);
            break;
        default:
            printf("Invalid test case selected.\n");
            break;
    }

    return 0;
}
