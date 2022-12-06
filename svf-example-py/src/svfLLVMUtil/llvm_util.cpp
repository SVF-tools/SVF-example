#include "SVF-FE/LLVMUtil.h"

using namespace SVF;

extern "C" {
    char **arg_value = NULL;
    int arg_num = 0;
    std::vector<std::string> moduleNameVec;
    int moduleNameVecLen = 0;
    void processArguments(int argc, char **argv);
    int getModuleNameVecLen();
    const char* getModuleNameVecItem(int index, char* result);
}


void processArguments(int argc, char **_argv){
    // TODD: delete free
    arg_value = new char*[argc];
    LLVMUtil::processArguments(argc, _argv, arg_num, arg_value, moduleNameVec);

}

int getModuleNameVecLen(){
    return moduleNameVec.size();
}

const char* getModuleNameVecItem(int index, char* result){
    // printf("moduleNameVec.size():%ld, index: %d\n", moduleNameVec.size(), index);
    if(index < moduleNameVec.size()){
        // printf("moduleNameVec[index]:%s\n", moduleNameVec[index].c_str());
        snprintf(result, strlen(moduleNameVec[index].c_str()) + 1, "%s", moduleNameVec[index].c_str());
    }
    return result;
}


