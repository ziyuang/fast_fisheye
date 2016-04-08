# Fast Fisheye
A faster fisheye plugin for d3, directly combined with _k_-d tree for faster nearest neighbors search.

## Build
1. Install and set up [emscripten](http://kripken.github.io/emscripten-site), to compile C++ code into JavaScript;
2. Get [nanoflann](https://github.com/jlblancoc/nanoflann), a header-only _k_-d tree implementation in C++;
3. `make` to generate `fast_fisheye.em.js` from `fast_fisheye.cpp`;
4. Optionally get [async.js](https://github.com/caolan/async), to distort the svg elements asynchronously.

## Usage
Check `fast_random_circles.html`. Cartesian Distortion is untouched and not sped up.

## Issues
1. Not fast enough. I guess some NN search techniques for moving object (= current cursor position) should help. Left as future works.
2. Particularly (and strangly) it is much slower in Firefox.