#!/usr/bin/env python
"""
Hmm need to make connection to the volume traversal index 
"""
import os, logging, numpy as np
log = logging.getLogger(__name__)

from opticks.ana.geocache import keydir
from opticks.ana.prim import Dir
from opticks.ana.geom2d import Geom2d

np.set_printoptions(suppress=True)


if __name__ == '__main__':

    logging.basicConfig(level=logging.INFO)

    ok = os.environ["OPTICKS_KEY"]
    kd = keydir(ok)
    log.info(kd)
    assert os.path.exists(kd), kd 

    os.environ["IDPATH"] = kd    ## TODO: avoid having to do this, due to prim internals

    mm0 = Geom2d(kd, ridx=0)


    import matplotlib.pyplot as plt 

    plt.ion()
    fig = plt.figure(figsize=(6,5.5))
    ax = fig.add_subplot(111)
    plt.title("mm0 geom2d")
    sz = 50 
    ax.set_ylim([-sz,sz])
    ax.set_xlim([-sz,sz])

    mm0.render(ax)


    pz = 0.3
    pr = 0.32
    sc = 30.    # half the extent of world volume in meters


    dtype = np.float32

    phase0 = np.arccos(pz) 
    t0 = np.linspace( 0, 2*np.pi, 20 )[:-1]
    m0 = len(t0)
    st0 = np.sin(t0+phase0)
    ct0 = np.cos(t0+phase0)


    oxz = np.zeros( (m0,3) , dtype=dtype )
    oxz[:,0] = st0
    oxz[:,1] = 0
    oxz[:,2] = ct0

    uxz = np.zeros( (m0,3) , dtype=dtype )
    uxz[:,0] = st0
    uxz[:,1] = 0 
    uxz[:,2] = ct0 

    oxz *= pr



    n = 2 + m0 
    eye = np.zeros( (n, 3), dtype=dtype )
    look = np.zeros( (n, 3), dtype=dtype )
    up = np.zeros( (n, 3), dtype=dtype )

    eye[0] = [-1, 0, pz] 
    eye[1] = [ 0, 0, pz]
    eye[2:2+len(oxz)] = oxz

    up[0] = [0,0,1]
    up[1] = [0,0,1]
    up[2:2+len(uxz)] = uxz




    look[:-1] = eye[1:]
    look[-1] = eye[0]

    gaze = look - eye

    x = sc*eye[:,0] 
    z = sc*eye[:,2]

    #u = gaze[:, 0] 
    #w = gaze[:, 2] 

    u = up[:, 0] 
    w = up[:, 2] 

 
   
    #ax.plot( x,z )
    ax.quiver( x, z, u, w  ) 



    labels = True
    if labels:
        for i in range(len(eye)):
            plt.text( x[i], z[i], i , fontsize=12 )
        pass  
    pass


    fig.show()

    elu = np.zeros( (n,4,4), dtype=np.float32)
    elu[:,0,:3] = eye 
    elu[:,1,:3] = look
    elu[:,2,:3] = up
    np.save("/tmp/flightpath.npy", elu ) 


