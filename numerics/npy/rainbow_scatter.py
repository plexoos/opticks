#!/usr/bin/env python
"""
"""

import os, logging, numpy as np
log = logging.getLogger(__name__)

import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.patches import Rectangle

from env.numerics.npy.ana import Evt, Selection, costheta_, cross_
from env.numerics.npy.geometry import Boundary   
from env.numerics.npy.fresnel import fresnel_factor



deg = np.pi/180.
n2ref = 1.33257


   
def scattering_angle_plot(pmax=4, n=n2ref):
    idom = np.arange(0,90,1)*deg 
    fig = plt.figure()
    fig.suptitle("Scattering angle for different orders, 0:R, 1:T, 2:TRT, 3:TRRT ")
    ax = fig.add_subplot(1,1,1)
    for i,p in enumerate(range(pmax)):
        thp = scattering_angle(idom, p, n=n)
        msk = ~np.isnan(thp)
        ax.plot( idom[msk]/deg, thp[msk]/deg, label=p)
    pass
    ax.legend()


def intensity(i, k=1, n=n2ref):
    """
    This goes to infinity at the bow angles 
    """
    r = np.arcsin( np.sin(i)/n )                    
    dthdi = 2.-2*(k+1)*np.tan(r)/np.tan(i)
    return 1./dthdi


def intensity_plot(k=1, n=n2ref):
    """
    Need intensity as function of scattering angle summing over all bows 
    (and individually per bow and for reflection and transmission too)
    to compare against the unselected deviation plot

    Hmm this depends on the distribution of incident angles ...
    """
    dom = np.linspace(0,360,361)
    it = intensity(dom*deg, k=k, n=n )
    msk = ~np.isnan(it)
    plt.plot( dom[msk], it[msk] )




