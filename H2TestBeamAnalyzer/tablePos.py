import optparse

eta_centers = {16:9503,
               17:9884,
               18:10205,
               19:10517,
               20:10804,
               21:11074,
               22:11339,
               23:11608,
               24:11883,
               25:12161,
               26:12444
               }
eta_centers_inv = {v:k for k,v in eta_centers.iteritems()}

phi_centers = {1:89536,
               2:72654,
               3:57600,
               4:40500,
               5:23400,
               6:3482}

phi_centers_inv = {v:k for k,v in phi_centers.iteritems()}


def geti(tablei, i_dict):
    # find abs min and max
    imax = max(i_dict.keys())
    imin = min(i_dict.keys())
    if tablei >= imax:
        return i_dict[imax]
    if tablei <= imin:
        return i_dict[imin]

    # find closest center
    closest_up = -1
    closest_up_v = -1
    closest_down = -1
    closest_down_v = -1
    mindist_up = 999999
    mindist_down = 999999
    for k,v in i_dict.iteritems():
        if k > tablei:
            if k - tablei < mindist_up:
                mindist_up = k - tablei
                closest_up = k
                closest_up_v = v
        else:
            if tablei - k < mindist_down:
                mindist_down = tablei - k
                closest_down = k
                closest_down_v = v

    dist = closest_up - closest_down
    dist_v = closest_up_v - closest_down_v
    # Do linear interpolation
    rel_pos = closest_down_v + (float(tablei - closest_down)/dist) * dist_v 

    return rel_pos

def getiphi(tablephi):
    if tablephi < 10:
        return geti(tablephi, phi_centers)
    else:
        return geti(tablephi, phi_centers_inv)

def getieta(tableieta):
    if tableieta < 30:
        return geti(tableieta, eta_centers)
    else:
        return geti(tableieta, eta_centers_inv)


if __name__ == '__main__':

    
    parser = optparse.OptionParser("usage: %prog [options] \n")

    parser.add_option ('-e', '--eta', dest='eta', type='int',
                       default = None,
                       help="eta to convert")
    parser.add_option ('-p', '--phi', dest='phi', type='int',
                       default = None,
                       help="phi to convert")

    options, args = parser.parse_args()

    if options.eta is not None:
        print "Converting eta %s" % (options.eta)
        print "--> %.2f" % getieta(options.eta)

    if options.phi is not None:
        print "Converting phi %s" % (options.phi)
        print "--> %.2f" % getiphi(options.phi)
