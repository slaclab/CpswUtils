#include <stdio.h>
#include <iostream>
#include <string.h>
#include <iomanip>
#include <vector>
#include <inttypes.h>
#include <boost/shared_ptr.hpp>
#include <cpsw_api_user.h>
#include <stdexcept>
#include <yaml-cpp/yaml.h>
#include <getopt.h>
#include <stdexcept>

void dumpRegisterMap(const Path& p)
{

    Path p_aux = p->clone();
    Child c_aux = p_aux->tail();
    Hub h_aux;

    // Check if the child is null which mean where are the origin
    if (c_aux)
        h_aux = c_aux->isHub();
    else
        h_aux = p_aux->origin();


    Children    c = h_aux->getChildren();
    int         n = c->size();

    // Recursively look for hubs.
    for (int i = 0 ; i < n ; i++)
    {
        Hub h2 = (*c)[i]->isHub();
        if (h2)
        {
            Path p2 = p_aux->findByName((*c)[i]->getName());
            dumpRegisterMap(p2);
        }
        else
        {
            // When a leave is found, write it path and name to the output file
            std::cout << p_aux->toString() << '/' << (*c)[i]->getName() << std::endl;
        }
    }

    return;
}

void usage(char *name)
{
    std::cout << "Dump the register map of a given YAML definition file to the standard output stream." << std::endl;
    std::cout << "Usge: " << name << " -Y yaml_file [-R root_name] [-D yaml_dir] [-h]" << std::endl;
    std::cout << "    -Y yaml_file : Path to the top level YAML file" << std::endl;
    std::cout << "    -R root_name : Dump the map from the node with name \"root_name\". Default is \"mmio\"" << std::endl;
    std::cout << "    -D yaml_dir  : Directory where all the yaml file reside. Default is the same directory are \"yaml_file\"" << std::endl;
    std::cout << "    -h           : Print this message" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char **argv)
{
    std::string yamlDoc; 
    std::string yamlDir;
    std::string rootName = "mmio";
    int c;

    while((c =  getopt(argc, argv, "Y:D:R:h")) != -1)
    {
        switch (c)
        {
            case 'Y':
                yamlDoc = std::string(optarg);
                break;
            case 'D':
                yamlDir = std::string(optarg);
                break;
            case 'R':
                rootName = std::string(optarg);
                break;
            case 'h':
                usage(argv[0]);
                exit(0);
                break;
            default:
                std::cout << "Invalid option." << std::endl;
                usage(argv[0]);
                exit(1);
                break;
        }
    }

    if (yamlDoc.empty())
    {
        std::cout << "Must specify a Yaml top path." << std::endl;
        usage(argv[0]);
        exit(1);
    }


    Path root;
    if (yamlDir.empty())
        root = IPath::loadYamlFile( yamlDoc.c_str(), rootName.c_str(), NULL );  
    else
        root = IPath::loadYamlFile( yamlDoc.c_str(), rootName.c_str(), yamlDir.c_str() );  

    dumpRegisterMap(root);

    return 0;
}
