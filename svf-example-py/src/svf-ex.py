#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import re
import sys
# import os
# sys.path.append(os.getcwd() + '/py')

from svfLLVMUtil import LLVMUtil
from svfModule import SVFG, cl
from svfModule import LLVMModuleSet
from svfModule import SVFIRBuilder
from svfModule import AndersenWaveDiff
from svfModule import SVFIR, VFG, SVFGBuilder

def main(argv):
    moduleNameVec = []
    
    LLVMUtil.processArguments(argv, moduleNameVec)
    # cl.ParseCommandLineOptions()
    
    svfModule = LLVMModuleSet.buildSVFModule(moduleNameVec)
    
    svfModule.buildSymbolTableInfo()
    
    pag = SVFIRBuilder.build()
    
    ander = AndersenWaveDiff.createAndersenWaveDiff(pag)
    callgraph = ander.getPTACallGraph()
    pag.getICFG()

    VFG.newInstances(callgraph)

    SVFGBuilder.buildFullSVFG(ander)

    VFG.delete()
    SVFG.delete()
    AndersenWaveDiff.releaseAndersenWaveDiff()
    SVFIR.releaseSVFIR()
    LLVMModuleSet.dumpModulesToFile('.svf.bc')
    LLVMModuleSet.releaseLLVMModuleSet()



if __name__ == "__main__":
    # execute only if run as a script
    argv = sys.argv
    main(argv)
