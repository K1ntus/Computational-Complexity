#include <stdlib.h>

#include "Z3Tools.h"
#include <Graph.h>
#include <Parsing.h>
#include "Solving.h"


Graph loadGraph (char* argv);

int main ( int argc, char* argv[] ) {
    if(argc < 2) {
        return EXIT_FAILURE;
    }
    
    /**
     * 
     * 
     * 
     * 
     */
    Graph * graphList =  (Graph *) malloc(sizeof(Graph) * (argc-1));
    int nbGraph = 0;
        printf("\n\n*******************\n* [INFO] Graph Loading ...\n*******************\n\n");
        for(; nbGraph < argc-1; nbGraph++){
            printf("- Loading Graph: %d\n", nbGraph);
            Graph tmp = loadGraph(argv[nbGraph+1]);
            graphList[nbGraph] = tmp;
        }

        printf("\n\n*******************\n* [INFO] %d graph loaded.\n*******************\n\n", nbGraph);


    Z3_context ctx = makeContext();
    printf(" * [INFO] Creating the context. Must be destroyed at the end of the program.\n");



    /**
     * 
     * 
     * 
     * 
     */
        printf("\n\n*******************\n* [INFO] Sat Generation ...\n*******************\n\n");

        int pathLength = 3; //Make variable
        Z3_ast res = graphsToPathFormula(ctx, graphList, nbGraph, pathLength);



        
        printf("\n\n*******************\n* [INFO]  sat generated.\n*******************\n\n");

    // Z3_ast x = mk_bool_var(ctx,"x");
    // printf("Variable %s created.\n",Z3_ast_to_string(ctx,x));

    // Z3_ast y = mk_bool_var(ctx,"y");
    // printf("Variable %s created.\n",Z3_ast_to_string(ctx,y));

    // Z3_ast negX = Z3_mk_not(ctx,x);
    // printf("Formula %s created.\n",Z3_ast_to_string(ctx,negX));

    // Z3_ast absurdTab[3] = {x,y,negX};
    // Z3_ast absurd = Z3_mk_and(ctx,3,absurdTab);
    // printf("Formula %s created.\n",Z3_ast_to_string(ctx,absurd));

    // Z3_ast anOtherTab[3] = {negX,y,absurd};
    // Z3_ast easy = Z3_mk_or(ctx,3,anOtherTab);
    // printf("We have now: %s\n\n",Z3_ast_to_string(ctx,easy));

    // Z3_lbool isSat = isFormulaSat(ctx,absurd);

    // switch (isSat) {
    //     case Z3_L_FALSE:
    //         printf("%s is not satisfiable.\n",Z3_ast_to_string(ctx,absurd));
    //         break;

    //     case Z3_L_UNDEF:
    //         printf("We don't know if %s is satisfiable.\n",Z3_ast_to_string(ctx,absurd));
    //         break;

    //     case Z3_L_TRUE:
    //         printf("%s is satisfiable.\n",Z3_ast_to_string(ctx,absurd));
    //         Z3_model model = getModelFromSatFormula(ctx,absurd);
    //         printf("Model obtained for %s:\n",Z3_ast_to_string(ctx,absurd));
    //         printf("    The value of %s is %d\n",Z3_ast_to_string(ctx,x),valueOfVarInModel(ctx,model,x));
    //         printf("    The value of %s is %d\n",Z3_ast_to_string(ctx,y),valueOfVarInModel(ctx,model,y));
    //         printf("    The value of %s is %d\n",Z3_ast_to_string(ctx,negX),valueOfVarInModel(ctx,model,negX));
    //         break;
    // }

    // isSat = isFormulaSat(ctx,easy);
    // printf("\n");

    // switch (isSat) {
    //     case Z3_L_FALSE:
    //         printf("%s is not satisfiable.\n",Z3_ast_to_string(ctx,easy));
    //         break;

    //     case Z3_L_UNDEF:
    //         printf("We don't know if %s is satisfiable.\n",Z3_ast_to_string(ctx,easy));
    //         break;

    //     case Z3_L_TRUE:
    //         printf("%s is satisfiable.\n",Z3_ast_to_string(ctx,easy));
    //         Z3_model model = getModelFromSatFormula(ctx,easy);
    //         printf("Model obtained for %s:\n",Z3_ast_to_string(ctx,easy));
    //         printf("    The value of %s is %d\n",Z3_ast_to_string(ctx,x),valueOfVarInModel(ctx,model,x));
    //         printf("    The value of %s is %d\n",Z3_ast_to_string(ctx,y),valueOfVarInModel(ctx,model,y));
    //         printf("    The value of %s is %d\n",Z3_ast_to_string(ctx,negX),valueOfVarInModel(ctx,model,negX));
    //         break;
    // }

    Z3_del_context(ctx);
    printf("Context deleted, memory is now clean.\n");
 



    for(int i = 0; i < nbGraph; i++){
        printf("Deleting Graph: %d.\n", i);
        // deleteGraph(graphList[i]);
    }
    // free(graphList);
    printf("Graphes successfully deleted.\n");



    return 0;
}


Graph loadGraph (char* argv){
    if(argv == 0x0){
        printf("--- Graph: NULL\n", argv);
        Graph null_graph;
        return null_graph;
    }

    printf("--- Graph: %s\n", argv);

    Graph graph = getGraphFromFile(argv);

    // printGraph(graph);

    // printf("detailed informations:\n");

    // printf(" There are %d vertices.\n",orderG(graph));
    // printf(" There are %d edges.\n",sizeG(graph));

    // printf("\n Note: all graphs provided will have a single source and single target.\n");
    int node;
    for(node=0;node<orderG(graph) && !isSource(graph,node);node++);
    printf("----- The source is %s.\n",getNodeName(graph,node));

    for(node=0;node<orderG(graph) && !isTarget(graph,node);node++);
    printf("----- The target is %s.\n",getNodeName(graph,node));

    // if(isEdge(graph,0,1)) printf(" There is an edge between %s and %s.\n",getNodeName(graph,0),getNodeName(graph,1));
    // else printf("\n There is no edge between %s and %s.\n",getNodeName(graph,0),getNodeName(graph,1));

}