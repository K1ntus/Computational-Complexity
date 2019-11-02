#include <stdlib.h>
#include <string.h>
//MKDIR
#include <sys/stat.h>
#include <sys/types.h>

#include "Z3Tools.h"
#include <Graph.h>
#include <Parsing.h>
#include "Solving.h"


Graph loadGraph (char* argv);
void printHelp(void);



#define MAX_NUM_ARGS 7
bool mode_verbose = false;
bool mode_extended_verbose = false;
bool mode_display_formula = false;
bool mode_paths_found = false;
bool mode_save_dot_file = false;

int main ( int argc, char* argv[] ) {
    int begin_args_graph = 1;
    if(argc < 2) {
        return EXIT_FAILURE;
    }


    if (argc > 1 && !strcmp(argv[1], "-h")) { // print help 
        printHelp();
        return EXIT_SUCCESS;
    }

    for(int i = 0; i < MAX_NUM_ARGS; i++) {
        if (argc > begin_args_graph && !strcmp (argv[begin_args_graph], "-v")) { // activate verbose
            mode_verbose = true;
            begin_args_graph += 1;
        }

        if (argc > begin_args_graph && !strcmp (argv[begin_args_graph], "-V")) { // activate verbose
            mode_extended_verbose = true;
            mode_verbose = true;
            mode_display_formula = true;
            mode_paths_found = true;
            begin_args_graph += 1;
        }

        if ((argc > begin_args_graph && !strcmp (argv[begin_args_graph], "-F"))) { // activate verbose
            mode_display_formula = true;
            begin_args_graph += 1;
        }

        if ((argc > begin_args_graph && !strcmp (argv[begin_args_graph], "-t"))) { // activate verbose
            mode_paths_found = true;
            begin_args_graph += 1;
        }

        if ((argc > begin_args_graph && !strcmp (argv[begin_args_graph], "-f"))) { // activate verbose
            mode_save_dot_file = true;
            begin_args_graph += 1;
        }


    }
    
    
    /**
     * 
     * 
     * 
     * 
     */
    Graph * graphList =  (Graph *) malloc(sizeof(Graph) * (argc-1));
        printf("\n\n*******************\n* [INFO] Graph Loading ...\n*******************\n\n");
        int nbGraph = 0;
        for(; nbGraph+begin_args_graph < argc; nbGraph++){
            printf("- Loading Graph: %d\n", nbGraph);
            Graph tmp = loadGraph(argv[begin_args_graph+nbGraph]);
            graphList[nbGraph] = tmp;
        }

        printf("\n\n*******************\n* [INFO] %d graph loaded.\n*******************\n\n", nbGraph);


    Z3_context ctx = makeContext();
    printf("* [INFO] Creating the context. Must be destroyed at the end of the program.\n");



    /**
     * 
     * 
     * 
     * 
     */
    printf("\n\n*******************\n* [INFO] Sat Generation ...\n*******************");

    int pathLength = 2; //Make variable
    Z3_ast res = graphsToPathFormula(ctx, graphList, nbGraph, pathLength);
 
    if(res!=NULL){
        if(mode_display_formula){
            printf("graphsToPathFormula-----> %s\n", Z3_ast_to_string(ctx, res));
        }
        printf("\n\n*******************\n* [INFO] Sat Generated.\n*******************\n\n");

        Z3_model model = getModelFromSatFormula(ctx, res);

        if(mode_paths_found)
            printPathsFromModel(ctx, model, graphList, nbGraph, pathLength);

        if(mode_save_dot_file){
            mkdir("output", 0755);
            createDotFromModel(ctx, model, graphList, nbGraph, pathLength, NULL);
        }
    }
    

    Z3_del_context(ctx);
    printf("\n\nContext deleted, memory is now clean.\n");
 



    for(int i = 0; i < nbGraph; i++){
        printf("Deleting Graph: %d.\n", i);
        deleteGraph(graphList[i]);
    }
    free(graphList);
    printf("Graphes successfully deleted.\n");



    return 0;
}


Graph loadGraph (char* argv){
    if(argv == 0x0){
        printf("--- Graph: NULL\n", argv);
        Graph null_graph;
        return null_graph;
    }
    

    Graph graph = getGraphFromFile(argv);
    if(mode_verbose){
        printf("--- Graph: %s\n", argv);

        printGraph(graph);

        if(mode_extended_verbose) {
            printf("detailed informations:\n");

            printf("- There are %d vertices.\n",orderG(graph));
            printf("- There are %d edges.\n",sizeG(graph));

            printf("\n Note: all graphs provided will have a single source and single target.\n");
            int node;
            for(node=0;node<orderG(graph) && !isSource(graph,node);node++);
            printf("----- The source is %s. At ID: %d.\n",getNodeName(graph,node), node);

            for(node=0;node<orderG(graph) && !isTarget(graph,node);node++);
            printf("----- The target is %s. At ID: %d\n",getNodeName(graph,node), node);

            if(isEdge(graph,0,1)) printf(" There is an edge between %s and %s.\n",getNodeName(graph,0),getNodeName(graph,1));
            else printf("\n There is no edge between %s and %s.\n",getNodeName(graph,0),getNodeName(graph,1));
        }
    }

    return graph;

}


void printHelp() {
    //  NOTE -- flags are ignored until the relevant assignment.
//  Some of the flags are interpreted here; some in system.cc.
//
	printf (
        "Usage:\n"
        "./equalPath -v/V <verbose flag> -F <full formula>\n"
        "       -t <display paths found> -f <output file>\n"
        "       -o <NAME.dot> <GRAPHS.dot TO TEST>\n"
        "Example:\n"
        "./equalPath -v -F -t graphs/assignment-instance/triangle.dot graphs/assignment-instance/G1.dot \n"
        "Options:\n"
        "* DEBUG:\n"
        "   -h Displays this help\n"
        "   -v Activate the Verbose Mode (displays parsed graphes)\n"
        "   -V Activate the Extended Verbose Mode (More for debugging)\n"
        "   -F Displays the formula computed\n"
        "   -t Displays the paths found on the terminal [if not present, only displays the existence of the path].\n"
        "\n"
        "* FILE:\n"
        "   -f Writes the result with colors in a .dot file. See next option for the name. These files will be produced in the folder 'sol'.\n"
        "   TODO: -o Writes the output in \"NAME-lLENGTH.dot\" where LENGTH is the length of the solution. Writes several files in this format if both -s and -a are present. [if not present: \"result-lLENGTH.dot\"]\n"
    );
}
