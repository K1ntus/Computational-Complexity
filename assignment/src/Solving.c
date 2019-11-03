#include "Solving.h"
#include <stdlib.h>
#include "Z3Tools.h"
#include <unistd.h> //pipe
#include <stdio.h>  //File
#include <string.h> // <cstring> en C++
#include <assert.h>

extern bool mode_verbose;
extern bool mode_extended_verbose;

Z3_ast graphsToValideFormula(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength);
int sat_checker(Z3_context ctx, Z3_ast formula);
int sat_checker_print(Z3_context ctx, Z3_ast formula);

int binomialCoeff(int n, int k);
Z3_ast graphsToValideFormula(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength);
Z3_ast uniqueVertexAtEachIndex(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength);
Z3_ast atLeastOneVertexAtEachIndex(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength);
Z3_ast atMostOneVertexAtEachIndex(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength);
Z3_ast ExistsPath(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength);

void sortAndDisplayPath(Graph g, int nodes[], int pathLength);

int *sortDotPath(Graph g, int nodes[], int pathLength);
int *getDotPathsFromModel(Z3_context ctx, Z3_model model, Graph *graphs, int numGraph, int pathLength, int graph_number);

void testSubformula(Z3_context ctx, Z3_ast phi1_1, Z3_ast phi1_2, Z3_ast phi1_3, Z3_ast valide_formula, Z3_ast edge_between_nodes);
Z3_ast getNodeVariable(Z3_context ctx, int number, int position, int k, int node)
{
    char buffer[64];
    sprintf(buffer, "x %d,%d,%d,%d", number, position, k, node);
    Z3_ast res = mk_bool_var(ctx, buffer);

    if (mode_extended_verbose)
        printf("----- Variable %s created.\n", Z3_ast_to_string(ctx, res));
    return res;
}

Z3_ast graphsToPathFormula(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength)
{
    Z3_ast valide_formula = graphsToValideFormula(ctx, graphs, numGraphs, pathLength);
    Z3_ast phi1_1 = uniqueVertexAtEachIndex(ctx, graphs, numGraphs, pathLength);
    Z3_ast phi1_2 = atLeastOneVertexAtEachIndex(ctx, graphs, numGraphs, pathLength);
    Z3_ast phi1_3 = atMostOneVertexAtEachIndex(ctx, graphs, numGraphs, pathLength);

    Z3_ast edge_between_nodes = ExistsPath(ctx, graphs, numGraphs, pathLength);

    testSubformula(ctx, phi1_1, phi1_2, phi1_3, valide_formula, edge_between_nodes);
    Z3_ast tmp[5] = {phi1_1, phi1_2, phi1_3, edge_between_nodes, valide_formula};
    Z3_ast res_formula = Z3_mk_and(ctx, 5, tmp);
    if (mode_extended_verbose)
    {
        printf("- Check Final Formula\n");
        if (sat_checker_print(ctx, res_formula) == 1)
            return res_formula;
    }
    else
    {
        if (sat_checker(ctx, res_formula) == 1)
            return res_formula;
    }
    return NULL;
}

Z3_ast graphsToFullFormula(Z3_context ctx, Graph *graphs, unsigned int numGraphs)
{
    assert(graphs);
    Z3_ast res_final_formula; //The final formula which concatene every graphs formula
    //Pick the number of first graph's vertex -1 as the max pathLength.
    int max_pathLength = orderG(graphs[0]) - 1;
    for (int k = max_pathLength; k >= 0; k--)
    {
        res_final_formula = graphsToPathFormula(ctx, graphs, numGraphs, k);
        if (res_final_formula != NULL)
        {
            return res_final_formula;
        }
    }
    return NULL;
}

