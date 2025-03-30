'''
┌───────────────────────────────────────────┐ 
│installDeps.py                             │
│Author: Tripp R.                           │
│-------------------------------------------│
│A simple tool to install most of the       │
│dependencies needed to compile the Vulkan  │
│Abstraction Library                        |
│                                           │
└───────────────────────────────────────────┘ 
'''

import requests
import time;
import os;
import sys;
import platform
import zipfile
import shutil
import urllib
import urllib.request
from urllib.request import urlopen

EXTERNAL_DEPENDENCY_DIR = "../ExternalLibraries/"

TEMP_DOWNLOAD_DIR = "VAL_INSTALL_DEP_TEMP_INSTALL_FOLDER"


def update_download_progress_bar(percent):
    totalLen = 25
    bar = ["░"] * totalLen
    for i in range(0,round(totalLen*percent)):
        bar[i] = "█"

    bar_str = "".join(bar) #convert list to str
    print(f"\rDownloading: {bar_str} {(percent*100):.1f}%", end="", flush=True)

        
def download_file(url, save_path):
    response = urllib.request.urlopen(url)
    if response.getcode() == 200:
        meta = response.info()
        total_size = int(meta.get("Content-Length"))

        with open(save_path, "wb") as file:
            downloaded_size = 0
            downloaded_size = 0
            while True:
                chunk = response.read(1024)  # 1 KB
                if not chunk:
                    break
                file.write(chunk)
                downloaded_size += len(chunk)
                if total_size:
                    update_download_progress_bar(downloaded_size/total_size)
                else:
                    print(f"\rDownloading: {downloaded_size} bytes", end="", flush=True)
        
        print("");
    else:
        print(f"Failed to pull file from URL: {url}");


        
def download_and_extract(url, save_path, extract_to, rename_top_folder = None):
    print(f"Downloading file from {url}...")
    response = requests.get(url)
    
    if response.status_code == 200:
        #download file
        with open(save_path, "wb") as f:
            f.write(response.content)
        print(f"Downloaded to {save_path}")

        # keep track of the files that already existed in the dir
        before_files = set(os.listdir(extract_to)) if os.path.exists(extract_to) else set()


        # unzip file
        if zipfile.is_zipfile(save_path):
            with zipfile.ZipFile(save_path, "r") as zip_ref:
                zip_ref.extractall(extract_to)


            after_files = set(os.listdir(extract_to))
            extracted_files = list(after_files - before_files)

            if rename_top_folder:
                for item in extracted_files:
                    item =  extract_to+item
                    if (os.path.isdir(item)):
                        os.rename(item, extract_to+"GLFW")
                        print(f"Extracted {extract_to+"GLFW"}")
            else:
                print(f"Extracted {extract_to}")

        else:
            print("The downloaded file is not a valid zip file.")
    else:
        print(f"Failed to download the file. Status code: {response.status_code}")

def get_platform_info():
    system = platform.system()  # Get OS name
    architecture = "x64" if sys.maxsize > 2**32 else "x32"  # Determine 32-bit or 64-bit

    if system == "Windows":
        return f"Windows {architecture}"
    elif system == "Darwin":
        return "macOS"
    elif system == "Linux":
        return "Linux"
    else:
        return "Unknown OS"


'''
┌──────────────────────────────────────────────────────────┐ 
│                           MAIN                           │
└──────────────────────────────────────────────────────────┘ 
'''


files = {
    "stb_image.h": "https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_image.h",
    "tiny_obj_loader.h": "https://raw.githubusercontent.com/tinyobjloader/tinyobjloader/refs/heads/release/tiny_obj_loader.h"
}

codebases = {"glm": "https://github.com/g-truc/glm/releases/download/1.0.1/glm-1.0.1-light.zip"}
if (get_platform_info() == "Windows x64"):
    codebases = {"GLFW": "https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.WIN64.zip"}
elif (get_platform_info() == "Windows x32"):
    codebases = {"GLFW": "https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.WIN32.zip"}
elif (get_platform_info() == "macOS"):
    codebases = {"GLFW": "https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.MACOS.zip"}
elif(get_platform_info() == "Linux"):
    print("GLFW will not be installed as it is not available as a prebuilt binary for Linux. You will have to build it from source, read more here: https://www.glfw.org/docs/3.3/compile.html")


# Create the External Libraries path if it doesn't yet exist
if (os.path.isdir(EXTERNAL_DEPENDENCY_DIR) == False):
    os.makedir(EXTERNAL_DEPENDENCY_DIR)


# Zip files will be downloaded to this temp dir and then deleted
os.mkdir(TEMP_DOWNLOAD_DIR)

try:
    # Download each file
    for filename, url in files.items():
        filename = EXTERNAL_DEPENDENCY_DIR+filename
    

        if os.path.exists(filename):
            print(f"Dependency '{filename}' is already installed, would you like to redownload it? (y/n)")
            c = input()
            if (c=='y'):
                    download_file(url, filename)
            else:
                continue;
        else:
            download_file(url, filename)

    for pkgName, pkg in codebases.items():
        pkgFilename = EXTERNAL_DEPENDENCY_DIR + pkgName

        renameTopDir=None
        if (pkgName == "GLFW"):
            renameTopDir="GLFW"

        if os.path.exists(pkgFilename):
            print(f"Dependency '{pkgName}' is already installed, would you like to redownload it? (y/n)")
            c = input()
            if (c=='y'):
                # clear the existing codebase
                shutil.rmtree(pkgFilename)

                download_and_extract(codebases[pkgName], TEMP_DOWNLOAD_DIR + "/glmZip", EXTERNAL_DEPENDENCY_DIR,renameTopDir)
            else:
                continue;
        else:
            download_and_extract(codebases[pkgName], TEMP_DOWNLOAD_DIR + "/glmZip", EXTERNAL_DEPENDENCY_DIR,renameTopDir)

    print("The last step to finish the installation is more complicated and cannot be done via this script:")
    print("1) Install the Vulkan SDK from https://vulkan.lunarg.com/")
    print("2) Copy and paste the /Lib and /Bin directories from your VulkanSDK installation into VAL/ExternalLibaries")
    print("3) Find the glslc.exe and glslangValidator.exe from the VulkanSDK/<version-no>/Bin folder into VAL/ExternalLibaries. These are used to compile and debug shaders.")

except Exception as e:
    exc_type, exc_obj, exc_tb = sys.exc_info()
    fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
    print(exc_type, fname, exc_tb.tb_lineno)

if (len(TEMP_DOWNLOAD_DIR) == 0 or TEMP_DOWNLOAD_DIR == '/'):
    exit;

for file in os.listdir(TEMP_DOWNLOAD_DIR):
    os.remove(TEMP_DOWNLOAD_DIR + "/" + file)
os.rmdir(TEMP_DOWNLOAD_DIR)