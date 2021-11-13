#!/usr/bin/env python
import os, numpy as np, logging
log = logging.getLogger(__name__)

class CSGFoundry(object):
    FOLD = os.path.expandvars("$TMP/CSG_GGeo/CSGFoundry")
    FMT = "   %10s : %20s  : %s "

    @classmethod
    def namelist_to_namedict(cls, namelist):
        return dict(zip(range(len(namelist)),list(map(str,namelist)))) 
 
    def __init__(self, fold=FOLD):
        self.load(fold)
        self.meshnamedict = self.namelist_to_namedict(self.meshname)
        self.bndnamedict = self.namelist_to_namedict(self.bndname)
        self.mokname = "zero one two three four five six seven eight nine".split()
        self.moknamedict = self.namelist_to_namedict(self.mokname)

    def load(self, fold):
        log.info("load %s " % fold)

        if not os.path.isdir(fold):
            log.fatal("CSGFoundry folder %s does not exist " % fold)
            log.fatal("create foundry folder from OPTICKS_KEY geocache with CSG_GGeo/run.sh " )
            assert 0 
        pass

        names = os.listdir(fold)
        stems = []
        for name in filter(lambda name:name.endswith(".npy") or name.endswith(".txt"), names):
            path = os.path.join(fold, name)
            stem = name[:-4]
            a = np.load(path) if name.endswith(".npy") else np.loadtxt(path, dtype=np.object)
            if name == "bnd.txt": stem = "bndname"  ## TODO: avoid clash of stems between bnd.npy and bnd.txt ?
            setattr(self, stem, a)
            stems.append(stem)
            #globals()[stem] = a 
            print(self.FMT % (stem, str(a.shape), path))
        pass
        self.stems = stems
        self.fold = fold

    def desc(self, stem):
        a = getattr(self, stem)
        ext = ".txt" if a.dtype == 'O' else ",npy"
        pstem = "bnd" if stem == "bndname" else stem 
        path = os.path.join(self.fold, "%s%s" % (pstem, ext))
        return self.FMT % (stem, str(a.shape), path) 

    def __repr__(self):
        return "\n".join(map(lambda stem:self.desc(stem),self.stems))

    def dump_node_boundary(self):
        logging.info("dump_node_boundary") 
        node = self.node
        bndname = self.bndname

        node_boundary = node.view(np.uint32)[:,1,2]
        ubs, ubs_count = np.unique(node_boundary, return_counts=True)

        for ub, ub_count in zip(ubs, ubs_count):
            print(" %4d : %6d : %s " % (ub, ub_count, bndname[ub]))
        pass 




if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO)
    cf = CSGFoundry()
    cf.dump_node_boundary()


