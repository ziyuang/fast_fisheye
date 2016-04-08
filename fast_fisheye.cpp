#include <cmath>
#include <set>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <emscripten/emscripten.h>
#include <nanoflann.hpp>
#include <iostream>

using namespace emscripten;

// based on nanoflann/examples/pointcloud_example.cpp
struct Point
{
	double x,y;
};

struct FisheyeDistortion : Point {
	double z;
};

struct FisheyeIndexDistortionPair {
	size_t index;
	FisheyeDistortion distortion;
};

struct PointCloud
{
	std::vector<Point> pts;
	PointCloud(const std::vector<Point>& pts_): pts(pts_) {};

	// Must return the number of data points
	inline size_t kdtree_get_point_count() const { return pts.size();}

	// Returns the distance between the vector "p1[0:size-1]" and the data point with index "idx_p2" stored in the class:
	inline double kdtree_distance(const double *p1, const size_t idx_p2, size_t /*size*/) const
	{
		const double d0=p1[0]-pts[idx_p2].x;
		const double d1=p1[1]-pts[idx_p2].y;
		return d0*d0+d1*d1;
	}

	// Returns the dim'th component of the idx'th point in the class:
	// Since this is inlined and the "dim" argument is typically an immediate value, the
	//  "if/else's" are actually solved at compile time.
	inline double kdtree_get_pt(const size_t idx, int dim) const
	{
		if (dim==0) 
			return pts[idx].x;
		else 
			// dim==1
			return pts[idx].y;

	}

	// Optional bounding-box computation: return false to default to a standard bbox computation loop.
	//   Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
	//   Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)
	template <class BBOX>
	bool kdtree_get_bbox(BBOX& /* bb */) const { return false; }
};


typedef nanoflann::KDTreeSingleIndexAdaptor<
		nanoflann::L2_Simple_Adaptor<double, PointCloud > ,
		PointCloud,
		2 > KDTree;

class Fisheye {
private:
	PointCloud _pointCloud;
	KDTree _tree;
	double _radius, _k0, _k1;

	FisheyeDistortion distortPoint(const Point& p, const Point& focus) {
		FisheyeDistortion distorted;
		double dx = p.x - focus.x;
		double dy = p.y - focus.y;
		double dd = std::sqrt(dx*dx+dy*dy);
		if (!dd || dd >= _radius) {
			distorted.x = p.x;
			distorted.y = p.y;
			distorted.z = dd >= _radius? 1 : 10;
		}
		else {
			double k = _k0 * (1 - std::exp( - dd * _k1)) / dd * 0.75 + 0.25;
			distorted.x = focus.x + dx * k;
			distorted.y = focus.y + dy * k;
			distorted.z = std::fmin(k, 10);
		}
		return distorted;
	}

public:
	Fisheye(const std::vector<Point>& pts, double radius, double k0, double k1, size_t leaf_max_size)
		: _pointCloud(pts)
		, _tree(2, _pointCloud, nanoflann::KDTreeSingleIndexAdaptorParams(leaf_max_size))
		, _radius(radius)
		, _k0(k0)
		, _k1(k1) {

			// for (size_t i = 0; i < pts.size(); i++) {
			// 	std::cout << "pts[" << i << "] = (" << pts[i].x << ", " << pts[i].y << ")" << std::endl;
			// }
			// std::cout << "radius = " << _radius << std::endl;
			// std::cout << "k0 = " << _k0 << std::endl;
			// std::cout << "k1 = " << _k1 << std::endl;

			_tree.buildIndex();
		}

	std::vector<FisheyeIndexDistortionPair> distort(const Point& focus) {
		// std::cout << "focus = (" << focus.x << ", " << focus.y << ")" << std::endl;
		double cfocus[2] = {focus.x, focus.y};
		std::vector<std::pair<size_t, double> > kdtreeMatches;
		double factorSqrd = 3;

		size_t nMatches = _tree.radiusSearch(&cfocus[0], factorSqrd * _radius*_radius, kdtreeMatches, nanoflann::SearchParams());

		std::vector<FisheyeIndexDistortionPair> toDistort(nMatches);
		// std::cout << "Found " << nMatches << " neighbors" << std::endl;
		for (size_t i = 0; i < nMatches; i++) {
			FisheyeIndexDistortionPair toDistort_i;
			toDistort_i.index = kdtreeMatches[i].first;
			toDistort_i.distortion = distortPoint(_pointCloud.pts[toDistort_i.index], focus);
			// std::cout << "distortion: (" << toDistort_i.distortion.x << ", " << toDistort_i.distortion.y << ", " << toDistort_i.distortion.z << ")" << std::endl;
			toDistort[i] = toDistort_i;
		}

		return toDistort;
	}

};

EMSCRIPTEN_BINDINGS(FastFisheye) {
	value_object<Point>("Point")
	.field("x", &Point::x)
	.field("y", &Point::y);

	value_object<FisheyeDistortion>("FisheyeDistortion")
	.field("x", &FisheyeDistortion::x)
	.field("y", &FisheyeDistortion::y)
	.field("z", &FisheyeDistortion::z);

	register_vector<Point>("PointVec");

	value_object<FisheyeIndexDistortionPair>("FisheyeIndexDistortionPair")
	.field("index", &FisheyeIndexDistortionPair::index)
	.field("distortion", &FisheyeIndexDistortionPair::distortion);

	register_vector<FisheyeIndexDistortionPair>("FisheyeIndexDistortionPairVector");

	register_vector<size_t>("Indices");

	class_<Fisheye>("FastFisheye")
	.constructor<const std::vector<Point>&, double, double, double, size_t>()
	.function("distort", &Fisheye::distort);
}