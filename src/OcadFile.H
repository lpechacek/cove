#ifndef __OCADFILE_H__
#define __OCADFILE_H__

#ifndef USING_PCH
#include <string>
#endif /* USING_PCH */

#include "OcadFileStructs.H"
#include "Polygons.H"

class OcadFile
{
  private:
    std::string fname;
    std::string templFName;
    int resolution;
    static const int MAXPOLYPOINTS;
    
    OcadFile();
    void writeIndexBlock(const TIndexBlock& ib, std::ofstream& fout, int indexblockplace);

  public:
    OcadFile(const char* fname, int resolution, const char* templFName);
    void insertPolygons(const Polygons::PolygonList& polys, float xOff = 0, float yOff = 0);
};

#endif
