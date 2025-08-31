#include "linker.hpp"

static Linker* linker;

int main(int argc, char* argv[]) {

    std::vector<std::string> inputFiles = {};
    bool relocatable = false;
    bool hex = false;
    std::string outputFile = std::string("izlazniFajlLinker.o");
    
    std::unordered_map<std::string, uint32> sectionAddress = {};
    uint32 maxSectionAddress = 0;
    std::string maxSectionName = "";

    for(int i = 1; i < argc; ++i) {
        if(std::string(argv[i]) == "-relocatable") { 
            if(hex) { 
                std::cerr << "Error: Cannot use -relocatable and -hex options together\n";
                return -1;
            }
            relocatable = true;
        }
        else if(std::string(argv[i]) == "-hex") { 
            if(relocatable) {
                std::cerr << "Error: Cannot use -relocatable and -hex options together\n";
                return -1; 
            }
            hex = true;
        }
        else if(std::string(argv[i]) == "-o") {
            if(i+1 == argc) {
                std::cerr << "Error: No output file specified\n";
                return -2;
            }
            else outputFile = std::string(argv[++i]); 
        }
        else if(std::string(argv[i]).at(0) == '-') {
            if(relocatable) { 
                std::cerr << "Error: Cannot use -place option with -relocatable\n";
                return -3;
            }
            std::string option = std::string(argv[i]);
            std::string prefix = "-place=";
            option.erase(0, prefix.size());

            size_t atPos = option.find("@");
            if(atPos == std::string::npos)  {
                std::cerr << "Error: Invalid format of -place option\n";
                return -4;
            }

            std::string section = option.substr(0, atPos);
            std::string addr = option.substr(atPos+1);

            uint32 address = std::stoul(addr, nullptr, 0);
            if(sectionAddress.find(section) != sectionAddress.end()) {
                std::cerr << "Error: section " << section << " is already placed\n";
                return -5;
            }
            sectionAddress[section] = address;
            if(address > maxSectionAddress) {
                maxSectionAddress = address;
                maxSectionName = section;
            }
        } else { // input files
            inputFiles.push_back(std::string(argv[i]));
        }
    }
    
    if(!hex && !relocatable) {
        std::cerr << "Error: No output format specified, use -relocatable or -hex\n";
        return -6;
    }
    if(inputFiles.empty()) {
        std::cerr << "Error: No input files specified\n";
        return -7;
    }

    linker = new Linker(inputFiles, outputFile, relocatable, sectionAddress, maxSectionAddress, maxSectionName);
    
    int ret = linker -> linkAndCreateOutput();

    return ret;
}