class Scatter(object):
    """
    View a a general scattering process, without assuming particular bow angle

    Comparison of the various ways of calculating rainbows: Descartes, Newton, Mie, Debybe, Airy

    * http://www.philiplaven.com/p8e.html


    :param p: type of scatter

    p=0 
          reflection from the exterior of the sphere;
    p=1
          transmission through the sphere
    p=2
          one internal reflection
    p=3
          two internal reflections

    p=0     2i - 180
    p=1     2i - 2r
    p > 1   2i - 2pr + (p-1)180

    """

    @classmethod
    def seqhis(cls, pp):
        return map(cls.seqhis_, pp) 

    @classmethod
    def seqhis_(cls, p):
        if p == 0:
            seq = "BR "
        elif p == 1:
            seq = "BT BT "
        elif p > 1:
            seq = "BT " + "BR " * (p-1) + "BT "  
        else:
            assert 0 
        pass
        seq += "SA"
        return seq 

    def __init__(self, p, n1=1, n2=n2ref, psim=None, ssim=None, not_=False):

        composite = type(p) is not int
        if composite:
            seq = self.seqhis(p)
            k = None
        else: 
            seq = [self.seqhis_(p)]
            k = p - 1
        pass

        self.p = p
        self.k = k
        self.n1 = n1
        self.n2 = n2
        self.n  = n2/n1
        self.seq = seq
        self.not_ = not_

        log.info("seq [%s] " % seq )

        if psim is not None:
           psim = Selection(psim, seq, not_=not_)
        self.psim = psim

        if ssim is not None:
           ssim = Selection(ssim, seq, not_=not_)
        self.ssim = ssim


    def __repr__(self):
        extra = ""
        if self.k is not None and self.p > 1:
            extra = " BOW k=%d " % self.k
        pass
        return "Scatter p=%s %s %s  %s" % (self.p, self.seq,self.not_, extra)

    def theta(self, i):
        """
        :param i: incident angle arising from beam characteristic, and orientation to the sphere
        :return scattering angle:        

        For a half illuminated drop the incident angle will vary from
        zero at normal incidence to pi/2 at tangential incidence
        """
        n = self.n
        p = self.p
        k = self.k
 
        i = np.asarray(i)
        r = np.arcsin( np.sin(i)/n )

        #th = np.piecewise( i, [p==0,p==1,p>1], [np.pi - 2*i, 2*(i-r), k*np.pi + 2*i - 2*r*(k+1) ])  
        # should be lambda funcs to get this to work 
    
        if p == 0:
           th = 2*i - np.pi 
        elif p == 1:
           th = 2*(i-r)
        elif p > 1:
           th = k*np.pi + 2*i - 2*r*(k+1)
        else:
           assert 0    

        # maybe discrep between angle range convention ?
        return th % (2.*np.pi)

    def dthetadi(self, i):
        """
        This goes to infinity at the bow angles 
        """
        n = self.n
        p = self.p
        k = self.k
        i = np.asarray(i)
        r = np.arcsin( np.sin(i)/n )                    

        if p == 0:
            dthdi = 2.*np.ones(len(i))
        elif p == 1:
            dthdi = 2.-2*np.tan(r)/np.tan(i)
        elif p > 1:
            dthdi = 2.-2*(k+1)*np.tan(r)/np.tan(i)
        else:
            assert 0
        return dthdi


    def intensity(self, i):
        """
        Particular incident angle yields a particular scatter angle theta, 
        BUT how much is scattered in a particular theta bin relative to the
        total ?

        For "BR SA" geom is contant 0.5 and dthetadi is 2. leaving just the 
        familiar from reflection.py fresnel S, P factors.

        Are missing the modululation coming from the initial beam.

        What is the primordial incident angle distribution within the category ?  
        """
        pass
        th = self.theta(i)
        dthdi = self.dthetadi(i)
        geom = np.abs(0.5*np.sin(2*i)/np.sin(th))

        assert len(self.seq) == 1
        seq = self.seq[0]
 
        s = fresnel_factor(seq, i, self.n1, self.n2, spol=True)
        p = fresnel_factor(seq, i, self.n1, self.n2, spol=False)

        #adhoc = np.sin(i)

        s_inten = s*geom/dthdi 
        p_inten = p*geom/dthdi 

        return th, s_inten, p_inten

    def table(self, i):
        th = self.theta(i)
        dthdi = self.dthetadi(i)
        geom = np.abs(0.5*np.sin(2*i)/np.sin(th))

        assert len(self.seq) == 1
        seq = self.seq[0]

        s = fresnel_factor(seq, i, self.n1, self.n2, spol=True)
        p = fresnel_factor(seq, i, self.n1, self.n2, spol=False)

        s_inten = s*geom/dthdi 
        p_inten = p*geom/dthdi 

        qwns = "i/deg,s,p,s+p,th/deg,dthdi, geom, s_inten, p_inten".split(",")
        log.info(self)

        print "  " + "  ".join(["%-9s" % qwn for qwn in qwns])
        return np.dstack([i/deg,s,p,s+p,th/deg,dthdi, geom, s_inten, p_inten])


    @classmethod
    def combined_intensity(cls, pp=[1,2,3],  n1=1, n2=n2ref):
        idom = np.arange(0,90,.1)*deg 
        c_th = [] 
        c_si = []
        c_pi = []
        for p in pp:
            sc = Scatter(p, n1=n1, n2=n2)
            th,si,pi = sc.intensity(idom)
            c_th.append(th)
            c_si.append(si)
            c_pi.append(pi)
        pass
        cth = np.hstack(c_th)
        csi = np.hstack(c_si)
        cpi = np.hstack(c_pi)

        return cth, csi, cpi

    @classmethod
    def combined_intensity_plot(cls, pp=[1,2,3,4], log_=True, ylim=None, flip=False, scale=1, mask=False):
        tsp = cls.combined_intensity(pp)
        cls._intensity_plot(tsp, log_=log_, ylim=ylim, flip=flip, scale=scale, mask=mask)
            
    @classmethod
    def _intensity_plot(cls, tsp, log_=True, ylim=None, flip=False, scale=1, mask=False):

        th, s_inten, p_inten = tsp
        if flip:
           s_inten = -s_inten 
           p_inten = -p_inten 

        if mask:
           msk = s_inten > 0
        else:
            msk = np.tile(True, len(th)) 

        ax = plt.gca()
        ax.plot( th[msk]/deg, s_inten[msk]*scale )
        ax.plot( th[msk]/deg, p_inten[msk]*scale )
 
        if ylim:
            ax.set_ylim(ylim)
        if log_:
            ax.set_yscale('log')

    def intensity_plot(self, i, log_=True, ylim=None, flip=False, scale=1, mask=False):
        """
        Lots of negative lobes
        """
        tsp = self.intensity(i)
        self._intensity_plot(tsp, log_=log_, ylim=ylim, flip=flip, scale=scale, mask=mask)

