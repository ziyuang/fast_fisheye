# Fast Fisheye
A faster fisheye plugin for d3, directly combined with _k_-d tree for nearest neighbors search.

## Build
1. Install and set up [emscripten](http://kripken.github.io/emscripten-site), to compile C++ code into JavaScript;
2. Get [nanoflann](https://github.com/jlblancoc/nanoflann), a header-only _k_-d tree implementation in C++;
3. `make` to generate `fast_fisheye.em.js` from `fast_fisheye.cpp`.

## Usage
Check `fast_random_circles.html`. Cartesian Distortion is untouched and not sped up.

## Issues
1. Not fast enough. I guess some NN search techniques for moving query (= current cursor position) should help. Leave it as future work.
2. Strangely much slower in Firefox.
