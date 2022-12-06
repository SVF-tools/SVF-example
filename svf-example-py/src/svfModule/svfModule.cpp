#include "SVF-FE/LLVMUtil.h"
#include "Graphs/SVFG.h"
#include "WPA/Andersen.h"
#include "SVF-FE/SVFIRBuilder.h"
#include "Util/Options.h"

using namespace llvm;
using namespace std;
using namespace SVF;

extern "C" {
    char **arg_value = NULL;
    int arg_num = 0;
    std::vector<std::string> moduleNameVec;
    SVFModule* svfModule;
    SVFIRBuilder *builder;
    SVFIR* pag;
    Andersen* ander;
    PTACallGraph* callgraph;
    ICFG* icfg;
    VFG* vfg;
    SVFGBuilder* svfBuilder;
    SVFG* svfg;

    void ParseCommandLineOptions();
    void buildSVFModule();
    void setModuleNameVec(char* name);
    void buildSymbolTableInfo();
    void build();
    void createAndersenWaveDiff();
    void getPTACallGraph();
    void getICFG();
    void newInstances();
    void buildFullSVFG();
    void deleteSvfg();
    void deleteVfg();
    void releaseAndersenWaveDiff();
    void releaseSVFIR();
    void dumpModulesToFile(const char *s);
    void releaseLLVMModuleSet();
    void llvm_shutdown();
}

void ParseCommandLineOptions(){
    printf("arg_num: %d\n", arg_num);
    for(int i = 0; i < arg_num; i++){
        printf("arg_value[%d]:%s\n", i, arg_value[i]);
    }
    cl::ParseCommandLineOptions(arg_num, arg_value, "Whole Program Points-to Analysis\n");
}

void buildSVFModule(){
    svfModule = LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);
}

void setModuleNameVec(char* name){
    if(name != nullptr){
        std::string s(name);
        // printf("name:%s\n", name);
        moduleNameVec.push_back(s);
    }
}

void buildSymbolTableInfo(){
    svfModule->buildSymbolTableInfo();
}

void build(){
    SVFIRBuilder _builder;
    pag = _builder.build(svfModule);
}

void createAndersenWaveDiff(){
    ander = AndersenWaveDiff::createAndersenWaveDiff(pag);
}

void getPTACallGraph(){
    callgraph = ander->getPTACallGraph();
}

void getICFG(){
    icfg = pag->getICFG();
}

void newInstances(){
    vfg = new VFG(callgraph);
}

void buildFullSVFG(){
    SVFGBuilder _svfBuilder;
    _svfBuilder.buildFullSVFG(ander);
}

void deleteVfg(){
    delete vfg;
}
void deleteSvfg(){
    delete svfg;
}

void releaseAndersenWaveDiff(){
    AndersenWaveDiff::releaseAndersenWaveDiff();
}

void releaseSVFIR(){
    SVFIR::releaseSVFIR();
}

void dumpModulesToFile(const char *s){
    LLVMModuleSet::getLLVMModuleSet()->dumpModulesToFile(s);
}
void releaseLLVMModuleSet(){
    SVF::LLVMModuleSet::releaseLLVMModuleSet();
}

void llvm_shutdown(){
    llvm::llvm_shutdown();
}
