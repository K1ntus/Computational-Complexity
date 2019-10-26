#include "Solving.h"
#include <stdlib.h>
#include "Z3Tools.h"
#include <string.h>  // <cstring> en C++




Z3_ast graphsToValideFormula( Z3_context ctx, Graph *graphs,unsigned int numGraphs, int pathLength);
Z3_ast graphsToExistsPath( Z3_context ctx, Graph *graphs,unsigned int numGraphs, int pathLength);




Z3_ast getNodeVariable(Z3_context ctx, int number, int position, int k, int node) {
    char buffer[64];
    sprintf(buffer, "x %d,%d,%d,%d", number, position, k, node);
    
    Z3_ast res = mk_bool_var(ctx,buffer);

    printf("----- Variable %s created.\n",Z3_ast_to_string(ctx,res));
    return res;
}


Z3_ast graphsToPathFormula( Z3_context ctx, Graph *graphs,unsigned int numGraphs, int pathLength) {

    printf("\n\n+ Graphes validity. +\n");
    Z3_ast valide_formula = graphsToValideFormula(ctx, graphs, numGraphs, pathLength);

    printf("\n\n+ Path between edges. +\n");
    Z3_ast edge_formula = graphsToExistsPath(ctx, graphs, numGraphs, pathLength);

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




Z3_ast graphsToValideFormula( Z3_context ctx, Graph *graphs,unsigned int numGraphs, int pathLength) {
    Z3_ast res_all_graph[numGraphs+1];
    Z3_ast res_final_formula;
    Z3_ast res_two_node = NULL;

    for(unsigned int graph_number = 0; graph_number < numGraphs; graph_number++) {
        printf("- Graph Number: %d\n", graph_number);
        printf("--- Graph have: %d nodes.\n",graphs[graph_number].numNodes);

        // printGraph(graphs[graph_number]);


        Z3_ast source = NULL;
        Z3_ast target = NULL;
        res_two_node = NULL;
        // Z3_ast* res_array = (Z3_ast*) malloc(sizeof(Z3_ast) * 1500);
        for(unsigned int node_number = 0; node_number < orderG(graphs[graph_number]); node_number++){
            Graph tmp_graph = graphs[graph_number];
            printf("--- Node Number: %d\n", node_number);
            

            if(isSource(tmp_graph, node_number)){
                printf("----- Found source. \n");
                source = getNodeVariable(ctx, graph_number, 0, pathLength, node_number);
            }
            if(isTarget(tmp_graph, node_number)){
                printf("----- Found target. \n");
                target = getNodeVariable(ctx, graph_number, pathLength, pathLength, node_number);
            }

            if(source == NULL || target == NULL)
                continue;

            Z3_ast node[2] = {source,target};

            res_two_node = Z3_mk_and(ctx,2,node);
            printf("+ Formula %s created.\n",Z3_ast_to_string(ctx,res_two_node));
            break;

        }

        if(res_two_node == NULL) {
            printf("----- Lack of at least the source or the target edge.\n");
            res_two_node = mk_bool_var(ctx,"false");
        }
        res_all_graph[graph_number] = res_two_node;

        printf("\n");
    }

    //Concatene each formula coming from each graph
    for(int i = 0; i < numGraphs; i++) {    
        res_final_formula = Z3_mk_and(ctx, numGraphs, res_all_graph);
    }
    printf("-----> %s\n",Z3_ast_to_string(ctx,res_final_formula));

    return res_final_formula;
}




Z3_ast graphsToExistsPath( Z3_context ctx, Graph *graphs,unsigned int numGraphs, int pathLength) {
    Z3_ast res_all_graph[numGraphs+1];
    Z3_ast res_final_formula;
    Z3_ast formula_for_source_id;
    Z3_ast res_graph;
    int size_nb_paire = 0;

    for(unsigned int graph_number = 0; graph_number < numGraphs; graph_number++) {
        printf("- Graph Number: %d\n", graph_number);
        printf("--- Graph have: %d nodes.\n", sizeG(graphs[graph_number]));

        // printGraph(graphs[graph_number]);


        Z3_ast source = NULL;
        Z3_ast target = NULL;
        formula_for_source_id = NULL;
        

        Z3_ast * res_graph_formula = (Z3_ast *) malloc(sizeof(Z3_ast) * sizeG(graphs[graph_number]));
        size_nb_paire = 0;
        for(unsigned int source_id = 0; source_id < orderG(graphs[graph_number]); source_id++){ //Check if last node is tested ?
            Graph tmp_graph = graphs[graph_number];
            printf("--- Source Number: %d\n", source_id);


            for(unsigned int target_id = 0; target_id < orderG(tmp_graph); target_id++) {
                printf("--- Target Number: %d\n", target_id);


                if(isEdge(tmp_graph, source_id, target_id) && target_id != source_id){
                    source = getNodeVariable(ctx, graph_number, source_id, pathLength, source_id);
                    target = getNodeVariable(ctx, graph_number, source_id+1, pathLength, target_id);
                    printf("* Found an edge.\n");
                } else {
                    continue;
                }

                Z3_ast node[2] = {source,target};
                res_graph_formula[size_nb_paire] = Z3_mk_and(ctx,2,node);
                size_nb_paire += 1;

            }

        }

        res_all_graph[graph_number] = Z3_mk_or(ctx,size_nb_paire,res_graph_formula);
        printf("+ Formula %s created.\n",Z3_ast_to_string(ctx,res_all_graph[graph_number]));
        printf("\n");
        free(res_graph_formula);
    }

    //Concatene each formula coming from each graph
    for(int i = 0; i < numGraphs; i++) {    
        res_final_formula = Z3_mk_and(ctx, numGraphs, res_all_graph);
    }
    printf("-----> %s\n",Z3_ast_to_string(ctx,res_final_formula));

    return res_final_formula;
}