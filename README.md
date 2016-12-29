# CoVe
Contour line vectorizer for orienteering maps

This program was designed for contour line extraction from raster image 
orienteering maps and subsequent line vectorization.

## Building the software
Install Qt5 development environment and then build the application itself.

```
qmake -r
make
```

Run the app.
```
app/cove
```

## Using the software

* Start CoVe (app/cove)
* Load map raster image (Load image button)
* Check raster image resolution and possibly fix it
* Switch to Colors tab and run classification (button labeled Now)
* Choose colors corresponding to contour lines using the small colored buttons
* Switch to Thinning tab and prepare the image for vectorization by pressing
  Thin lines button
* Translate pixel into Bezier curves using Create vectors
* Store the resulting vectors into OCAD 6 file through Save vectors button

The file can be opened or imported into OCAD. The lines are drawn with symbol
990.0 (Vectorized lines). All fixing and symbol changing is then done in OCAD.
