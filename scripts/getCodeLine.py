import os
import sys

def getNumberOfFileLines(fileName):
    with open(fileName, 'r') as f:
        return len(f.readlines())

def isInSuffixs(fileName, suffixs):
    for suffix in suffixs:
        if fileName.endswith(suffix):
            return True
    return False

def isDirectory(path):
    return os.path.isdir(path)

def isFile(path):
    return os.path.isfile(path)

def getNumberOfCodeLine(path, suffixs):
    cnt = 0
    if isDirectory(path):
        for fileName in os.listdir(path):
            filePath = os.path.join(path, fileName)
            if isFile(filePath):
                if isInSuffixs(fileName, suffixs):
                    print(fileName)
                    cnt += getNumberOfFileLines(filePath)
            elif isDirectory(filePath):
                cnt += getNumberOfCodeLine(filePath, suffixs)
    elif isFile(path):
        if isInSuffixs(path, suffixs):
            print(path)
            return getNumberOfFileLines(path)
    else:
        print('Error: ' + path + ' is not a file or directory')
        sys.exit(1)
    return cnt
count = 0
source_dirs=["/home/book/Siempre/src", "/home/book/Siempre/include", "/home/book/Siempre/test"]
suffix=[".c", ".cpp", ".h", ".hpp", ".cc"]

for i in source_dirs:
    count += getNumberOfCodeLine(i, suffix)

print("总共有" + str(count) + "行代码")
