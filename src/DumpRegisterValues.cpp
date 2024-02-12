#include <stdio.h>
#include <iostream>
#include <string.h>
#include <iomanip>
#include <vector>
#include <fstream>
#include <sstream>
#include <inttypes.h>
#include <boost/shared_ptr.hpp>
#include <cpsw_api_user.h>
#include <stdexcept>
#include <yaml-cpp/yaml.h>
#include <getopt.h>
#include <stdexcept>

// HOW TO RUN:
//
//  <Path-To_CpswUtils>/src/O.rhel6-x86_64/DumpRegisterValues -Y <Yaml-Dir>/000TopLevel.yaml -A <FPGA-IP> -D <Yaml-Dir>
//
// e.g.
// /afs/slac/g/controls/development/users/skoufis/other/CpswUtils/CpswUtils-git/src/O.rhel6-x86_64/DumpRegisterValues -Y /data/
// sioc-b084-ts05/yaml/000TopLevel.yaml -A 10.0.1.105 -D /data/sioc-b084-ts05/yaml

class DumpRegVals : public IPathVisitor
{
     public:
        DumpRegVals(Path root) : r_(root) { r_->explore(this); }

        virtual bool visitPre  (ConstPath p) {return true;}
        virtual void visitPost (ConstPath p) {
            /*
            try 
            {
                if (!p->tail()->isHub())
                {
                    // Create full path for current register (i.e. include "mmio")
                    //std::string mmioPath = "mmio" + p->toString();
                    //printf("Register mmio path is %s\n", mmioPath.c_str());

                    // Try reading integer type first
                    //try
                    //{
                        // Create the interface
                        //ScalVal_RO ifScalVal = IScalVal_RO::create(r->findByName(p->toString().c_str()));
                        //printf("Created Int interface for %s\t0x%x\n", p->toString().c_str(), 111);
                        //ScalVal_RO ifScalVal = IScalVal_RO::create(r->findByName(mmioPath.c_str()));
                        //printf("Created Int interface for %s\t0x%x\n", mmioPath.c_str(), 111);
                        //printf("%s\n", p->toString().c_str());
                        //p->tail()->dump();
                        //printf("\n\n");

                        // Get the number of elements
                        //printf("Number of elements: %d\n", ifScalVal->getNelms());

                        // Read register value(s)
                        //uint64_t readout[ifScalVal->getNelms()];
                        //ifScalVal -> getVal(readout, ifScalVal->getNelms());
                        //uint64_t readout;
                        //ifScalVal -> getVal(&readout, 1, 0);

                        // Print the register path and the corresponding value(s)
                        //printf("%s\t0x%x\n", p->toString().c_str(), readout[0]);
                        //printf("%s\t0x%x\n", p->toString().c_str(), readout);
                    }
                    catch (CPSWError &e)
                    {
                        // Try reading double next
                        try
                        {
                            // Create the interface
                            //DoubleVal_RO ifDoubleVal = IDoubleVal_RO::create(r->findByName(p->toString().c_str()));
                            //printf("Created Double interface for %s\t0x%x\n", p->toString().c_str(), 222);
                            //DoubleVal_RO ifDoubleVal = IDoubleVal_RO::create(r->findByName(mmioPath.c_str()));
                            //printf("Created Double interface for %s\t0x%x\n", mmioPath.c_str(), 222);
                            //p->tail()->dump();
                            //printf("\n\n");
                    
                            // Get the number of elements
                            //printf("Number of elements: %d\n", ifDoubleVal->getNelms());

                            // Read register value(s)
                            //double readout[ifDoubleVal->getNelms()];
                            //ifDoubleVal -> getVal(readout, ifDoubleVal->getNelms());
                            //double readout;
                            //ifDoubleVal -> getVal(&readout, 1, 0);

                            // Print the register path and the corresponding value(s)
                            //printf("%s\t%f\n", p->toString().c_str(), readout[0]);
                            //printf("%s\t%f\n", p->toString().c_str(), readout);
                        }
                        catch (CPSWError &e)
                        {
                            // Try reading command next
                            try
                            {
                                // Create the interface
                                //Command ifCommand = ICommand::create(r->findByName(p->toString().c_str()));
                                //printf("Created Command interface for %s\t0x%x\n", p->toString().c_str(), 333);
                                //Command ifCommand = ICommand::create(r->findByName(mmioPath.c_str()));
                                //printf("Created Command interface for %s\t0x%x\n", mmioPath.c_str(), 333);
                                //p->tail()->dump();
                                //printf("\n\n");

                                // Get the command 
                                //std::string readout;
                                //ifCommand -> getVal(&readout, 1, 0);

                                // Print the register path and the corresponding command 
                                //printf("%s\t%s\n", p->toString().c_str(), readout);
                            }
                            catch (CPSWError &e)
                            {
                                // ANY OTHER TYPE WE MAY ENCOUNTER???
                                //printf("Exception caught! Nothing to do.\n");
                            }
                        }
                    }
                } 
            }
            catch (CPSWError &e)
            {
                //fprintf(stderr,"Test Failed: %s\n", e.what());
                //throw;
                //exit(1);
            }
            */
        };
	      
