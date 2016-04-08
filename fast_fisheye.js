// based on d3-plugins/fisheye/fisheye.js
(function() {
  d3.fisheye = {
    scale: function(scaleType) {
      return d3_fisheye_scale(scaleType(), 3, 0);
    },
    circular: function() {
      var radius = 200,
          distortion = 2,
          leaf_max_size = 10,
          k0,
          k1,
          focus = {'x': 0, 'y': 0},
          x = function(p) {return p.x;},
          y = function(p) {return p.y;},
          nPts,
          fastFisheye;

      function fisheye(d) {
        var dx = d.x - focus.x,
            dy = d.y - focus.y,
            dd = Math.sqrt(dx * dx + dy * dy);
        if (!dd || dd >= radius) return {x: d.x, y: d.y, z: dd >= radius ? 1 : 10};
        var k = k0 * (1 - Math.exp(-dd * k1)) / dd * .75 + .25;
        return {x: focus.x + dx * k, y: focus.y + dy * k, z: Math.min(k, 10)};
      }

      function rescale() {
        k0 = Math.exp(distortion);
        k0 = k0 / (k0 - 1) * radius;
        k1 = distortion / radius;
        return fisheye;
      }

      fisheye.radius = function(_) {
        if (!arguments.length) return radius;
        radius = +_;
        return rescale();
      };

      fisheye.distortion = function(_) {
        if (!arguments.length) return distortion;
        distortion = +_;
        return rescale();
      };

      fisheye.focus = function(currentFocus) {
        if (!arguments.length) return focus;
        focus = {'x': currentFocus[0], 'y': currentFocus[1]};
        return fisheye;
      };

      fisheye.fast_distort_at = function(currentFocus, distortFunc) {
        currentFocus = {'x': currentFocus[0], 'y': currentFocus[1]};
        var dx = focus.x - currentFocus.x;
        var dy = focus.y - currentFocus.y;
        var eps = 5;
        var actual_radius = Math.sqrt(dx*dx+dy*dy) + radius + eps;
        var toDistort = fastFisheye.distort(currentFocus, actual_radius);

        for (var i = 0; i < toDistort.size(); i++) {
          var affected = toDistort.get(i);
          distortFunc(affected.index, affected.distortion);
        }
          
        focus = currentFocus;
      };

      fisheye.x = function(xAccessor) {
        if (!arguments.length) return x;
        x = xAccessor;
        return fisheye;
      }

      fisheye.y = function(yAccessor) {
        if (!arguments.length) return y;
        y = yAccessor;
        return fisheye;
      }

      fisheye.leaf_max_size = function(_) {
        if (!arguments.length) return leaf_max_size;
        leaf_max_size = +_;
        return fisheye;
      }

      fisheye.prepare = function(pts) {
        var pointVec = new Module.PointVec();
        nPts = pts.length;
        for (var i = 0; i < nPts; i++) {
          pointVec.push_back({'x': x(pts[i]), 'y': y(pts[i])});
        }
        fastFisheye = new Module.FastFisheye(pointVec, radius, k0, k1, leaf_max_size);
        return fisheye;
      }

      return rescale();
    }
  };

  function d3_fisheye_scale(scale, d, a) {

    function fisheye(_) {
      var x = scale(_),
          left = x < a,
          range = d3.extent(scale.range()),
          min = range[0],
          max = range[1],
          m = left ? a - min : max - a;
      if (m == 0) m = max - min;
      return (left ? -1 : 1) * m * (d + 1) / (d + (m / Math.abs(x - a))) + a;
    }

    fisheye.distortion = function(_) {
      if (!arguments.length) return d;
      d = +_;
      return fisheye;
    };

    fisheye.focus = function(_) {
      if (!arguments.length) return a;
      a = +_;
      return fisheye;
    };

    fisheye.copy = function() {
      return d3_fisheye_scale(scale.copy(), d, a);
    };

    fisheye.nice = scale.nice;
    fisheye.ticks = scale.ticks;
    fisheye.tickFormat = scale.tickFormat;
    return d3.rebind(fisheye, scale, "domain", "range");
  }
})();
