#include "Solving.h"



Z3_ast getNodeVariable(Z3_context ctx, int number, int position, int k, int node) {
    // Z3_ast mk_var(Z3_context ctx, const char * name, Z3_sort ty)
    // Z3_ast res = mk_var(ctx, "sommet!" )
    return NULL;
}


Z3_ast graphsToPathFormula( Z3_context ctx, Graph *graphs,unsigned int numGraphs, int pathLength) {
    for(unsigned int graph_number = 0; graph_number < numGraphs; graph_number++) {
        printf("- Graph Number: %d\n", graph_number);

        for(unsigned int node_number = 0; node_number < pathLength; node_number++){
            printf("--- Node Number: %d\n", node_number);
            getNodeVariable(ctx, graph_number, node_number, numGraphs, 0);

        }


    }

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