#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import ctypes

import os
import sys

# print('sys.platform:' + sys.platform)

if(sys.platform ==  'linux'):
    libSvfModule = ctypes.cdll.LoadLibrary(os.getcwd() + '/src/svfModule/libsvfModule.so')
elif(sys.platform == 'darwin'):
    libSvfModule = ctypes.cdll.LoadLibrary(os.getcwd() + '/src/svfModule/libsvfModule.dylib')

class SVFUtil(object):
    global libSvfModule
    svfUtil = libSvfModule
    def processArguments(self, argv):
        select = (ctypes.c_char_p * len(argv))()
        for key, item in enumerate(argv):
            select[key] = item.encode('utf-8')
        self.svfUtil.processArguments(len(argv), select)

class LLVMModuleSet(object) :
    global libSvfModule
    llv = libSvfModule
    def buildSVFModule(self, moduleNameVec):
        for i in moduleNameVec:
            self.llv.setModuleNameVec(ctypes.c_char_p(i.encode('utf-8')))
        self.llv.buildSVFModule()
        return SVFModule
    def dumpModulesToFile(self, s):
        self.llv.dumpModulesToFile(ctypes.c_char_p(s.encode('utf-8')));
    def releaseLLVMModuleSet(self):
        self.llv.releaseLLVMModuleSet();

class cl(object):
    global libSvfModule
    _cl = libSvfModule
    def ParseCommandLineOptions(self):
        self._cl.ParseCommandLineOptions()


class SVFModule(object):
    global libSvfModule
    svfModule = libSvfModule
    def buildSymbolTableInfo(self):
        self.svfModule.buildSymbolTableInfo()

class SVFIRBuilder(object):
    global libSvfModule
    svfIRBuilder = libSvfModule
    def build(self):
        self.svfIRBuilder.build()
        return SVFIR

class SVFIR(object):
    global libSvfModule
    svfIR = libSvfModule
    def getICFG(self):
        self.svfIR.getICFG()
    def releaseSVFIR(self):
        self.svfIR.getICFG()

class AndersenWaveDiff(object):
    global libSvfModule
    andersenWaveDiff = libSvfModule
    def createAndersenWaveDiff(self, pag):
        self.andersenWaveDiff.createAndersenWaveDiff()
        return Andersen
    def releaseAndersenWaveDiff(self):
        self.andersenWaveDiff.releaseAndersenWaveDiff()

class Andersen(object):
    global libSvfModule
    andersen = libSvfModule
    def getPTACallGraph(self):
        self.andersen.getPTACallGraph()
        return PTACallGraph


class PTACallGraph(object):
    pass


class ICFG(object):
    pass

class VFG(object):
    global libSvfModule
    vfg = libSvfModule
    def newInstances(self, callgraph):
        self.vfg.newInstances()

    def delete(self):
        self.vfg.deleteSvfg()

class SVFGBuilder(object):
    global libSvfModule
    svfgBuilder = libSvfModule
    def buildFullSVFG(self, ander):
        self.svfgBuilder.buildFullSVFG()

class SVFG(object):
    global libSvfModule
    svfg = libSvfModule
    def delete(self):
        self.svfg.deleteSvfg()

class llvm(object):
    global libSvfModule
    llvm = libSvfModule
    def llvm_shutdown(self):
        self.llvm.llvm_shutdown()

SVFUtil = SVFUtil()
LLVMModuleSet = LLVMModuleSet()
SVFModule = SVFModule()
cl = cl()
SVFIRBuilder = SVFIRBuilder()
AndersenWaveDiff = AndersenWaveDiff()
Andersen = Andersen()
SVFIR = SVFIR()
ICFG = ICFG()
VFG = VFG()
SVFGBuilder = SVFGBuilder()
llvm = llvm()
SVFG = SVFG()