# just print out in a cvs and use Excel
import numpy as np
f_out = open('fig18.csv','w')

data = dict()
lastline = ''
printline = False

first = True

cent_dict = {'40-100%':40,'20-40%':20,'min-bias':-1,'0-20%':0}

for line in open('fig18.txt','r').readlines()[4:]:
    array = line.split()
    if array[0] in ('pi-','pi+','K-','K+','pbar','proton'):
        pass
    f_out.write(', '.join(line.split())+'\n')
