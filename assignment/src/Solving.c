#include "Solving.h"



Z3_ast getNodeVariable(Z3_context ctx, int number, int position, int k, int node) {

    return NULL;
}


Z3_ast graphsToPathFormula( Z3_context ctx, Graph *graphs,unsigned int numGraphs, int pathLength) {


    return NULL;
}


Z3_ast graphsToFullFormula( Z3_context ctx, Graph *graphs,unsigned int numGraphs) {

    return NULL;
}



int getSolutionLengthFromModel(Z3_context ctx, Z3_model model, Graph *graphs) {



    return 0;
}



void printPathsFromModel(Z3_context ctx, Z3_model model, Graph *graphs, int numGraph, int pathLength) {


    return;
}




void createDotFromModel(Z3_context ctx, Z3_model model, Graph *graphs, int numGraph, int pathLength, char* name) {


    return;
}