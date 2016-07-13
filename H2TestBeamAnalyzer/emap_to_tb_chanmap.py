#!/usr/bin/python
#Takes electronics channel map with first two lines as header, properties delimited by spaces, and writes the code for the chanmap in tb_utils.py, to be copied and pasted
import sys
import subprocess

if len(sys.argv) != 2:
    print "Usage: ./emap_to_tb_chanmap.py [emap file]"
else:
    filename = sys.argv[1]
    emapFileShort = filename.rsplit('.',1)[0].rsplit('/')[-1]
    f = open('tb_chanmap-temp.py', 'w')
    with open(filename, 'r') as inFile:
        lines = inFile.readlines()
        i = 0
        reverse = []
        f.write("chanmap = {}\n")
        f.write("#ieta iphi depth = channelId\n")
        for line in lines:
            line = line[:-1]
	    line = line.split("#")[0]
	    if len(line) > 0:
                    i = i + 1
                    channel = line.split()
                    ieta = channel[9]
                    iphi = channel[10]
                    depth = channel[11]
                    f.write("chanmap[%s,%s,%s] = %s\n" % (ieta, iphi, depth, i))
                    reverse.append(("chanmap[%s]  = (%s,%s,%s)" % (i, ieta, iphi, depth)))
        f.write("\n")
        for rev in reverse:
            f.write(rev)
	    f.write("\n")
        f.write("\n")
        f.write("chanlist = range(1,%i)\n" % (i+1))
        f.close()
        subprocess.call(["cp","tb_chanmap-temp.py","tb_chanmap.py"])
	subprocess.call(["cp","tb_chanmap-temp.py","tb_chanmap_"+emapFileShort+".py"])
        subprocess.call(["rm","-f","tb_chanmap-temp.py"])
