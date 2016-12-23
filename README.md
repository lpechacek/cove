# CoVe
Contour line vectorizer for orienteering maps

This program was designed for contour line extraction from raster image 
orienteering maps and subsequent line vectorization.

## Building the software
Install Qt5 development environment, build libraries and then the applicatoin itself:

potrace line vectorization - library patched to handle loose ends
```
cd src/potrace
make
```

Generic raster image decomposition
```
cd ../libvectorizer/
qmake
make
```

CoVe application itself
```
cd ..
qmake
make
```

Run the app
```
./cove
```
