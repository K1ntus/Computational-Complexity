#include <stdlib.h>
#include <string.h>
//MKDIR
#include <sys/stat.h>
#include <sys/types.h>

#include "Z3Tools.h"
#include <Graph.h>
#include <Parsing.h>
#include "Solving.h"

Graph loadGraph(char *argv);
void printHelp(void);



#define MAX_NUM_ARGS 8
bool mode_verbose = false;
bool mode_extended_verbose = false;
bool mode_display_formula = false;
bool mode_paths_found = false;

bool mode_first_depth_sat = false;
bool mode_explore_decreasing_order = false;
bool mode_every_solutions = false;

bool mode_save_dot_file = false;
bool mode_custom_namefile = false;
char * address_to_filename = NULL;

int defineUserMode(int argc, char* argv[],char*val);


int main(int argc, char *argv[])
{
    char * fileName_buffer = (char*) malloc(sizeof(char)*1024);
    int begin_args_graph = defineUserMode(argc, argv, fileName_buffer);
    address_to_filename = fileName_buffer;

    Graph *graphList = (Graph *)malloc(sizeof(Graph) * (argc - 1));
    int nbGraph = 0;
    for (; nbGraph + begin_args_graph < argc; nbGraph++)
    {
        Graph tmp = loadGraph(argv[begin_args_graph + nbGraph]);
        graphList[nbGraph] = tmp;
    }

   

    Z3_context ctx = makeContext();

    Z3_ast fullFormula = graphsToFullFormula(ctx, graphList, nbGraph);

    if (fullFormula)
    {
        Z3_model model = getModelFromSatFormula(ctx, fullFormula);
        int pathLength = getSolutionLengthFromModel(ctx, model, graphList);
        if (mode_display_formula)
        {
            printf("Full Formula : %s\n", Z3_ast_to_string(ctx, fullFormula));
        }
        

        // if(mode_save_dot_file){
        //     createDotFromModel(ctx, model, graphList, nbGraph, pathLength, fileName_buffer);
        // }
    }


    Z3_del_context(ctx);
    printf("\nContext deleted, memory is now clean.\n");

    for (int i = 0; i < nbGraph; i++)
    {
        printf("Deleting Graph: %d.\n", i);
        deleteGraph(graphList[i]);
    }
    free(graphList);
    printf("Graphes successfully deleted.\n");

    return 0;
}

Graph loadGraph(char *argv)
{
    if (argv == 0x0)
    {
        printf("--- Graph: NULL\n", argv);
        Graph null_graph;
        return null_graph;
    }

    Graph graph = getGraphFromFile(argv);
    if (mode_verbose)
    {
        printf("--- Graph: %s\n", argv);

        printGraph(graph);

        if (mode_extended_verbose)
        {
            printf("detailed informations:\n");

            printf("- There are %d vertices.\n", orderG(graph));
            printf("- There are %d edges.\n", sizeG(graph));

            printf("\n Note: all graphs provided will have a single source and single target.\n");
            int node;
            for (node = 0; node < orderG(graph) && !isSource(graph, node); node++)
                ;
            printf("----- The source is %s. At ID: %d.\n", getNodeName(graph, node), node);

            for (node = 0; node < orderG(graph) && !isTarget(graph, node); node++)
                ;
            printf("----- The target is %s. At ID: %d\n", getNodeName(graph, node), node);

            if (isEdge(graph, 0, 1))
                printf(" There is an edge between %s and %s.\n", getNodeName(graph, 0), getNodeName(graph, 1));
            else
                printf("\n There is no edge between %s and %s.\n", getNodeName(graph, 0), getNodeName(graph, 1));
        }
    }

    return graph;
}

void printHelp()
{
    //  NOTE -- flags are ignored until the relevant assignment.
    //  Some of the flags are interpreted here; some in system.cc.
    //
    printf(
        "Usage:\n"
        "./equalPath -v/V <verbose flag> -F <full formula>\n"
        "       -t <display paths found> -f <output file>\n"
        "       -o <NAME.dot> <GRAPHS.dot TO TEST>\n"
        "Example:\n"
        "./equalPath -v -F -t graphs/assignment-instance/triangle.dot graphs/assignment-instance/G1.dot \n"
        "Options:\n"
        "* DEBUG:\n"
        "   -h  Displays this help\n"
        "   -v  Activate the Verbose Mode (displays parsed graphes)\n"
        "   -V  Activate the Extended Verbose Mode (More for debugging)\n"
        "   -F  Displays the formula computed\n"
        "   -t  Displays the paths found on the terminal [if not present, only displays the existence of the path].\n"
        "   -s  Test every formula by depth.\n"
        "   -d  [If -s present] Explore the length by decreasing order.\n"
        "   -a  [If -s present] Compute every length instead of the first found.\n"
        "\n"
        "* FILE:\n"
        "   -f Writes the result with colors in a .dot file. See next option for the name. These files will be produced in the folder 'output'.\n"
        "   -o Writes the output in \"NAME-lLENGTH.dot\" where LENGTH is the length of the solution. Writes several files in this format if both -s and -a are present. [if not present: \"result-lLENGTH.dot\"]\n"
    );
}

int defineUserMode(int argc, char * argv[], char* fileName_buffer){

    int begin_args_graph = 1;
    if (argc < 2)
    {
        return EXIT_FAILURE;
    }

    if (argc > 1 && !strcmp(argv[1], "-h"))
    { // print help
        printHelp();
        exit(EXIT_SUCCESS);
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
            mkdir("output", 0755);
        }

        if ((argc > begin_args_graph && !strcmp (argv[begin_args_graph], "-a"))) { // activate verbose
            mode_every_solutions = true;
            begin_args_graph += 1;
        }

        if ((argc > begin_args_graph && !strcmp (argv[begin_args_graph], "-s"))) { // activate verbose
            mode_first_depth_sat = true;
            begin_args_graph += 1;
        }

        if ((argc > begin_args_graph && !strcmp (argv[begin_args_graph], "-d"))) { // activate verbose
            mode_explore_decreasing_order = true;
            begin_args_graph += 1;
        }

        if ((argc > begin_args_graph +1 && !strcmp (argv[begin_args_graph], "-o"))) { // activate verbose
            mode_custom_namefile = true;
            strcpy(fileName_buffer, argv[begin_args_graph+1]);
            begin_args_graph += 2;
        }



    }

    return begin_args_graph;
}