/*
* @brief if all graphs of @p graphs contain an accepting path of common length, then the first graph also.
*       Test only with the first graph.
* @param ctx The solver context.
* @param model A variable assignment.
* @param graphs An array of graphs.
* @return int The length of a common simple accepting path in all graphs from @p graphs.
*/
int getSolutionLengthFromModel(Z3_context ctx, Z3_model model, Graph *graphs)
{
    assert(model);
    assert(graphs);
    int resPathLength = 0;
    Graph firstGraph = graphs[0];
    int sizeGraph = orderG(firstGraph);
    int maxPathLength = sizeGraph - 1;

    // try all pathLength
    for (int pathLength = maxPathLength; pathLength >= 0; pathLength--)
    {

        // loop through pathLength
        for (int j = 0; j <= pathLength; j++)
        {
            Z3_ast tmpVar = graphsToPathFormula(ctx, graphs, 1, pathLength);
            bool satisfiedCheck = valueOfVarInModel(ctx, model, tmpVar);
            if (satisfiedCheck)
            {
                return pathLength;
            }
        }
    }

    return resPathLength;
}

size_t FindIndex(int *a, size_t size, int value)
{
    size_t index = 0;

    while (index < size && a[index] != value)
        ++index;

    return (index == size ? -1 : index);
}

void createDotFromModel(Z3_context ctx, Z3_model model, Graph *graphs, int numGraph, int pathLength, char *name)
{
    FILE *save_file;
    if (name == 0x0)
        save_file = fopen("output/NAME-lLENGTH.dot", "w");
    else
        save_file = fopen(name, "w");

    int file_descriptor = fileno(save_file);
    int fd_stdout = dup(1);
    dup2(file_descriptor, 1);

    printf("digraph G{\n");
    for (int graph_number = 0; graph_number < numGraph; graph_number++)
    {
        printf("\tsubgraph %d{\n", graph_number);
        int *nodes = getDotPathsFromModel(ctx, model, graphs, numGraph, pathLength, graph_number);
        {
            for (int node_id = 0; node_id < orderG(graphs[graph_number]); node_id++)
            {
                if (isTarget(graphs[graph_number], node_id))
                    printf("\t\tG%d_%s [final=1,color=red];\n", graph_number, getNodeName(graphs[graph_number], node_id));
                else if (isSource(graphs[graph_number], node_id))
                    printf("\t\tG%d_%s [initial=1,color=green];\n", graph_number, getNodeName(graphs[graph_number], node_id));
                else
                    printf("\t\tG%d_%s;\n", graph_number, getNodeName(graphs[graph_number], node_id));

                size_t indexValue = FindIndex(nodes, pathLength, node_id);
                for (int node2 = 0; node2 < orderG(graphs[graph_number]); node2++)
                {
                    if (isEdge(graphs[graph_number], node_id, node2))
                    {
                        if ((indexValue == -1) || getNodeName(graphs[graph_number], nodes[indexValue + 1]) != getNodeName(graphs[graph_number], node2))
                            printf("\t\tG%d_%s->G%d_%s;\n", graph_number, getNodeName(graphs[graph_number], node_id), graph_number, getNodeName(graphs[graph_number], node2));
                    }
                }
            }

            printf("\t\t");
            for (int j = 0; j < pathLength; j++)
            {
                printf("G%d_%s->", graph_number, getNodeName(graphs[graph_number], nodes[j]));
            }
            printf("G%d_%s [color=blue];\n", graph_number, getNodeName(graphs[graph_number], nodes[pathLength]));

            int edge;
            for (edge = 0; edge < sizeG(graphs[graph_number]); edge++)
            {
            }
            printf("\t\tlabel = \"Graphe %d\";\n", graph_number);
        }
        free(nodes);
        printf("\t}\n");
    }
    printf("}\n");

    dup2(fd_stdout, 1);

    close(file_descriptor);
    close(fd_stdout);
    fclose(save_file);
}

