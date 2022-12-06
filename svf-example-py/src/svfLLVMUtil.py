#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import ctypes

import os
import sys

if(sys.platform ==  'linux'):
    libsvfLLVMUtil = ctypes.cdll.LoadLibrary(os.getcwd() + '/src/svfLLVMUtil/libsvfLLVMUtil.so')
elif(sys.platform == 'darwin'):
    libsvfLLVMUtil = ctypes.cdll.LoadLibrary(os.getcwd() + '/src/svfLLVMUtil/libsvfLLVMUtil.dylib')


class LLVMUtil(object):
    global libsvfLLVMUtil
    LLVMUtil = libsvfLLVMUtil
    def processArguments(self, argv, moduleNameVec):
        select = (ctypes.c_char_p * len(argv))()
        for key, item in enumerate(argv):
            select[key] = item.encode('utf-8')
        self.LLVMUtil.processArguments(ctypes.c_int(len(argv)), select)
        num =  self.LLVMUtil.getModuleNameVecLen()
        # print('num = ' + str(num))
        i = 0
        while i < num:
            result = ctypes.create_string_buffer(100)
            self.LLVMUtil.getModuleNameVecItem(ctypes.c_int(i), result)
            # str_p = self.LLVMUtil.getModuleNameVecItem(ctypes.c_int(i)) 
            # print('s: '  + result.value.decode('utf-8'))
            moduleNameVec.append(result.value.decode('utf-8'))
            i = i + 1
        # print(moduleNameVec)


LLVMUtil = LLVMUtil()