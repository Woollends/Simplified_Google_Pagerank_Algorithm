//Caleb Woollends 23002355
#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"
#define BUFFER 512

int get_web_dimension(FILE* web) {

	int dim = 0; //variable for the dimension of the maze
	char line_buffer[BUFFER];

	fgets(line_buffer, BUFFER, web); //assinging the first line of web to linebuffer


	for (int i = 0; line_buffer[i] != '\0'&&line_buffer[i]!='\n'; ++i) { //loop for dim, skipping space charachters
		if (line_buffer[i] != ' ') {
			dim++;
		}
		
	}

	fseek(web, 0, SEEK_SET); //resest the file pointer to the start of the file
	return dim; //returns the dimensions of web.txt
}



int main(void) {

	Engine* ep = NULL; // A pointer to a MATLAB engine object
	mxArray* webmatrix = NULL, * result = NULL; // mxArray is the fundamental type underlying MATLAB data
	FILE* file; //File pointer
	char  line_buffer[BUFFER]; //char for fgets
	int row = 0; 
	int column = 0;
	int dimension;
	int** web = NULL; //place to store the web.txt
	


	file = fopen("web.txt", "r"); //open file

	if (file == NULL) { //check if it opened
		printf("error, file did not open");
		return 0;
	}

	dimension = get_web_dimension(file); //calling on the function to get the dimensions

	web = (int**)calloc(dimension, sizeof(int**)); //allocating space based off the dimensions

	for (row = 0; row < dimension; ++row) {
		web[row] = (int*)calloc(dimension, sizeof(int*));
	}

	row = 0;
	
	while (fgets(line_buffer, BUFFER, file)) { //populating the allocated spacewith the approprorate values
		
		int column = 0;

		for (int i = 0; line_buffer[i] != '\0' && column < dimension; ++i) {
			if (line_buffer[i] != ' ') {

				web[row][column] = line_buffer[i]-'0';
				column++;
			}
		}
		row++;
	}
	
	fclose(file); //closing file

	webmatrix = mxCreateDoubleMatrix(dimension, dimension, mxREAL); //setting up webmatrix to be implements into matlab

	double* pr = mxGetPr(webmatrix); //itterative loop to copy values into the correct spaces, double typing since output is decimal
	for (int i = 0; i < dimension; ++i) {
		for (int j = 0; j < dimension; ++j) {
			pr[i + j * dimension] = web[i][j];
		}
	}
	

	if (!(ep = engOpen(NULL))) { //opneing matlab, ensuring it opend
		fprintf(stderr, "\nCan't start MATLAB engine\n");
		system("pause");
		return 1;
	}

	engPutVariable(ep, "ConnectivityMatrix", webmatrix); //lines to find page rank in matlab
	engEvalString(ep, "[rows, columns] = size(ConnectivityMatrix)");
	engEvalString(ep, "dimension = size(ConnectivityMatrix, 1)");
	engEvalString(ep, "columnsums = sum(ConnectivityMatrix, 1)");
	engEvalString(ep, "p = 0.85");
	engEvalString(ep, "zerocolumns = find(columnsums~=0)");
	engEvalString(ep, "D = sparse( zerocolumns, zerocolumns, 1./columnsums(zerocolumns), dimension, dimension)");
	engEvalString(ep, "StochasticMatrix = ConnectivityMatrix * D");
	engEvalString(ep, "[row, column] = find(columnsums==0)");
	engEvalString(ep, "StochasticMatrix(:, column) = 1./dimension");
	engEvalString(ep, "Q = ones(dimension, dimension);");
	engEvalString(ep, "TransitionMatrix = p * StochasticMatrix + (1 - p) * (Q/dimension)");
	engEvalString(ep, "PageRank = ones(dimension, 1)");
	engEvalString(ep, "for i = 1:100 PageRank = TransitionMatrix * PageRank; end;");
	engEvalString(ep, "PageRank = PageRank / sum(PageRank)");

	if ((result = engGetVariable(ep, "PageRank")) == NULL) { //reciving the values of pagerank from matlab
		fprintf(stderr, "\nFailed to retrieve testArrayMatrix\n");
		system("pause");
		exit(1);
	}

	size_t sizeOfResult = mxGetNumberOfElements(result); //prinitng the results in the specifed way
	size_t i = 0;
	printf("NODE  RANK\n---   ----\n");
	for (i = 0; i < sizeOfResult; ++i) {
		printf("%i     %.4f\n",i+1, *(mxGetPr(result) + i));

	}
	
	for (int i = 0; i < row; i++) { //deallocating memory and closing matlab
		free(web[i]);
	}
	free(web);
	mxDestroyArray(webmatrix); 
	engClose(ep);  


	return 0;
}