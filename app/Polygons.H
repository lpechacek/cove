#ifndef __POLYGONS_H__
#define __POLYGONS_H__

#ifndef USING_PCH
#include <vector>
#include <functional>
#endif /* USING_PCH */

extern "C" {
#include "curve.h"
#include "trace.h"
}

class QImage;
class QRect;
class ProgressObserver;

class Polygons
{
  protected:
    bool simpleonly;
    unsigned specklesize;
    float maxdist, distdirratio, alphamax, opttolerance;
    enum DIRECTION {NONE=0, NORTH, EAST, SOUTH, WEST};
    struct POLYGON_POINT { double x, y; unsigned char curve_control_point; bool corner; };
    struct PATH_POINT { int x, y; DIRECTION d; };

    class Path: public std::vector<PATH_POINT>
    {
      bool pathClosed;

      public:
      Path();
      void setClosed(bool closed);
      bool isClosed() const;
    };

    class PathList: public std::vector<Path>
    {
    };

  public:
    class Polygon: public std::vector<POLYGON_POINT>
    {
      bool polygonClosed;
      double minX, minY, maxX, maxY;

      public:
      Polygon();
      void push_back(const value_type& p);
      QRect boundingRect() const;
      void recheckBounds();
      void setClosed(bool closed);
      bool isClosed() const;
    };

    class PolygonList: public std::vector<Polygon>
    {
    };

  private:
    enum JOINEND
    { NOEND, FRONT, BACK };

    enum JOINTYPE
    { NOJOIN, FF, FB, BF, BB };

    struct JOINOP
    {
      float weight;
      JOINTYPE joinType;
      path_t *a, *b;
      bool simple, rescheduled;
      JOINOP (float weight, JOINTYPE joinType, path_t * a,
	  path_t * b):weight (weight), joinType (joinType), a (a), b (b),
		      simple (false), rescheduled (false)
      {
      }
    };

    typedef std::vector < JOINOP > JOINOPLIST;

    struct JOINENDPOINT
    {
      dpoint_t coords;
      JOINEND end;
      path_t *path;
      JOINENDPOINT (const dpoint_t & a, JOINEND b, path_t * c):coords (a),
							       end (b), path (c)
      {
      }
    };

    typedef std::vector < JOINENDPOINT > JOINENDPOINTLIST;

    struct greater_weight: public std::binary_function <const JOINOP &, const JOINOP &,
			   bool>
    {
      bool operator () (const JOINOP & x, const JOINOP & y)
      {
	return x.weight > y.weight;
      }
    };

    // implementation dependent value...
    static const int NPOINTS_MAX;

    bool compdists(JOINENDPOINTLIST & pl, JOINOPLIST & ops,
	const dpoint_t& min, const dpoint_t& max, 
	bool vertical, ProgressObserver * progressObserver,
	const double pBase, const double piece) const;
    bool splitlist(JOINENDPOINTLIST & pl, JOINOPLIST & ops,
	const dpoint_t& min, const dpoint_t& max, 
	bool vertical, ProgressObserver * progressObserver,
	const double pBase, const double piece) const;
    inline float distSqr (const dpoint_t * a, const dpoint_t * b) const;
    inline JOINEND joinEndA (JOINTYPE j) const;
    inline JOINEND joinEndB (JOINTYPE j) const;
    inline JOINEND oppositeEnd (JOINEND j) const;
    inline JOINTYPE endsToType (JOINEND ea, JOINEND eb) const;
    inline float dstfun (const dpoint_t * a, const dpoint_t * b, const dpoint_t * c, const dpoint_t * d) const;
    bool joinPolygons(path_t*& plist,
	ProgressObserver* progressObserver = 0) const;

  protected:
    static bool findNextPixel(const QImage& image, int &xp, int &yp);
    static void followPath(const QImage& image, int &x, int &y, Path* path = 0);
    static Path recordPath(const QImage& image, const int initX, const int initY);
    static void removePathFromImage(QImage& image, const Path& path);
    PathList decomposeImageIntoPaths(const QImage& sourceImage,
	ProgressObserver* progressObserver = 0) const;
    PolygonList getPathPolygons(const PathList& constpaths,
	ProgressObserver* progressObserver = 0) const;
    static float distance(const POLYGON_POINT& a, const POLYGON_POINT& b);

  public:
    Polygons();
    void setAlphaMax(double val);
    void setOptTolerance(double val);
    void setSpeckleSize(int val);
    void setMaxDistance(double val);
    void setSimpleOnly(bool val);
    void setDistDirRatio(double val);
    double alphaMax() const;
    double optTolerance() const;
    int speckleSize() const;
    double maxDistance() const;
    bool simpleOnly() const;
    double distDirRatio() const;
    PolygonList createPolygonsFromImage(const QImage& image,
	ProgressObserver* progressObserver = 0) const;
    // search for best value of NPOINTS_MAX - optinpoints.cpp 
    friend void perfcheck(void);
};

#endif
