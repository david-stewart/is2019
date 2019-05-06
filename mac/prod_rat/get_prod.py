import numpy as np
f = open('fig18.txt','r')

data = dict()
lastline = ''
printline = False

first = True

cent_dict = {'40-100%':40,'20-40%':20,'min-bias':-1,'0-20%':0}

for line in open('fig18.txt','r').readlines()[4:]:
    array = line.split()
    if array[0] in ('pi-','pi+','K-','K+','pbar','proton') or line.strip() == '1.075   0.073879    0.002376':
        print('array:::',array)
        if first:
            first = False
        else:
            data[species][cent] = { 'pt' : np.array(pt),
                    'y' : np.array(y),
                    'var' : np.array(var) 
            }
            data[cent][species] = { 'pt' : np.array(pt),
                    'y' : np.array(y),
                    'var' : np.array(var) 
            }
            print('species:',species,'cent:',cent,'len:',len(pt))
            
        # printline = line.strip()
        # print('line:',printline)
        # print('line:',f'{array[0]}__{array[1]}')
    
        species = array[0]
        if species == 'proton':
            species = 'p+'
        elif species == 'pbar':
            species = 'p-' 

        cent  = cent_dict[array[1]]

        print('|| spec:', species, 'cent:', cent)

        if not species in data:
            data[species] = dict()
        if not cent in data:
            data[cent] = dict()

        pt  = []
        y   = []
        var = []
    else:
        pt.append(float(array[0]))
        y.append(float(array[1]))
        var.append(float(array[2]))

print(data['p-'].keys())
print(data['p-'][0]['pt'])

# print ('data:',data)
# for spec in ('p','pi','K'):
    # a = f'{spec}+'
    # b = f'{spec}-'
    # print(f'a: {a}, b: {b}')
    # for cent in (0,20,40):
        # print('cent:',cent)
        # print ('-----------')
        # print ('-----------')
        # print('a:',data[a])
        # print ('*******')
        # if any data[a][cent]['pt'] != data[a][-1]['pt']:
            # print('not same pt range in min-bias and ',cent, 'in ',a)
        # if any data[b][cent]['pt'] != data[b][-1]['pt']:
            # print('not same pt range in min-bias and ',cent, 'in ',b)

# for cent in (-1,0,20,40):
    # for spec in ('p+','p-','pi+','pi-','K+','K-'):
# comp = dict()
# for x in ('p+','p-','pi+','pi-','K+','K-'):
    # if 
