//Caleb Woollends 23002355


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define _CRT_SECURE_NO_WARNINGS
#include "engine.h"
#define  BUFSIZE 512

void testprint(mxArray* test) {

    double* toprint = mxGetPr(test);

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%f ", toprint[i + j * 3]);
        }
        printf("\n");
    }


}


int main(void) {


    /* Variables */
    Engine* ep = NULL; // A pointer to a MATLAB engine object
    mxArray* testOne = NULL, * result = NULL; // mxArray is the fundamental type underlying MATLAB data
    mxArray* testTwo = NULL, * testArrayEigen = NULL;
    double matrixOne[3][3] = { { 1.0, 2.0, 3.0 }, {4.0, 5.0, 6.0 }, {7.0, 8.0, 9.0 } }; // Our test 'matrix', a 2-D array
    double matrixTwo[3][3] = { { 2.0, 2.0, 2.0 }, {3.0, 3.0, 3.0 }, {4.0, 4.0, 4.0 } };
    char buffer[BUFSIZE + 1];

    /* Starts a MATLAB process */
    if (!(ep = engOpen(NULL))) {
        fprintf(stderr, "\nCan't start MATLAB engine\n");
        system("pause");
        return 1;
    }

    testOne = mxCreateDoubleMatrix(3, 3, mxREAL);
    testTwo = mxCreateDoubleMatrix(3, 3, mxREAL);

    memcpy((void*)mxGetPr(testOne), (void*)matrixOne, 9 * sizeof(double));
    memcpy((void*)mxGetPr(testTwo), (void*)matrixTwo, 9 * sizeof(double));

    if (engPutVariable(ep, "testOne", testOne)) {
        fprintf(stderr, "\nCannot write test array to MATLAB \n");
        system("pause");
        exit(1); // Same as return 1;
    }

    if (engPutVariable(ep, "testTwo", testTwo)) {
        fprintf(stderr, "\nCannot write test array to MATLAB \n");
        system("pause");
        exit(1); // Same as return 1;
    }

    if (engEvalString(ep, "testArrayMatrix = testOne * testTwo")) {
        fprintf(stderr, "\nError calculating Matrix  \n");
        system("pause");
        exit(1);
    }

    if (engEvalString(ep, "testArrayEigen = eig(testOne)")) {
        fprintf(stderr, "\nError calculating eigenvalues  \n");
        system("pause");
        exit(1);
    }

    if ((testArrayEigen = engGetVariable(ep, "testArrayEigen")) == NULL) {
        fprintf(stderr, "\nFailed to retrieve testArrayEigen\n");
        system("pause");
        exit(1);
    }


    if ((result = engGetVariable(ep, "testArrayMatrix")) == NULL) {
        fprintf(stderr, "\nFailed to retrieve testArrayMatrix\n");
        system("pause");
        exit(1);
    }
    else {
        printf("The first matrix was:\n");
        testprint(testOne);
        printf("The second matrix was:\n");
        testprint(testTwo);
        printf("The matrix product is:\n");
        testprint(result);

    }

    if (engOutputBuffer(ep, buffer, BUFSIZE)) {
        fprintf(stderr, "\nCan't create buffer for MATLAB output\n");
        system("pause");
        return 1;
    }
    buffer[BUFSIZE] = '\0';

    engEvalString(ep, "whos"); // whos is a handy MATLAB command that generates a list of all current variables
    printf("%s\n", buffer);

    mxDestroyArray(testOne);
    mxDestroyArray(testTwo);
    mxDestroyArray(result);
    mxDestroyArray(testArrayEigen);
    testArrayEigen = NULL;
    testOne = NULL;
    testTwo = NULL;
    result = NULL;
    if (engClose(ep)) {
        fprintf(stderr, "\nFailed to close MATLAB engine\n");
    }

    system("pause"); // So the terminal window remains open long enough for you to read it
    return 0; // Because main returns 0 for successful completion




}