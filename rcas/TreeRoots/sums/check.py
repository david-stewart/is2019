#!/usr/bin/python

#----------------------To do:
# 1.   Get the current absolute file path
# 2-7. Write a list of files with are good, or bad (for no out, no err, no WeEND, or bad_alloc error)
# 8.   Write a summary file
# 9.   hadd the good files together


# run this file from kate/root/[name]
from glob import glob
import subprocess
from subprocess import call
from os import path
import os

# -- 1.
cwd = os.getcwd()

# -- 2.
good_files = []
no_weEND = []
bad_alloc = []
no_out_file = []
no_err_file = []

lists = {'good_file':good_files,
         'no_weEND' :no_weEND,
         'bad_alloc':bad_alloc,
         'no_out_file':no_out_file,
         'no_err_file':no_err_file}

# find the out directory
i = 0
# for r_file in glob('*.root'):
for r_file in glob('output/*.root'):
# for r_file in glob('*_5622.root'):
    # print(r_file)
    if 'hadd' in r_file: # skipp the hadd root_file
        continue

    base = r_file.split('.')[0].split('/')[-1]
    r_file = r_file.split('/')[-1]

    out_file_list = glob('out/*%s'%base)
    if not len(out_file_list) == 1:
        no_out_file.append(r_file)
        continue
    out_file = out_file_list[0]

    err_file_list = glob('err/*%s'%base)
    if not len(err_file_list) == 1:
        no_err_file.append(r_file)
        continue
    err_file = err_file_list[0]

    endline = subprocess.check_output(["tail","-n","1",out_file])
    if 'We END' not in endline:
        no_weEND.append(r_file)
        continue

    try:
        err = subprocess.check_output(["grep","bad_alloc","%s"%err_file])
        if "bad_alloc" in err:
            bad_alloc.append(r_file)
            continue
    except:
        pass

    good_files.append(r_file)
    i+=1
    if (i % 50 == 0):
        print('finished file: %i'%i)

    #TODO:
    # if i > 10:
        # break

dir_summary = 'sub_results'

if not path.isdir(dir_summary):
    os.mkdir(dir_summary)

# -- 3
for key in lists.keys():
    if len(lists[key]) > 0:
        with open ('%s/%s.list'%(dir_summary,key), 'w') as f:
            for x in lists[key]:
                f.write('%s\n'%x)

# with open('%s/good_file.list'%dir_summary,'w') as f:
#     for x in good_files:
#         f.write('%s/%s\n'%(cwd, x))

# # -- 4
# with open ('%s/no_weEND.list'%dir_summary,'w') as f:
#     for x in no_weEND:
#         f.write('%s/%s\n'%(cwd, x))

# # -- 5
# with open ('%s/bad_alloc.list'%dir_summary,'w') as f:
#     for x in bad_alloc:
#         f.write('%s/%s\n'%(cwd, x))

# # -- 6
# with open ('%s/no_out_file.list'%dir_summary,'w') as f:
#     for x in no_out_file:
#         f.write('%s/%s\n'%(cwd, x))

# # -- 7
# with open ('%s/no_err_file.list'%dir_summary,'w') as f:
#     for x in no_err_file:
#         f.write('%s\n'%x)


# subprocess.call(['hadd','hadd_file.root','@good_file.list'])

# -- 8 write a summary file

with open ('%s/summary.txt'%dir_summary,'w') as f:
    f.write("""Note that below, each failure mode is checked sequentially
and therefore if a file fails more than one test, it is only recorded with
the first failure\n\n""")
    f.write("Total number of good files: %i\n"%len(good_files))
    f.write("Total with no out file:     %i\n"%len(no_out_file))
    f.write("Total with no err file:     %i\n"%len(no_err_file))
    f.write("Total with no_weEND:        %i\n"%len(no_weEND))
    f.write("Total with bad_alloc:       %i\n"%len(bad_alloc))
    f.write('\n\n To hadd the results together, go to sub_results and run `star-submit hadd.xml`')

for x in open('%s/summary.txt'%dir_summary,'r').readlines():
    print(x.strip())

# write the hadd xml script and submission script
if not path.isdir("%s/sums"%dir_summary):
    os.mkdir("%s/sums"%dir_summary)

output_path = subprocess.check_output(["readlink","output"]).strip()

with open("%s/hadd.xml"%dir_summary,'w') as f:
    f.write(
'''<?xml version="1.0" encoding="utf-8" ?>
<job maxFilesPerProcess="1" fileListSyntax="xrootd" copyInputLocally="false" simulateSubmission="false" >
    <!-- Script to hadd together the .root files from the results -->
	<command>
        <!-- (a) -->
        cp %s/sub_results/good_file.list .
        cp %s/*.root .
        hadd -f hadd_file.root @good_file.list
	</command>
    <stdout URL="file:./xml_out" />	
    <stderr URL="file:./xml_err" />
    <output fromScratch="hadd_file.root"  toURL="file:%s"/>

	<Generator>
        <Location>      ./sums</Location>
        <ScriptLocation>./sums</ScriptLocation>
        <ListLocation>  ./sums</ListLocation>
	</Generator>
</job>
'''%(cwd,output_path,output_path))
