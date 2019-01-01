import sge,sys,os
fronta = 'vip_seq.q' 
spade_dir = "~/bp/spade/build/spade"
prefixspan_dir = "~/bp/prefixspan/build/prefixspan"
def je_rozsah(a):
    return a.find('|') != -1

def daj_kroky(l):
    for x in l:
        if je_rozsah(x):
            tmp, krok = x.split('|')
            start, end = tmp.split('-')
            for y in range(int(start), int(end)+int(krok), int(krok)):
                yield y
        else:
            yield int(x)

def daj_data(fin):
    for line in fin:
        if line[0]=='#':
          continue        
        data = line.split()
        print list(daj_kroky(data[1:]))
        yield data[0], list(daj_kroky(data[1:]))
        
def spade(filename, support):
    cmd = ' '.join((spade_dir,' -f', filename, '-s', str(support), '-t'))
    jname = ''.join((filename,'-','spade-',str(support),'-'))
    return sge.Job(jname,cmd,fronta)
    #return os.popen(cmd).read()

def prefixspan(filename, support):
    cmd = ' '.join((prefixspan_dir, filename, str(support), '-t'))
    jname = ''.join((filename,'-','prefixspan-',str(support),'-'))
    return  sge.Job(jname,cmd,fronta)
    #return os.popen(cmd).read() 
        
#spade -f filename -s support -t
#prefixspan filename support -t
"""
if os.path.abspath.exists(spade_dir) == False:
  print "missing spade path"
  sys.exit()
if os.path.isfile(prefixspan_dir) == False:
  print "missing prefixspan path"
  sys.exit()
"""
  
f = open(sys.argv[1])
jobs = []
for data in daj_data(f):
    file = data[0]
    supporty = data[1]
    list_x = []
    spade_y = []
    prefixspan_y = []
    print file
    for s in supporty:
        list_x.append(s)
        jobs.append(spade(file,s))
        jobs.append(prefixspan(file,s))
        #spade_y.append(int(spade(file, s)))
        #prefixspan_y.append(int(prefixspan(file, s)))
sge.build_submission('./',jobs)        
        
        
        
        
        
        
