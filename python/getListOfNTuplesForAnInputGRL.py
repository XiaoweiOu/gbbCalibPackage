'''
For getting a list of datasets corresponding to a given GRL
'''

import subprocess,os,sys
stderr_log = []

grlpath = sys.argv[1]
grlfile = open(grlpath,'r')

runlist = []
filelist = []

for line in grlfile :
    if not '<Metadata Name="RunList">' in line : 
        continue
    else :
        line = line.replace('<Metadata Name="RunList">','')
        line = line.replace('</Metadata>','')
        line = line.strip()
        runlist = line.split(',')
        print 'list of good runs:',runlist

if 'data15_13TeV' in grlpath :
    command = 'rucio list-dids user.ruth.data15_13TeV.*FTNtupCalib.gbb_v00-01-02*p2950_tuple.root --short | sort'
else : 
    command = 'rucio list-dids user.ruth.data16_13TeV.*FTNtupCalib.gbb_v00-01-02*p2950_tuple.root --short | sort'

# execute the command
p = subprocess.Popen([command], stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)

# save the output of the command
p_communicate = p.communicate()

# stdout: this is the rucio output, so split by line to iterate
ruciolist = p_communicate[0]
ruciolist = ruciolist.splitlines()

# stderr: add to the stderr_log array if error was thrown
p_err = p_communicate[1]
if p_err != '' :
    stderr_log.append(p_err)

# iterate through rucio output and parse it
for dataset in ruciolist :

    goodrun = False

    for index, run in enumerate(runlist) :
        if run in dataset :

            # this run shows up in the GRL!
            goodrun = True

            # if we already have a file for this run, then we overwrite it
            # (as the sorted list should have r-tags below f-tags, and higher 
            # numbered tags below lower numbered ones)
            if len(filelist) > index :
                print 'replacing',filelist[index],'with',dataset
                filelist[index] = dataset
            else :
                filelist.append(dataset)

            print dataset

print '----------------------------------'
print 'filelist:'
print ''

outfile = open('filesPassingGRL.txt','w')
for line in filelist : 
    if line == filelist[0] :
        line = " '"+line+"'"
    else :
        line = ",'"+line+"'"

    outfile.write(line+'\n')
    print line

print ''
print '----------------------------------'

print 'checking for errors...'
print ''

if len(stderr_log) > 0 :
    print "Errors occurred!"
    for err_line in stderr_log :
        print err_line.strip()
elif len(filelist) != len(runlist) :
    print 'WARNING!!',len(runlist),'good runs but',len(filelist),'files'
else :
    print 'Success!'
