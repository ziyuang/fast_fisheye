# Fast Fisheye
A faster fisheye plugin for d3, directly combined with _k_-d tree for faster nearest neighbors search.

## Build
1. Install and set up [emscripten](http://kripken.github.io/emscripten-site), to compile C++ code into JavaScript;
2. Download [nanoflann](https://github.com/jlblancoc/nanoflann), a header-only _k_-d tree implementation in C++;
3. `make` to generate `fast_fisheye.em.js` from `fast_fisheye.cpp`;

## Usage
Check `fast_random_circles.html`. Cartesian Distortion is untouched and not sped up.

## Issues
1. You cannot move your cursor too fast. The original fisheye plugin runs through the whole point set and sets the distortion for each point. But this one only runs through a part of the points, and if one distorted point is too far away from the current cursor position, it won't be resetted.
2. It is not fast enough...I guess some NN search techniques for moving object (= current cursor position) should help. Left as future works.