int *getDotPathsFromModel(Z3_context ctx, Z3_model model, Graph *graphs, int numGraph, int pathLength, int graph_number)
{

    int nb_vertex_positions = pathLength + 1;
    int *nodes = (int *)malloc(sizeof(int) * nb_vertex_positions);
    int nodes_counter = 0;
    //loop through nodes
    int size_graph = orderG(graphs[graph_number]);
    for (unsigned int node_number = 0; node_number < size_graph; node_number++)
    {

        //loop through pathLength
        for (int j = 0; j <= pathLength; j++)
        {
            Z3_ast tmp_var = getNodeVariable(ctx, graph_number, j, pathLength, node_number);
            bool satisfied_var = valueOfVarInModel(ctx, model, tmp_var);
            if (satisfied_var)
            {
                if (isSource(graphs[graph_number], node_number))
                {
                    //reserve first index for source
                    nodes[0] = node_number;
                }
                else if (isTarget(graphs[graph_number], node_number))
                {
                    //reserve last index for target
                    nodes[nb_vertex_positions - 1] = node_number;
                }
                else
                {
                    //rest index for other satisfied variable
                    nodes_counter++;
                    nodes[nodes_counter] = node_number;
                }
                break;
            }
        }
    }

    return nodes;
}

void printPathsFromModel(Z3_context ctx, Z3_model model, Graph *graphs, int numGraph, int pathLength)
{
    //loop through graphs
    for (unsigned int graph_number = 0; graph_number < numGraph; graph_number++)
    {

        int nb_vertex_positions = pathLength + 1;
        int nodes[nb_vertex_positions];
        int nodes_counter = 0;
        //loop through nodes
        int size_graph = orderG(graphs[graph_number]);
        for (unsigned int node_number = 0; node_number < size_graph; node_number++)
        {

            //loop through pathLength
            for (int j = 0; j <= pathLength; j++)
            {
                Z3_ast tmp_var = getNodeVariable(ctx, graph_number, j, pathLength, node_number);
                bool satisfied_var = valueOfVarInModel(ctx, model, tmp_var);
                if (satisfied_var)
                {
                    if (isSource(graphs[graph_number], node_number))
                    {
                        //reserve first index for source
                        nodes[0] = node_number;
                    }
                    else if (isTarget(graphs[graph_number], node_number))
                    {
                        //reserve last index for target
                        nodes[nb_vertex_positions - 1] = node_number;
                    }
                    else
                    {
                        //rest index for other satisfied variable
                        nodes_counter++;
                        nodes[nodes_counter] = node_number;
                    }
                    break;
                }
            }
        }
        // check for pathLength
        if (nodes_counter != pathLength - 1)
        {
            printf("printPathsFromModel-->Failed on Graph N°%d\n", graph_number);
        }
        else
        {
            //create path;
            sortAndDisplayPath(graphs[graph_number], nodes, nb_vertex_positions);
        }
    }
    return;
}
/*
 * @brief sort array nodes @p nodes and display the correct path
 * 
 * @param g A graph.
 * @param nodes An array of nodes, first index must be source and last index must be target.
 * @param nb_vertex_positions The size of the array.
*/
void sortAndDisplayPath(Graph g, int nodes[], int nb_vertex_positions)
{
    int path[nb_vertex_positions];
    // printf("nb_vertex %d\n",nb_vertex_positions);
    //first index contains a path's source
    path[0] = nodes[0];
    int path_index = 0;
    for (int path_index = 0; path_index < nb_vertex_positions; path_index++)
    {
        for (int node_index = 1; node_index < nb_vertex_positions; node_index++)
        {
            if (isEdge(g, path[path_index], nodes[node_index]))
            {
                path[path_index + 1] = nodes[node_index];
                break;
            }
        }
    }

    for (int i = 0; i < nb_vertex_positions; i++)
    {
        if (i != nb_vertex_positions - 1)
        {
            printf("%s->", getNodeName(g, path[i]));
        }
        else
        {
            printf("%s\n", getNodeName(g, path[i]));
        }
    }
}

