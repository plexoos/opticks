Basic instructions how to build and run the opticks container:

```shell
cd opticks/
docker build -t opticks .
docker run --rm -it opticks
```

For development, you may want to mount the local project directory:

```shell
docker run --rm -it -v $PWD:/opticks opticks
```

Run tests:

```shell
docker run --gpus all --rm -it opticks ctest --test-dir /build/okconf
docker run --gpus all --rm -it opticks ctest --test-dir /build/sysrap
docker run --gpus all --rm -it opticks ctest --test-dir /build/boostrap
docker run --gpus all --rm -it opticks ctest --test-dir /build/npy
docker run --gpus all --rm -it opticks ctest --test-dir /build/optickscore

docker run --gpus all --rm -it opticks ctest --test-dir /build/CSG
docker run --gpus all --rm -it opticks ctest --test-dir /build/qudarap
docker run --gpus all --rm -it opticks ctest --test-dir /build/CSGOptiX

docker run --gpus all --rm -it opticks ctest --test-dir /build/ggeo
docker run --gpus all --rm -it opticks ctest --test-dir /build/gdxml
docker run --gpus all --rm -it opticks ctest --test-dir /build/u4
docker run --gpus all --rm -it opticks ctest --test-dir /build/g4cx
docker run --gpus all --rm -it opticks ctest --test-dir /build/opticksgeo
docker run --gpus all --rm -it opticks ctest --test-dir /build/extg4
docker run --gpus all --rm -it opticks ctest --test-dir /build/CSG_GGeo
docker run --gpus all --rm -it opticks ctest --test-dir /build/GeoChain
docker run --gpus all --rm -it opticks ctest --test-dir /build/GeoChain
```
