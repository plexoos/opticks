#!/usr/bin/env python
"""

::

   ipython -i  OInterpolationTest_identity.py


"""
import os, numpy as np

from opticks.ana.proplib import PropLib

np.set_printoptions(precision=3, suppress=True)



if __name__ == '__main__':

    blib = PropLib("GBndLib")
    names = blib.names
    t = np.load(os.path.expandvars("$IDPATH/GBndLib/GBndLib.npy"))

    nl = 39
    o = np.load(os.path.expandvars("$TMP/InterpolationTest/OInterpolationTest_identity.npy")).reshape(-1,4,2,nl,4) 
    c = np.load(os.path.expandvars("$TMP/InterpolationTest/CInterpolationTest_identity.npy")).reshape(-1,4,2,nl,4) 

    assert np.all(t == o)

    assert len(t) == len(names)
    assert len(t) == len(o) 
    assert len(t) == len(c) 
    n = len(t)

    ok={}
    g4={}
    g4b={}

    for i in range(n):

        name = names[i]
        omat,osur,isur,imat = name.split("/")

        ok_omat = np.all( t[i,blib.B_OMAT,0] == o[i,blib.B_OMAT,0] )
        ok_imat = np.all( t[i,blib.B_IMAT,0] == o[i,blib.B_IMAT,0] )

        g4_omat = np.all( t[i,blib.B_OMAT,0] == c[i,blib.B_OMAT,0] )
        g4_imat = np.all( t[i,blib.B_IMAT,0] == c[i,blib.B_IMAT,0] )


        if omat in ok:
            assert ok[omat] == ok_omat
        else:
            ok[omat] = ok_omat

        if imat in ok:
            assert ok[imat] == ok_imat
        else:
            ok[imat] = ok_imat

        if omat in g4:
            assert g4[omat] == g4_omat
        else:
            g4[omat] = g4_omat

        if imat in g4:
            assert g4[imat] == g4_imat
        else:
            g4[imat] = g4_imat
 


        if len(osur) > 0:
            ok_osur = np.all( t[i,blib.B_OSUR,0] == o[i,blib.B_OSUR,0] )
            g4_osur = np.all( t[i,blib.B_OSUR,0] == c[i,blib.B_OSUR,0] )

            if osur in ok:
                assert ok[osur] == ok_osur
            else:
                ok[osur] = ok_osur

            if osur in g4:
                assert g4[osur] == g4_osur
            else:
                g4[osur] = g4_osur
        else:
            ok_osur = None
            g4_osur = None
        pass


     
        if len(isur) > 0:
            ok_isur = np.all( t[i,blib.B_ISUR,0] == o[i,blib.B_ISUR,0] )
            g4_isur = np.all( t[i,blib.B_ISUR,0] == c[i,blib.B_ISUR,0] )
            if isur in ok:
                assert ok[isur] == ok_isur
            else:
                ok[isur] = ok_isur

            if isur in g4:
                assert g4[isur] == g4_isur
            else:
                g4[isur] = g4_isur
        else:
            ok_isur = None
            g4_isur = None
        pass

     




        if g4_omat == False:
           if not omat in g4b:
               g4b[omat] = []
           g4b[omat].append( (i,blib.B_OMAT,0) )  

        if g4_osur == False:
           if not osur in g4b:
               g4b[osur] = []
           g4b[osur].append( (i,blib.B_OSUR,0) )  

        if g4_isur == False:
           if not isur in g4b:
               g4b[isur] = []
           g4b[isur].append( (i,blib.B_ISUR,0) )  

        if g4_imat == False:
           if not imat in g4b:
               g4b[imat] = []
           g4b[imat].append( (i,blib.B_IMAT,0) )  
 

        #print "%4d omat %25s imat %25s         ok_omat %7s ok_imat %7s      g4_omat %7s g4_imat %7s " % (  i, omat, imat, ok_omat, ok_imat,  g4_omat, g4_imat )  

        if len(isur) > 0 or len(osur) > 0:
            print "%4d osur %35s isur %35s         ok_osur %7s ok_isur %7s      g4_osur %7s g4_isur %7s " % (  i, osur, isur, ok_osur, ok_isur,  g4_osur, g4_isur )  
              
 


    pass


    print "ok", ok
    print "g4", g4

    for k,v in g4b.items():
        if len(v) > 0:print k, str(v)





