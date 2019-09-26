#!/usr/bin/env python
"""
envg4.py
===========

Used by g4-export-ini to write Geant4 data config
which can easily adapted to working from changed 
installation prefixes.

See:: 

   CG4::preinit
   OpticksResource::ReadIniEnvironment
   BEnv::setEnvironment

"""
import os, logging, argparse
from collections import OrderedDict as odict 

class EnvG4(object):
    """
    Collects envvars starting with G4 into a dict and 
    modifies the values, which are required to start with the
    expanded OPTICKS_INSTALL_PREFIX path to use a token. 
    
    Replacing the hard coded install prefix with a token, 
    allows mobility for the data.
    """
    TOKEN = "OPTICKS_INSTALL_PREFIX"
    def __init__(self, prefix, token, export=False):
        d = odict()
        for k,v in filter(lambda kv:kv[0].startswith("G4"), os.environ.items()):
            assert v.startswith(prefix), (v, prefix)
            if token != "ASIS":
                v = "$" + token + "/" + v[len(prefix)+1:]
            pass
            if export:
                k = "export %s" % k
            pass
            #print("%s=%s" % (k,v))   
            d[k] = v 
        pass
        self.d = d 
    def __str__(self):
        return "\n".join(["%s=%s" % (k,v) for k,v in self.d.items()])  

if __name__ == '__main__':

    parser = argparse.ArgumentParser(__doc__)

    token = EnvG4.TOKEN
    parser.add_argument(     "--prefix", default=os.environ[token], help="Directory prefix to be replaced with token" )
    parser.add_argument(     "--token",  default="%s" % token )
    parser.add_argument(     "--export",  action="store_true", default=False )
    parser.add_argument(     "--level", default="info", help="logging level" ) 
    args = parser.parse_args()

    fmt = '[%(asctime)s] p%(process)s {%(pathname)s:%(lineno)d} %(levelname)s - %(message)s'
    logging.basicConfig(level=getattr(logging,args.level.upper()), format=fmt)

    envg4 = EnvG4(args.prefix,args.token, args.export) 
    print(envg4) 