    private:
        Path r_;
};

// Set IP Class
class IYamlSetIP : public IYamlFixup {
    public:
        IYamlSetIP( const char* ip_addr ) : ip_addr_(ip_addr) {}
        virtual void operator()(YAML::Node &node, YAML::Node &dummy)
        {
            node["ipAddr"] = ip_addr_;
        }

        virtual ~IYamlSetIP() {}
    private:
        std::string ip_addr_;
};

// Usage print message
void usage(char *name)
{
    std::cout << "Dump the register values of a given YAML definition file to the standard output stream." << std::endl;
    std::cout << "Usage: " << name << " -Y yaml_file -A ipAddress [-R root_name] [-D yaml_dir] [-h]" << std::endl;
    std::cout << "    -Y yaml_file : Path to the top level YAML file" << std::endl;
    std::cout << "    -R root_name : Dump the map for root with name \"root_name\". Default is \"mmio\"" << std::endl;
    std::cout << "    -D yaml_dir  : Directory where all the yaml files reside. Default is same directory as \"yaml_file\"" << std::endl;
    std::cout << "    -A ipAddress : FPGA IP" << std::endl;
    std::cout << "    -h           : Print this message" << std::endl;
    std::cout << std::endl;
}

// Main function
int main(int argc, char **argv)
{
    int         c;
    std::string yamlTop;
    std::string yamlDir;
    std::string ipAddress;
    std::string rootName = "mmio";
    std::string rootType = "NetIODev";
    
    // Get input arguments
    while((c =  getopt(argc, argv, "Y:D:R:A:h")) != -1)
    {
        switch (c)
        {
            case 'Y':
                yamlTop = std::string(optarg);
                break;
            case 'R':
                rootName = std::string(optarg);
                break;
            case 'D':
                yamlDir = std::string(optarg);
                break;
            case 'A':
                ipAddress = std::string(optarg);
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

    if (yamlTop.empty())
    {
        std::cout << "Must specify Yaml top file and directory paths." << std::endl;
        usage(argv[0]);
        exit(1);
    }
   
    // Load the yaml hierarchy from the YAML files
    Path root;
  
    try 
    {
        std::cout << "Loading YAML hierarchy" << std::endl;
        if(ipAddress.empty())
            root = IPath::loadYamlFile( yamlTop.c_str(), rootType.c_str(), yamlDir.c_str());
        else
        {
            if (rootType == "NetIODev") // root type is NetIODev
            {
                std::cout << "Setting the IP" << std::endl;
                IYamlSetIP setIP(ipAddress.c_str());
                root = IPath::loadYamlFile( yamlTop.c_str(), rootType.c_str(), yamlDir.c_str(), &setIP );
                std::cout << "Done loading the YAML file" << std::endl;
            }
            else if (rootType == "MemDev")
            {
            }
            else
            {
                std::cerr << "The root type should be NetIODev or MemDev" << std::endl;
                return -1;
            }
        }
    } 
    catch (CPSWError &e) 
    {
        std::cerr << "Unable to load YAML file: " << e.getInfo() << std::endl;
        return -1;
    }

    DumpRegVals* regVals = new DumpRegVals(root);

    const char *yamlFile = "DumpRegVals.yaml";
    YAML::Node config;  YAML::Emitter emit;
    std::fstream strm(yamlFile, std::fstream::out);

    root -> dumpConfigToYaml(config);
    emit << config;
    strm << emit.c_str() << "\n";

    std::cout << "Done Dumping (RW) Register Values" << std::endl;
    
    return 0;
}