int *sortDotPath(Graph g, int nodes[], int nb_vertex_positions)
{
    int *path = (int *)malloc(sizeof(int) * nb_vertex_positions);
    // printf("nb_vertex %d\n",nb_vertex_positions);
    //first index contains a path's source
    path[0] = nodes[0];
    int path_index = 0;
    for (int path_index = 0; path_index < nb_vertex_positions; path_index++)
    {
        for (int node_index = 1; node_index < nb_vertex_positions; node_index++)
        {
            if (isEdge(g, path[path_index], nodes[node_index]))
            {
                path[path_index + 1] = nodes[node_index];
                break;
            }
        }
    }

    return path;
    // for (int i = 0; i < nb_vertex_positions; i++)
    // {
    //     if (i != nb_vertex_positions - 1)
    //     {
    //         printf("%s->", getNodeName(g, path[i]));
    //     }
    //     else
    //     {
    //         printf("%s [color=blue];\n", getNodeName(g, path[i]));
    //     }
    // }
}

Z3_ast uniqueVertexAtEachIndex(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength)
{
    Z3_ast res_all_graph[numGraphs]; //Array that will contains every z3 formula for each graph
    Z3_ast res_final_formula;        //The final formula which concatene every graphs formula
    int nb_vertex_positions = pathLength + 1;
    //Loop through each graph
    int graph_count = 0;
    for (unsigned int graph_number = 0; graph_number < numGraphs; graph_number++)
    {
        //Loop every node in the graph
        int size_graph = orderG(graphs[graph_number]);
        Z3_ast all_vertex_formula[size_graph];
        int nb_subset = binomialCoeff(nb_vertex_positions, 2);
        Z3_ast subset_formula[nb_subset];

        for (unsigned int node_number = 0; node_number < size_graph; node_number++)
        {

            int subset_count = 0;
            //creat terms for subset of 2 elements from nb_vertex_positions
            for (int i = 0; i <= pathLength; i++)
            {
                Z3_ast termA = getNodeVariable(ctx, graph_number, i, pathLength, node_number);
                Z3_ast negTermA = Z3_mk_not(ctx, termA);
                for (int j = i + 1; j <= pathLength; j++)
                {
                    Z3_ast termB = getNodeVariable(ctx, graph_number, j, pathLength, node_number);
                    Z3_ast negTermB = Z3_mk_not(ctx, termB);
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
    if (mode_extended_verbose)
        printf("uniqueVertexAtEachIndex-----> %s\n", Z3_ast_to_string(ctx, res_final_formula));

    return res_final_formula;
}

Z3_ast atLeastOneVertexAtEachIndex(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength)
{
    Z3_ast res_all_graph[numGraphs]; //Array that will contains every z3 formula for each graph
    Z3_ast res_final_formula;        //The final formula which concatene every graphs formula
    int nb_vertex_positions = pathLength + 1;
    //Loop through each graph
    for (unsigned int graph_number = 0; graph_number < numGraphs; graph_number++)
    {
        // loop every index of pathLength
        Z3_ast index_formula[nb_vertex_positions];
        int index_count = 0;
        for (int j = 0; j <= pathLength; j++)
        {
            // loop every node in the graph
            int size_graph = orderG(graphs[graph_number]);
            Z3_ast vertex_formula[size_graph];
            for (unsigned int node_number = 0; node_number < size_graph; node_number++)
            {
                Z3_ast term = getNodeVariable(ctx, graph_number, j, pathLength, node_number);
                vertex_formula[node_number] = term;
            }
            index_formula[index_count] = Z3_mk_or(ctx, size_graph, vertex_formula);
            index_count++;
        }
        res_all_graph[graph_number] = Z3_mk_and(ctx, nb_vertex_positions, index_formula);
    }
    res_final_formula = Z3_mk_and(ctx, numGraphs, res_all_graph);
    if (mode_extended_verbose)
        printf("atLeastOneVertexAtEachIndex-----> %s\n", Z3_ast_to_string(ctx, res_final_formula));

    return res_final_formula;
}

Z3_ast atMostOneVertexAtEachIndex(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength)
{
    Z3_ast res_all_graph[numGraphs]; //Array that will contains every z3 formula for each graph
    Z3_ast res_final_formula;        //The final formula which concatene every graphs formula
    int nb_vertex_positions = pathLength + 1;
    //Loop through each graph
    for (unsigned int graph_number = 0; graph_number < numGraphs; graph_number++)
    {
        // loop every index of pathLength
        Z3_ast index_formula[nb_vertex_positions];
        int index_count = 0;

        int size_graph = orderG(graphs[graph_number]);

        for (int j = 0; j <= pathLength; j++)
        {
            // loop every node in the graph
            int nb_subset = binomialCoeff(size_graph, 2);
            Z3_ast subset_formula[nb_subset];
            int subset_count = 0;
            for (unsigned int node_number_A = 0; node_number_A < size_graph; node_number_A++)
            {

                Z3_ast termA = getNodeVariable(ctx, graph_number, j, pathLength, node_number_A);
                Z3_ast negTermA = Z3_mk_not(ctx, termA);
                // loop every node in the graph
                for (unsigned int node_number_B = node_number_A + 1; node_number_B < size_graph; node_number_B++)
                {
                    Z3_ast termB = getNodeVariable(ctx, graph_number, j, pathLength, node_number_B);
                    Z3_ast negTermB = Z3_mk_not(ctx, termB);
                    Z3_ast node[2] = {negTermA, negTermB};
                    subset_formula[subset_count] = Z3_mk_or(ctx, 2, node);
                    subset_count++;
                }
            }
            index_formula[index_count] = Z3_mk_and(ctx, nb_subset, subset_formula);
            index_count++;
        }

        res_all_graph[graph_number] = Z3_mk_and(ctx, nb_vertex_positions, index_formula);
    }
    res_final_formula = Z3_mk_and(ctx, numGraphs, res_all_graph);
    if (mode_extended_verbose)
        printf("atMostOneVertexAtEachIndex-----> %s\n", Z3_ast_to_string(ctx, res_final_formula));

    return res_final_formula;
}

Z3_ast ExistsPath(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength)
{
    Z3_ast res_all_graph[numGraphs]; //Array that will contains every z3 formula for each graph
    Z3_ast res_final_formula;        //The final formula which concatene every graphs formula
    int nb_vertex_positions = pathLength;

    //Loop through each graph
    for (unsigned int graph_number = 0; graph_number < numGraphs; graph_number++)
    {
        // loop every index of pathLength
        Z3_ast index_formula[nb_vertex_positions];
        int index_count = 0;

        int size_graph = orderG(graphs[graph_number]);

        for (int j = 0; j < pathLength; j++)
        {
            // loop every node in the graph
            int nb_subset = sizeG(graphs[graph_number]);
            Z3_ast subset_formula[nb_subset];
            int subset_count = 0;
            for (unsigned int node_number_A = 0; node_number_A < size_graph; node_number_A++)
            {
                // loop every node in the graph
                for (unsigned int node_number_B = 0; node_number_B < size_graph; node_number_B++)
                {
                    if (isEdge(graphs[graph_number], node_number_A, node_number_B))
                    {
                        Z3_ast termA = getNodeVariable(ctx, graph_number, j, pathLength, node_number_A);
                        Z3_ast termB = getNodeVariable(ctx, graph_number, j + 1, pathLength, node_number_B);
                        Z3_ast node[2] = {termA, termB};
                        subset_formula[subset_count] = Z3_mk_and(ctx, 2, node);
                        subset_count++;
                    }
                }
            }
            index_formula[index_count] = Z3_mk_or(ctx, nb_subset, subset_formula);
            index_count++;
        }

        res_all_graph[graph_number] = Z3_mk_and(ctx, nb_vertex_positions, index_formula);
    }
    res_final_formula = Z3_mk_and(ctx, numGraphs, res_all_graph);
    if (mode_extended_verbose)
        printf("ExistsPath-----> %s\n", Z3_ast_to_string(ctx, res_final_formula));

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
        if (mode_extended_verbose)
        {
            printf("- Graph Number: %d\n", graph_number);
            printf("--- Graph have: %d nodes.\n", graphs[graph_number].numNodes);
        }

        Z3_ast source = NULL; //Node that will contains the source
        Z3_ast target = NULL; //Node that will contains the target
        res_two_node = NULL;  //reinitialize the value of the node

        // Look every node of the graph
        for (unsigned int node_number = 0; node_number < orderG(graphs[graph_number]); node_number++)
        {
            Graph tmp_graph = graphs[graph_number]; //Local copy bcz more useful
            if (mode_extended_verbose)
                printf("--- Node Number: %d\n", node_number);

            if (isSource(tmp_graph, node_number))
            { //If node is the source, then save a sat valuation
                if (mode_extended_verbose)
                    printf("----- Found source. \n");
                source = getNodeVariable(ctx, graph_number, 0, pathLength, node_number);
            }
            if (isTarget(tmp_graph, node_number))
            { //If node is the target, then save a sat valuation
                if (mode_extended_verbose)
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
            if (mode_extended_verbose)
                printf("+ Formula %s created.\n", Z3_ast_to_string(ctx, res_two_node));
            break;
        }

        if (res_two_node == NULL)
        {
            printf("----- Lack of at least the source or the target edge.\n");
            res_two_node = mk_bool_var(ctx, "false");
        }
        res_all_graph[graph_number] = res_two_node;
    }

    //Concatene each formula coming from each graph
    for (int i = 0; i < numGraphs; i++)
    {
        res_final_formula = Z3_mk_and(ctx, numGraphs, res_all_graph);
    }
    if (mode_extended_verbose)
        printf("-----> %s\n", Z3_ast_to_string(ctx, res_final_formula));

    return res_final_formula;
}

void testSubformula(Z3_context ctx, Z3_ast phi1_1, Z3_ast phi1_2, Z3_ast phi1_3, Z3_ast valide_formula, Z3_ast edge_between_nodes)
{
    if (mode_extended_verbose)
    {
        printf("\n\n");
        printf("- Check Phi1.1 Formula\n");
        sat_checker(ctx, phi1_1);
        printf("- Check Phi1.2 Formula\n");
        sat_checker(ctx, phi1_2);
        printf("- Check Phi1.3 Formula\n");
        sat_checker(ctx, phi1_3);
        printf("- Check Path Formula\n");
        sat_checker(ctx, edge_between_nodes);
        printf("- Check Valide Formula\n");
        sat_checker(ctx, valide_formula);
        printf("\n\n- Merge sub-formula\n");
    }
}

int sat_checker(Z3_context ctx, Z3_ast formula)
{

    Z3_lbool isSat = isFormulaSat(ctx, formula);

    switch (isSat)
    {
    case Z3_L_FALSE:
        break;

    case Z3_L_UNDEF:
        break;

    case Z3_L_TRUE:
        return 1;
    }
    return 0;
}
int sat_checker_print(Z3_context ctx, Z3_ast formula)
{

    Z3_lbool isSat = isFormulaSat(ctx, formula);

    switch (isSat)
    {
    case Z3_L_FALSE:
        printf("--- This formula is not sat-solvable.\n", Z3_ast_to_string(ctx, formula));
        break;

    case Z3_L_UNDEF:
        printf("--- We don't know if %s is sat-solvable.\n", Z3_ast_to_string(ctx, formula));
        break;

    case Z3_L_TRUE:
        printf("--- This formula is sat-solvable.\n", Z3_ast_to_string(ctx, formula));
        return 1;
    }
    return 0;
}

int binomialCoeff(int n, int k)
{
    // Base Cases
    if (k == 0 || k == n)
        return 1;

    // Recur
    return binomialCoeff(n - 1, k - 1) + binomialCoeff(n - 1, k);
}
