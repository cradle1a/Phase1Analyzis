from tablePos import *
from runlists import *

def makeTwikiFragment(runtable, snippetname):
    """ Create a long format table with all run information for the given runtable. """

    f = open(snippetname, 'w')
    
    f.write("| Run number | Eta | Phi | # events | beam counters | beam type | QIE shunt | Notes | \n")

    for k in sorted(runtable.keys()):
        l = "| %(run)s | %(teta)s (%(ieta).1f) | %(tphi)s (%(iphi).1f) | %(nev)s | %(bc)s | %(bt)s | %(shunt)s | | \n" % {"run" : k,
                                                                                                                          "teta" : runtable[k][0],
                                                                                                                          "tphi" : runtable[k][1],
                                                                                                                          "nev" : runtable[k][2],
                                                                                                                          "bc" : runtable[k][3],
                                                                                                                          "bt" : runtable[k][4],
                                                                                                                          "ieta": getieta(runtable[k][0]),
                                                                                                                          "iphi": getiphi(runtable[k][1]),
                                                                                                                          "shunt": runtable[k][5]}
        f.write(l)
        
    f.close()


def makeEtaPhiTable(runtable, snippetname):
    """ Create a 2D (eta,phi) table for all runs in the specified runTable. """

    f = open(snippetname, 'w')

    # find all eta values in table
    etaset = set([info[0] for info in runtable.values()])
    # order them
    etalist = sorted(list(etaset))
    
    # find all phi values in table
    phiset = set([info[1] for info in runtable.values()])
    philist = sorted(list(phiset))

    heading = " | ".join([str(phi) for phi in philist])
    f.write("| | Phi | %s |\n" % (heading))
    heading2 = " | ".join(["%.2f"%getiphi(phi) for phi in philist])
    f.write("| Eta | | %s |\n" % (heading2))

    for eta in etalist:
        # construct the line in eta
        ieta = getieta(eta)
        l1 = "| %(eta)s | %(ieta).2f | " % locals()
        
        # go through runs and find all compatible with this eta
        runlist = [" "]*len(philist)
        for r,info in runtable.iteritems():
            if info[0] == eta:
                # find which phi it is
                phi_index = philist.index(info[1])
                runlist[phi_index] = str(r)
        l2 = " | ".join(runlist)
        f.write("%s%s |\n" % (l1, l2))

    f.close()


if __name__ == '__main__':

    print "Make sure to uncomment what you want to turn into a twiki snippet"
    makeTwikiFragment(runTable_pions_ODU12, "twiki1.txt")
    #makeEtaPhiTable(runTable_pions_ODU12, "twiki2.txt")