def scatter_plot_all(pevt, sevt):
    s_dv0 = sevt.deviation_angle()
    p_dv0 = pevt.deviation_angle()
    db = np.arange(0,360,1)

    ax = fig.add_subplot(1,1,1)

    for i,d in enumerate([s_dv0/deg, p_dv0/deg]):
        ax.set_xlim(0,360)
        ax.set_ylim(1,1e5)
        cnt, bns, ptc = ax.hist(d, bins=db,  log=True, histtype='step')
    pass

   
def scatter_plot_split(pevt, sevt, pp=range(1), ylim=None):
    pass
    db = np.arange(0,360,1)
    nplot = len(pp)+1
    ax0 = None
    for i,p in enumerate(pp+[pp]):
        not_ = type(p) is not int
        sc = Scatter(p, psim=pevt, ssim=sevt, not_=type(p) is not int)
        if ax0 is None:
            ax0 = ax = fig.add_subplot(nplot,1,i+1)
        else:
            ax = fig.add_subplot(nplot,1,i+1, sharex=ax0)
        pass
        _scatter_plot_one(ax, sc, bins=db, ylim=ylim)
    

def scatter_plot_component(pevt, sevt, p=0, ylim=None):
    db = np.arange(0,360,1)
    sc = Scatter(p, psim=pevt, ssim=sevt, not_=False)

    idom = np.arange(0,90,1.)*deg 
    tsp = sc.intensity(idom)

    ax0 = fig.add_subplot(2,1,1)
    _scatter_plot_one(ax0, sc, bins=db, ylim=ylim)
    sc._intensity_plot(tsp, log_=True, ylim=ylim, flip=False, scale=10000, mask=False)

    ax = fig.add_subplot(2,1,2, sharex=ax0)
    sc._intensity_plot(tsp, log_=True, ylim=ylim, flip=False, scale=1, mask=False)

    print sc.table(idom[::10])
    sc.tsp = tsp
    return sc
   

def _scatter_plot_one(ax, sc, bins, ylim=None):
    s_dv = sc.ssim.deviation_angle()
    p_dv = sc.psim.deviation_angle()

    for i,d in enumerate([s_dv/deg, p_dv/deg]):
        cnt, bns, ptc = ax.hist(d, bins=bins,  log=True, histtype='step')
    pass
    ax.set_xlim(0,360)
    if ylim is not None:
        ax.set_ylim(ylim)
 



def check_radius(sc, sli=slice(None)):
    """
    Huh getting wrong radius::

        In [53]: p1 = sc.ssim.recpost(1)[:,:3]

        In [54]: p1
        Out[54]: 
        array([[-80.9046, -19.9591,  -0.0305],
               [-49.0738, -62.1967, -25.8492],
               [-42.5733,  52.9191, -48.3108],
               ..., 
               [-42.2376,   5.2187, -71.6575],
               [-17.7007,  80.4773, -12.36  ],
               [-13.245 ,  76.7541, -29.6335]])

        In [55]: np.linalg.norm(p1, 2, 1)
        Out[55]: array([ 83.3302,  83.3357,  83.3478, ...,  83.3429,  83.3228,  83.3352])

        In [56]: rp1 = np.linalg.norm(p1, 2, 1)

        In [57]: rp1
        Out[57]: array([ 83.3302,  83.3357,  83.3478, ...,  83.3429,  83.3228,  83.3352])

        In [58]: rp1.min()
        Out[58]: 83.308185981350476

        In [59]: rp1.max()
        Out[59]: 83.357268171136297

    """
    s_p1 = sc.ssim.recpost(1)[:,:3]
    p_p1 = sc.psim.recpost(1)[:,:3]

    rs = np.linalg.norm(s_p1, 2, 1)
    rp = np.linalg.norm(p_p1, 2, 1)

    log.info("rs %s %s " % (rs.min(), rs.max()))
    log.info("rp %s %s " % (rp.min(), rp.max()))

    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    ax.scatter3D(s_p1[sli,0], s_p1[sli,1], s_p1[sli,2] )

    ax.set_xlabel("X")
    ax.set_ylabel("Y")
    ax.set_zlabel("Z")

    ax.auto_scale_xyz( [-100,100], [-100,100], [-100,100] )




