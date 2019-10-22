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
    sprintf(buffer, "x %d,%d,%d,%d", number, position, k, node);
    Z3_ast res = mk_bool_var(ctx,buffer);
    printf("----- Variable %s created.\n",Z3_ast_to_string(ctx,res));
    return res;
}


Z3_ast graphsToValideFormula( Z3_context ctx, Graph *graphs,unsigned int numGraphs, int pathLength) {
    Z3_ast res_all_graph[numGraphs+1];
    Z3_ast res_final_formula;
    Z3_ast res_two_node;

    for(unsigned int graph_number = 0; graph_number < numGraphs; graph_number++) {
        printf("- Graph Number: %d\n", graph_number);
        printf("--- Graph have: %d nodes.\n",graphs[graph_number].numNodes);

        // printGraph(graphs[graph_number]);


        Z3_ast source = NULL;
        Z3_ast target = NULL;
        // Z3_ast* res_array = (Z3_ast*) malloc(sizeof(Z3_ast) * 1500);
        for(unsigned int node_number = 0; node_number < sizeG(graphs[graph_number]); node_number++){
            Graph tmp_graph = graphs[graph_number];
            printf("--- Node Number: %d\n", node_number);
            

            if(isSource(tmp_graph, node_number)){
                printf("----- Found source. \n");
                source = getNodeVariable(ctx, graph_number, 0, numGraphs, node_number);
            }
            if(isTarget(tmp_graph, node_number)){
                printf("----- Found target. \n");
                target = getNodeVariable(ctx, graph_number, pathLength, numGraphs, node_number);
            }

            if(source == NULL || target == NULL)
                continue;

            Z3_ast node[2] = {source,target};

            res_two_node = Z3_mk_and(ctx,2,node);
            printf("+ Formula %s created.\n",Z3_ast_to_string(ctx,res_two_node));
            break;

        }
        res_all_graph[graph_number] = res_two_node;

        printf("\n");
    }

    for(int i = 0; i < numGraphs; i++) {
        res_final_formula = Z3_mk_and(ctx, numGraphs, res_all_graph);
    }
    printf("-----> %s\n",Z3_ast_to_string(ctx,res_final_formula));

    return res_final_formula;
}


Z3_ast graphsToPathFormula( Z3_context ctx, Graph *graphs,unsigned int numGraphs, int pathLength) {

    Z3_ast res1 = graphsToValideFormula(ctx, graphs, numGraphs, pathLength);
    // for(unsigned int graph_number = 0; graph_number < numGraphs; graph_number++) {
    //     printf("- Graph Number: %d\n", graph_number);
    //     // Z3_ast* res_array = (Z3_ast*) malloc(sizeof(Z3_ast) * 1500);


    //     for(unsigned int node_number = 0; node_number < pathLength; node_number++){
    //         printf("--- Node Number: %d\n", node_number);
            
    //         Z3_ast x = getNodeVariable(ctx, graph_number, node_number, numGraphs, 0);
    //         Z3_ast y = getNodeVariable(ctx, graph_number, node_number+1, numGraphs, 0);
             

    //         Z3_ast resTab[2] = {x,y};
    //         Z3_ast res = Z3_mk_and(ctx,2,resTab);
    //         printf("Formula %s created.\n",Z3_ast_to_string(ctx,res));

    //     }
    //     printf("\n");

    //     // free(res_array);


    // }

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