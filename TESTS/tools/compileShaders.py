# git > Open in Command Prompt
# cd tests/tools
# py compileShaders.py

           
'''
┌───────────────────────────────────────────┐ 
│compileShaders.py                          │
│Author: Tripp R.                           │
│-------------------------------------------│
│A simple python script to automate         │
│the complilation of shaders.               │ 
│The compiled shaders are output to         │
│tests/compiled-shaders.                    │
│The compiled shader will be named as such: │
│<original-name><originalFileExtension>.spv │
└───────────────────────────────────────────┘ 
'''


from ast import main
from calendar import c
from functools import cache
from operator import index
from sqlite3 import Timestamp
import subprocess
import os

import time
from datetime import UTC, datetime, timezone
from traceback import print_tb
from turtle import update
from xmlrpc.client import getparser

CACHE_FILE_NAME = "SHADER_COMPILE_CACHE.txt"
CACHE_FILE_ELEMENT_SEPERATOR = ','

SHADER_SOURCE_DIR = str(os.path.abspath("..\\shaders")) + "\\"
COMPILE_TO_DIR = str(os.path.abspath("..\\shaders-compiled")) + "\\"

''' 
The Cache File is organized like so:

shader.frag, 2025-02-11 20:57:50.104567+00:00
'''


def parseCacheFileLine(line):
    parsed = line.split(CACHE_FILE_ELEMENT_SEPERATOR)
    name = parsed[0]
    date = parsed[1]
    name = name.replace("\n", "")
    date = date.replace("\n", "")
    return [name, date]

def isShaderInCacheFile(shaderName):
    cacheFile = open(SHADER_SOURCE_DIR+shaderName)
    

def getShadersToCompile():
    # loads cache data
    file = open(CACHE_FILE_NAME,"r")
    cacheData = {}
    for line in file:
        [name, date] = parseCacheFileLine(line)
        cacheData[name] = date
    file.close()
    #############################
    
    shadersToCompile=[]
    utc_now = datetime.now(timezone.utc)

    files = os.listdir(SHADER_SOURCE_DIR)
    # Print the files
    for file in files:
        # if the file is not cached, it means that it hasn't been compiled
        if (not (file in cacheData)):
            shadersToCompile.append(file)
            continue
        timestampModified = datetime.fromtimestamp(os.path.getmtime(SHADER_SOURCE_DIR+file), tz=timezone.utc)
        timestampFromCache = datetime.fromisoformat(cacheData[file])
        
        # the cached timestamp is outdated, update the shader
        if (timestampModified > timestampFromCache):
            print(timestampModified)
            print(timestampFromCache)
            shadersToCompile.append(file)
            continue

    return shadersToCompile

def updateCache(shadersCompiled):
    # loads cache data
    file = open(CACHE_FILE_NAME,"r")
    cacheData = {}
    for line in file:
        [name, date] = parseCacheFileLine(line)
        cacheData[name] = date
    file.close()
    #############################

    utc_now = datetime.now(timezone.utc)

    for shader in shadersCompiled:
        cacheData[shader] = utc_now;
    #############################

    file = open(CACHE_FILE_NAME,'w')

    # writes cache data
    for elem in cacheData:
        line = elem+CACHE_FILE_ELEMENT_SEPERATOR+str(cacheData[elem])
        file.write(line+"\n")

    file.close()
        

def compileShaders(shadersToCompile):
    shadersSuccessfullyCompiled=[]

    for shader in shadersToCompile:
        # compileName is the shader name, but without the extension
        compileName = shader.split(".")[0]

        extension = shader.split(".")[1]
        glslc = "\""+str(os.path.dirname(__file__))+"\\glslc.exe"+"\""

        shaderSRCdir = "\""+SHADER_SOURCE_DIR + shader+"\""
        shaderCMPdir = "\""+COMPILE_TO_DIR + compileName+extension+".spv"+"\""
        cmdLineArgs = glslc+" -c "+shaderSRCdir+" -o "+shaderCMPdir

        #print(cmdLineArgs)
        
        result = subprocess.run(cmdLineArgs,capture_output=True)

        if result.returncode == 0:
            print("Succesfully compiled: " + shader + " to " + compileName+extension+".spv") 
            shadersSuccessfullyCompiled.append(shader)
        else:
            print(result.stdout)
            print(result.stderr)
            print("Failed to compile:", result.returncode)
    
    return shadersSuccessfullyCompiled

def main():
    shadersToCompile = getShadersToCompile()
    print("Compiling " + str(len(shadersToCompile)) + " shaders")
    if (len(shadersToCompile) > 0):
        shadersCompiled = compileShaders(shadersToCompile)
        updateCache(shadersCompiled)
    else:
        print("All shaders are up to date")

if __name__ == "__main__":
    main()