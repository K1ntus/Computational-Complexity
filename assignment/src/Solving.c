#include "Solving.h"
#include <stdlib.h>
#include "Z3Tools.h"
#include <string.h>  // <cstring> en C++


Z3_ast getNodeVariable(Z3_context ctx, int number, int position, int k, int node) {
    // Z3_ast mk_var(Z3_context ctx, const char * name, Z3_sort ty)
    // Z3_ast res = mk_var(ctx, "sommet!" )
//     Z3_ast mk_bool_var(Z3_context ctx, const char * name)
// {
//     Z3_sort ty = Z3_mk_bool_sort(ctx);
//     return mk_var(ctx, name, ty);
// }
    char buffer[1024];
    sprintf(buffer, "x%d,%d,%d,%d", number, position, k, node);
    Z3_ast res = mk_bool_var(ctx,buffer);
    printf("Variable %s created.\n",Z3_ast_to_string(ctx,res));
    return res;
}




Z3_ast graphsToPathFormula( Z3_context ctx, Graph *graphs,unsigned int numGraphs, int pathLength) {
    for(unsigned int graph_number = 0; graph_number < numGraphs; graph_number++) {
        printf("- Graph Number: %d\n", graph_number);
        // Z3_ast* res_array = (Z3_ast*) malloc(sizeof(Z3_ast) * 1500);


        for(unsigned int node_number = 0; node_number < pathLength; node_number++){
            printf("--- Node Number: %d\n", node_number);
            
            Z3_ast x = getNodeVariable(ctx, graph_number, node_number, numGraphs, 0);
            Z3_ast y = getNodeVariable(ctx, graph_number, node_number+1, numGraphs, 0);
             

            Z3_ast resTab[2] = {x,y};
            Z3_ast res = Z3_mk_and(ctx,2,resTab);
            printf("Formula %s created.\n",Z3_ast_to_string(ctx,res));

        }
        printf("\n");

        // free(res_array);


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