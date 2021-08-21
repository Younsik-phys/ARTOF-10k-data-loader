import numpy as np
import os

def npytoitx(a, binsize, Eparam, T = 'binMatrix'):

    ff = open(os.path.dirname(os.path.realpath(__file__)) + '/' + T + '.itx',"w")

    ff.write('IGOR\nWAVES/N=('+str(binsize[0])+','+str(binsize[1])+','+str(binsize[2])+')   binMatrix\nBEGIN\n')
    for k in range(binsize[2]):
        for i in range(binsize[0]):
            for j in range(binsize[1]):
                ff.write('  '+str(round(a[i][j][k],4)))
            ff.write('\n')
        ff.write('\n')

    ff.write('END\n')
    ff.write('X SetScale/P x -22,'+str(44/binsize[0])+',"thetaX [deg]", binMatrix; SetScale/P y -22,'+str(44/binsize[1])+',"thetaY [deg]", binMatrix; SetScale/P z '+str(Eparam[0])+','+str((Eparam[1]-Eparam[0])/binsize[2])+',"energy [eV]", binMatrix;\n')


    ff.close()

