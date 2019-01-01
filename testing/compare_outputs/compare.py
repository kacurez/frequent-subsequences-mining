import sys, os
import Gnuplot as gp
from time import sleep

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
        data = line.split()
        #print list(daj_kroky(data[1:]))
        yield data[0], list(daj_kroky(data[1:]))
        
def spade(filename, support):
    cmd = ' '.join(('spade -f', filename, '-s', str(support), '-p'))
    return os.popen(cmd).readlines()

def prefixspan(filename, support):
    cmd = ' '.join(('prefixspan', filename, str(support), '--show-patterns'))
    return os.popen(cmd).readlines()
    
def porovnajvystup(spade,prefixspan,alg = ' prefixspan',printRest=False):
		if alg == ' prefixspan':
			print "spade sequences_num="+str(len(spade))," prefixspan sequences_num=" +str(len(prefixspan))
		for line in spade:
			if line[0] == '<':				
				if line not in prefixspan:
					print line," not present in" + alg											 
			else:
				if line[0] == 'A' and printRest:
					print alg," ",line,
		#print 'OK'
		if alg == ' prefixspan':
			porovnajvystup(prefixspan,spade,' spade')
     
#spade -f filename -s support -t
#prefixspan filename support -t

f = open(sys.argv[1])
graphs = []
for data in daj_data(f):
    file = data[0]
    supporty = data[1]    
    print file    
    for s in supporty:
    	print "@",s,
    	porovnajvystup(spade(file, s),prefixspan(file, s))   




