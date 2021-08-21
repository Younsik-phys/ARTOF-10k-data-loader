from ctypes import *
import numpy as np
import re
import matplotlib.pyplot as plt
import time
import os
import npytoitx_tof

def findcfg(str0, str1):
    p = re.compile(str1)
    try:
        m = p.match(str0)
        return str0[m.end():]
    except:
        return '-1'

def matrixstr(str0):
    
    mat = np.zeros((21, 21))
    str1 = str0[1:-2].split()
    for i in range(21):
        for j in range(21):
           mat[i][j] = float(str1.pop())

    return mat

def vectorstr(str0):
    
    mat = np.zeros(21)
    str1 = str0[1:-2].split()
    for i in range(21):
        mat[i] = float(str1.pop())

    return mat

class params(Structure):
    _fields_ = [
        ('iterations', c_int32),
        ('tofvector', c_double * 21),
        ('radiusvector', c_double * 21),
        ('energymatrix', c_double * 21 * 21),
        ('thetamatrix', c_double * 21 * 21),
        ('xvector', c_double * 21),
        ('yvector', c_double * 21),
        ('xmatrix', c_double * 21 * 21),
        ('ymatrix', c_double * 21 * 21),
        ('t0', c_int32),
        ('centerE', c_double),
        ('beginE', c_double),
        ('endE', c_double),
        ('binsize', c_int * 3),
        ('filedirectory', c_char * 300),
        
    ]

#main start

filedirectory = "C:/Users/leblang/Dropbox/Younsik/data/2007FeTe_sys3/200708FeTe/2020.07.08.17.38.32/"

with open(filedirectory + "acquisition.cfg","r") as cfg:

    line = cfg.readlines()

    for i in line:
        if findcfg(i, 'lensIterations=') != '-1':
            iterations = int(findcfg(i, 'lensIterations='))
        elif findcfg(i, 'tofVector=') != '-1':
            tofvector = vectorstr(findcfg(i, 'tofVector='))
        elif findcfg(i, 'radiusVector=') != '-1':
            radiusvector = vectorstr(findcfg(i, 'radiusVector='))
        elif findcfg(i, 'energyMatrix=') != '-1':
            energymatrix = matrixstr(findcfg(i, 'energyMatrix='))
        elif findcfg(i, 'thetaMatrix=') != '-1':
            thetamatrix = matrixstr(findcfg(i, 'thetaMatrix='))
        elif findcfg(i, 'transformXVector=') != '-1':
            xvector = vectorstr(findcfg(i, 'transformXVector='))
        elif findcfg(i, 'transformYVector=') != '-1':
            yvector = vectorstr(findcfg(i, 'transformYVector='))    
        elif findcfg(i, 'transformXMatrix=') != '-1':
            xmatrix = matrixstr(findcfg(i, 'transformXMatrix='))
        elif findcfg(i, 'transformYMatrix=') != '-1':
            ymatrix = matrixstr(findcfg(i, 'transformYMatrix='))  
        elif findcfg(i, 't0=') != '-1':
            t0 = int(findcfg(i, 't0='))        
        elif findcfg(i, 'centerEnergy') != '-1':
            centerenergy = float(findcfg(i, 'centerEnergy='))   
        elif findcfg(i, 'spectrumBeginEnergy=') != '-1':
            beginenergy = float(findcfg(i, 'spectrumBeginEnergy='))   
        elif findcfg(i, 'spectrumEndEnergy=') != '-1':
            endenergy = float(findcfg(i, 'spectrumEndEnergy='))   

    energymatrix *= centerenergy/10

binsize = np.array([100, 150, 150])

mydll = WinDLL('C:/core.dll')

corefunc = mydll['cal']
corefunc.argtypes = [np.ctypeslib.ndpointer(dtype=np.float64, ndim=3, flags="C_CONTIGUOUS"), POINTER(params)]
corefunc.restype = c_int32


c_filedirectory = filedirectory.replace("/", "\\").encode()
param = params(iterations, 
                np.ctypeslib.as_ctypes(tofvector), 
                np.ctypeslib.as_ctypes(radiusvector), 
                np.ctypeslib.as_ctypes(energymatrix),
                np.ctypeslib.as_ctypes(thetamatrix),
                np.ctypeslib.as_ctypes(xvector),
                np.ctypeslib.as_ctypes(yvector),
                np.ctypeslib.as_ctypes(xmatrix),
                np.ctypeslib.as_ctypes(ymatrix),
                t0,
                centerenergy,
                beginenergy,
                endenergy,
                np.ctypeslib.as_ctypes(binsize),
                c_filedirectory
                )

a = np.zeros(binsize, dtype = np.float64)


stime = time.time()

if (corefunc(a, byref(param)) == 0):
    print("reading complete.")
    print("elapsed time :", round(time.time() - stime, 1), "seconds")
    print("writing started...")
else:
    print("read error")


npytoitx_tof.npytoitx(a, binsize, [beginenergy, endenergy])

print("successfully loaded data")