if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO)

    np.set_printoptions(precision=4, linewidth=200)

    plt.ion()
    plt.close()

    #intensity_plot() 
    #scattering_angle_plot() 

    
    step = .1
    #step = 10
    idom = np.arange(0,90,step)*deg 
    ylim = None

if 0:
    fig = plt.figure()
    ylim = [1e-6, 1e2]
    ax = fig.add_subplot(1,1,1)

if 1:
    spol,ppol = "5", "6"
    p_evt = Evt(tag=ppol, det="rainbow", label="P")
    s_evt = Evt(tag=spol, det="rainbow", label="S")

if 0:
    fig = plt.figure()
    fig.suptitle("Deviation angles without selection")
    ax = fig.add_subplot(1,1,1)
    scatter_plot(p_evt, s_evt)
    Scatter.combined_intensity_plot([1,2,3,4,5], ylim=ylim, scale=5e4, flip=False )

if 0:
    fig = plt.figure()
    fig.suptitle("Scatter plot split by mode, for R TT TRT TRRT ...")
    #ylim = [1e0,1e5] 
    ylim = None
    scatter_plot_split(p_evt, s_evt, pp=range(0,8), ylim=ylim)
    fig.subplots_adjust(hspace=0)

if 0:
    fig = plt.figure()
    p = 0
    fig.suptitle("Scatter angle for %s" % Scatter.seqhis_(p))
    ylim = [1e0,1e5] 
    #ylim = None
    sc = scatter_plot_component(p_evt, s_evt, p=p, ylim=ylim)
    fig.subplots_adjust(hspace=0)
     
    check_radius(sc,slice(None,None,100)) 

if 1:
    fig = plt.figure()
    p = 0
    fig.suptitle("Incident angle for %s compared to distribution without selection" % Scatter.seqhis_(p))
    xlim = [0,90]

    sc = Scatter(p, psim=p_evt, ssim=s_evt, not_=False)

    ax = fig.add_subplot(2,2,1)
    pci = sc.psim.incident_angle()
    ax.hist(np.arccos(pci)/deg, bins=100)
    ax.set_xlim(xlim)

    ax = fig.add_subplot(2,2,2)
    sci = sc.ssim.incident_angle()
    ax.hist(np.arccos(sci)/deg, bins=100)
    ax.set_xlim(xlim)


    ax = fig.add_subplot(2,2,3)
    pall = Selection(p_evt)
    pca = pall.incident_angle()
    ax.hist(np.arccos(pca)/deg, bins=100)
    ax.set_xlim(xlim)

    ax = fig.add_subplot(2,2,4)
    sall = Selection(s_evt)
    sca = sall.incident_angle()
    ax.hist(np.arccos(sca)/deg, bins=100)
    ax.set_xlim(xlim)
    




if 0:
    fig = plt.figure()
    fig.suptitle("Scatter contribs from modes 0:3")
    for i, p in enumerate(range(4)):
        sc = Scatter(p)
        print sc.table(idom)

        ax = fig.add_subplot(2,4,i+1)
        sc.intensity_plot(idom, ylim=None, flip=False, log_=False, mask=True)

        ax = fig.add_subplot(2,4,i+1+4)
        sc.intensity_plot(idom, ylim=ylim, flip=False, log_=True, mask=True)

        #ax = fig.add_subplot(3,4,i+1+8)
        #sc.intensity_plot(idom, ylim=ylim, flip=True, log_=True, mask=True)


if 0:
    #
    # as theta comes in from 180 both s and p are negative, they both go thru the 
    # discontinuiy at the bow angle before theta increases thru the positive lobe
    # ... the lobes are not symmetric 
    # 
    fig = plt.figure()
    fig.suptitle("Scatter intensity for bow 1")
    sc = Scatter(2)
    print sc.table(idom)
    ax = fig.add_subplot(1,1,1)
    idom = np.arange(0,90,1)*deg 
    tsp = sc.intensity(idom)
    Scatter._intensity_plot(tsp, ylim=None, flip=False, log_=True, mask=True)
    #ax.set_ylim([1e-4,10])
    #ax.set_yscale('log')
    d = np.dstack(tsp)
    print d 

    d[0,:,0] /= deg



