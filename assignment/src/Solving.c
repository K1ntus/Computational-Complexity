#include "Solving.h"
#include <stdlib.h>
#include "Z3Tools.h"
#include <string.h> // <cstring> en C++

int binomialCoeff(int n, int k);
Z3_ast graphsToValideFormula(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength);
Z3_ast uniqueVertexAtEachIndex(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength);
Z3_ast atLeastOneVertexAtEachIndex(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength);
Z3_ast atMostOneVertexAtEachIndex(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength);



Z3_ast getNodeVariable(Z3_context ctx, int number, int position, int k, int node)
{
    char buffer[64];
    sprintf(buffer, "x %d,%d,%d,%d", number, position, k, node);

    Z3_ast res = mk_bool_var(ctx, buffer);

    // printf("----- Variable %s created.\n", Z3_ast_to_string(ctx, res));
    return res;
}

Z3_ast graphsToPathFormula(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength)
{

    Z3_ast valide_formula = graphsToValideFormula(ctx, graphs, numGraphs, pathLength);
    Z3_ast phi1_1 = uniqueVertexAtEachIndex(ctx, graphs, numGraphs, pathLength);
    Z3_ast phi1_2 = atLeastOneVertexAtEachIndex(ctx, graphs, numGraphs, pathLength);
    Z3_ast phi1_3 = atMostOneVertexAtEachIndex(ctx, graphs, numGraphs, pathLength);

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

Z3_ast graphsToFullFormula(Z3_context ctx, Graph *graphs, unsigned int numGraphs)
{

    return NULL;
}

int getSolutionLengthFromModel(Z3_context ctx, Z3_model model, Graph *graphs)
{

    return 0;
}

void printPathsFromModel(Z3_context ctx, Z3_model model, Graph *graphs, int numGraph, int pathLength)
{

    return;
}

void createDotFromModel(Z3_context ctx, Z3_model model, Graph *graphs, int numGraph, int pathLength, char *name)
{

    return;
}

Z3_ast uniqueVertexAtEachIndex(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength)
{
    Z3_ast res_all_graph[numGraphs]; //Array that will contains every z3 formula for each graph
    Z3_ast res_final_formula;        //The final formula which concatene every graphs formula
    //Loop through each graph
    int graph_count=0;
    for (unsigned int graph_number = 0; graph_number < numGraphs; graph_number++)
    {
        //Loop every node in the graph
        int size_graph = orderG(graphs[graph_number]);
        Z3_ast all_vertex_formula[size_graph];
        int nb_subset = binomialCoeff(pathLength, 2);
        Z3_ast subset_formula[nb_subset];

        for (unsigned int node_number = 0; node_number < size_graph; node_number++)
        {
           
            int subset_count =0;
            //creat terms for subset of 2 elements from pathLength
            for (int i = 1; i <= pathLength; i++)
            {
                Z3_ast termA = getNodeVariable(ctx, graph_number, i, pathLength, node_number);
                Z3_ast negTermA = Z3_mk_not(ctx,termA);
                for (int j = i + 1; j <= pathLength; j++)
                {
                    Z3_ast termB = getNodeVariable(ctx, graph_number, j, pathLength, node_number);
                    Z3_ast negTermB = Z3_mk_not(ctx,termB);
                    Z3_ast node[2] = {negTermA, negTermB};
                    subset_formula[subset_count] = Z3_mk_or(ctx, 2, node);
                    subset_count++;
                }
            }

            all_vertex_formula[node_number] = Z3_mk_and(ctx, nb_subset, subset_formula);
        }
        res_all_graph[graph_count] = Z3_mk_and(ctx, size_graph, all_vertex_formula);
        graph_count++;
    }
    res_final_formula = Z3_mk_and(ctx, numGraphs, res_all_graph);
    printf("uniqueVertexAtEachIndex-----> %s\n", Z3_ast_to_string(ctx, res_final_formula));

    return res_final_formula;
}

Z3_ast atLeastOneVertexAtEachIndex(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength){
    Z3_ast res_all_graph[numGraphs]; //Array that will contains every z3 formula for each graph
    Z3_ast res_final_formula;        //The final formula which concatene every graphs formula
    //Loop through each graph
    for (unsigned int graph_number = 0; graph_number < numGraphs; graph_number++)
    {
        // loop every index of pathLength
        Z3_ast index_formula[pathLength];
        int index_count=0;
        for(int j=1;j<=pathLength;j++){
            // loop every node in the graph
            int size_graph = orderG(graphs[graph_number]);
            Z3_ast vertex_formula[size_graph];
            for(unsigned int node_number = 0; node_number < size_graph; node_number++){
                Z3_ast term = getNodeVariable(ctx, graph_number, j, pathLength, node_number);
                vertex_formula[node_number] = term;
            }
            index_formula[index_count] = Z3_mk_or(ctx, size_graph, vertex_formula);
            index_count++;
        }
        res_all_graph[graph_number] = Z3_mk_and(ctx, pathLength, index_formula);
    }
    res_final_formula = Z3_mk_and(ctx, numGraphs, res_all_graph);
    printf("atLeastOneVertexAtEachIndex-----> %s\n", Z3_ast_to_string(ctx, res_final_formula));

    return res_final_formula;
}

Z3_ast atMostOneVertexAtEachIndex(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength){
    Z3_ast res_all_graph[numGraphs]; //Array that will contains every z3 formula for each graph
    Z3_ast res_final_formula;        //The final formula which concatene every graphs formula
    //Loop through each graph
    for (unsigned int graph_number = 0; graph_number < numGraphs; graph_number++)
    {
        // loop every index of pathLength
        Z3_ast index_formula[pathLength];
        int index_count = 0;

        int size_graph = orderG(graphs[graph_number]);

        for (int j = 1; j <= pathLength; j++)
        {
            // loop every node in the graph
            int nb_subset = binomialCoeff(size_graph, 2);
            Z3_ast subset_formula[nb_subset];
            int subset_count =0;
            for (unsigned int node_number_A = 0; node_number_A < size_graph; node_number_A++)
            {
                
                Z3_ast termA = getNodeVariable(ctx, graph_number, j, pathLength, node_number_A);
                Z3_ast negTermA = Z3_mk_not(ctx,termA);
                // loop every node in the graph
                for(unsigned int node_number_B = node_number_A+1; node_number_B < size_graph; node_number_B++){
                    Z3_ast termB = getNodeVariable(ctx, graph_number, j, pathLength, node_number_B);
                    Z3_ast negTermB = Z3_mk_not(ctx,termB);
                    Z3_ast node[2] = {negTermA, negTermB};
                    subset_formula[subset_count] = Z3_mk_or(ctx, 2, node);
                    subset_count++;
                }
                
            }
            index_formula[index_count] = Z3_mk_and(ctx, nb_subset, subset_formula);
            index_count++;
            
        }

        res_all_graph[graph_number] = Z3_mk_and(ctx, pathLength, index_formula);
    }
    res_final_formula = Z3_mk_and(ctx, numGraphs, res_all_graph);
    printf("atMostOneVertexAtEachIndex-----> %s\n", Z3_ast_to_string(ctx, res_final_formula));

    return res_final_formula;
}

Z3_ast graphsToValideFormula(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength)
{

    Z3_ast res_all_graph[numGraphs + 1]; //Array that will contains every z3 formula for each graph
    Z3_ast res_final_formula;            //The final formula which concatene every graphs formula
    Z3_ast res_two_node = NULL;          //A node that will contain two valuation of a graph

    // Search every graphe
    for (unsigned int graph_number = 0; graph_number < numGraphs; graph_number++)
    {
        printf("- Graph Number: %d\n", graph_number);
        printf("--- Graph have: %d nodes.\n", graphs[graph_number].numNodes);

        Z3_ast source = NULL; //Node that will contains the source
        Z3_ast target = NULL; //Node that will contains the target
        res_two_node = NULL;  //reinitialize the value of the node

        // Look every node of the graph
        //TODO use orderG for nodes of the graph
        for (unsigned int node_number = 0; node_number < sizeG(graphs[graph_number]); node_number++)
        {
            Graph tmp_graph = graphs[graph_number]; //Local copy bcz more useful
            printf("--- Node Number: %d\n", node_number);

            if (isSource(tmp_graph, node_number))
            { //If node is the source, then save a sat valuation
                printf("----- Found source. \n");
                source = getNodeVariable(ctx, graph_number, 0, pathLength, node_number);
            }
            if (isTarget(tmp_graph, node_number))
            { //If node is the target, then save a sat valuation
                printf("----- Found target. \n");
                target = getNodeVariable(ctx, graph_number, pathLength, pathLength, node_number);
            }

            //If not both source or target nodes are defined
            // next iteration and search for new source or target
            if (source == NULL || target == NULL)
                continue;

            //Reached when source and target nodes has been find
            Z3_ast node[2] = {source, target};

            //Set a new and node with source and destination for this graph
            res_two_node = Z3_mk_and(ctx, 2, node);
            printf("+ Formula %s created.\n", Z3_ast_to_string(ctx, res_two_node));
            break;
        }

        if (res_two_node == NULL)
        {
            printf("----- Lack of at least the source or the target edge.\n");
            res_two_node = mk_bool_var(ctx, "false");
        }
        res_all_graph[graph_number] = res_two_node;

        printf("\n");
    }

    //Concatene each formula coming from each graph
    for (int i = 0; i < numGraphs; i++)
    {
        res_final_formula = Z3_mk_and(ctx, numGraphs, res_all_graph);
    }
    printf("-----> %s\n", Z3_ast_to_string(ctx, res_final_formula));

    return res_final_formula;
}

int binomialCoeff(int n, int k) 
{ 
  // Base Cases 
  if (k==0 || k==n) 
    return 1; 
  
  // Recur 
  return  binomialCoeff(n-1, k-1) + binomialCoeff(n-1, k); 
} 
