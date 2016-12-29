#ifndef USING_PCH
#include <assert.h>
#include <limits.h>
#include <fstream>
#endif /* USING_PCH */

#include "OcadFile.H"

using namespace std;

uint8_t cove_sym_icon[264] = {
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,
  0xFF,0xFF,0xFF,0xFF,0xF5,0xDF,0xFF,0xF5,0x55,0x55,0x55,0x00,
  0xFF,0xFF,0xFF,0xFF,0xD5,0x5F,0xFF,0xF5,0x55,0x55,0x55,0x00,
  0xFF,0xFF,0xFF,0xFF,0xD5,0x5D,0xFF,0xF5,0xDF,0xFF,0xFF,0x00,
  0xFF,0xFF,0xFF,0xFF,0x5D,0x55,0xFF,0xF5,0xDF,0xFF,0xFF,0x00,
  0xFF,0xFF,0xFF,0xFD,0x5F,0xD5,0xDF,0xF5,0x55,0x55,0x5F,0x00,
  0xFF,0xFF,0xFF,0xF5,0x5F,0xF5,0xDF,0xF5,0xD5,0x55,0xDF,0x00,
  0xFF,0xFF,0xFF,0xF5,0xDF,0xFD,0x5F,0xF5,0xDF,0xFF,0xFF,0x00,
  0xFF,0xFF,0xFF,0xD5,0xDF,0xFD,0x5F,0xF5,0xDF,0xFF,0xFF,0x00,
  0xFF,0xF5,0xD5,0xD5,0xDF,0xFD,0x5D,0xD5,0x55,0x55,0x55,0x00,
  0xFF,0xD5,0x55,0x55,0xDF,0xF5,0x55,0x55,0x55,0x55,0x5D,0x00,
  0xFD,0x5F,0xFF,0xD5,0xDF,0xD5,0xFF,0xFD,0x5F,0xFF,0xFF,0x00,
  0xD5,0xFF,0xFF,0xFF,0xFF,0x5F,0xFF,0xFF,0x5D,0xFF,0xFF,0x00,
  0xD5,0xFF,0xFF,0xFF,0xFD,0x5F,0xFF,0xFF,0xD5,0xFF,0xFF,0x00,
  0xD5,0xFF,0xFF,0xFF,0xFD,0x5F,0xFF,0xFF,0xD5,0xFF,0xFF,0x00,
  0xD5,0xFF,0xFF,0xFF,0xFD,0x5F,0xFF,0xFF,0x55,0xFF,0xFF,0x00,
  0xD5,0xDF,0xFF,0xFF,0xFF,0x5D,0xFF,0xF7,0x5D,0xFF,0xFF,0x00,
  0xF5,0x5D,0xFF,0xFD,0x5F,0xD5,0xD7,0x7D,0x5D,0xFF,0xFF,0x00,
  0xFF,0x55,0x55,0x55,0xDF,0xF5,0x55,0x55,0xDF,0xFF,0xFF,0x00,
  0xFF,0xFF,0x5D,0x5D,0xFF,0xFF,0x55,0xDF,0xFF,0xFF,0xFF,0x00,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00 };


/*! \class OcadFile
  \brief Actually a wrapper for insertPolygons method.
 */

/*! \var OcadFile::fname
  Filename of the .OCD file.
  */

/*! \var OcadFile::templFName
  Filename of the template file.
  */

/*! \var OcadFile::resolution
  Resolution of the original image.  Used when transforming coordinates.
  */

/*! Constant holding maximum number of polygon points in the TElement
  structure.  Ocad version 6. */
const int OcadFile::MAXPOLYPOINTS = 2000;

/*! Constructor. 
   \param[in] fname Filename of the resulting file.
   \param[in] resolution Resolution of the original bitmap.
   \param[in] templFName Filename of the template raster image.
 */
OcadFile::OcadFile(const char* fname, int resolution, const char* templFName)
{
  this->fname = fname;
  this->templFName = templFName;
  this->resolution = resolution;
}

/*! Private method writing one TIndexBlock into file. 
 \param[in] ib IndexBlock to write.
 \param[in] fout OuputStream to write to.
 \param[in] indexblockplace Position where to write the indexblock. The file
            pointer is restored into its original state.*/
void
OcadFile::writeIndexBlock(const TIndexBlock& ib, ofstream& fout, int indexblockplace)
{
  int fp = fout.tellp();
  fout.seekp(indexblockplace, ofstream::beg);
  fout.write(reinterpret_cast<const char*>(&ib), sizeof(ib));
  fout.seekp(fp, ofstream::beg);
}

/*! \brief Writes given PolygonList into file.
  The file gets truncated before write, so that this method cannot be used for
  multiple insertions into the same file. The previous polygons saved into the
  file get lost with each call to this method. 
 \param[in] polys PolygonList to write.
 \param[in] xOff X-offset by which the polygons will be moved
	   (used for centering drawing onto template).
 \param[in] yOff Y-offset. See \a xOff.
 */
void
OcadFile::insertPolygons(const Polygons::PolygonList& polys, float xOff, float yOff)
{
  ofstream fout(fname.c_str(), ofstream::binary|ofstream::trunc);

  TFileHeader h;
  memset(&h, 0, sizeof(h)); // all values default to zero
  h.OCADMark = 0x0cad;
  h.Version = 6;

  TStp s;
  memset(&s, 0, sizeof(s)); // all values default to zero
  s.WorkMode = 11; // edit point
  s.LineMode = 6; // straight
  s.EditMode = 11; // edit point
  s.ActSym = 9900;
  s.MapScale = 15000;
  s.DraftScaleX = 15000;
  s.DraftScaleY = 15000;
  // s.TempOffset = #is zero...
  s.TemplateFileName = templFName.c_str();
  s.TemplateEnabled = 1;
  s.TempResol = resolution;
  // s.rTempAng  = #is zero...

  TSymHeader sh;
  memset(&sh, 0, sizeof(sh)); // all values default to zero
  sh.nColors = 1;
  sh.aColorInfo[0].Color.magenta = 200;
  sh.aColorInfo[0].ColorName = "Vectorized lines";
  // color does not appear in any separation
  memset(&sh.aColorInfo[0].SepPercentage, 255, sizeof(sh.aColorInfo[0].SepPercentage));
    
  TSymbolBlock sb;
  memset(&sb, 0, sizeof(sb)); // all values default to zero

  TLineSym ls;
  memset(&ls, 0, sizeof(ls)); // all values default to zero
  ls.Size = sizeof(ls);
  ls.Sym = 9900;
  ls.Otp = 2;
  ls.Cols.set(0, true);
  ls.Description = "Vectorized lines";
  ls.LineWidth = 7;
  memcpy(ls.IconBits, cove_sym_icon,  sizeof(ls.IconBits));

  int indexblockplace, elementindex = 0;
  TIndexBlock ib;
  memset(&ib, 0, sizeof(ib)); // all values default to zero

  TElement e;
  memset(&e, 0, sizeof(e)); // all values default to zero
  e.Sym = 9900; // Symbol 990.0
  e.Otp = 2; // Object type: Line
  
  // skip the place where file header will be
  fout.seekp(sizeof(h), ofstream::cur);
  // write so called "Symbol Header" that is nothing more than colors definition
  fout.write(reinterpret_cast<const char*>(&sh), sizeof(sh));
  // fill the line symbol place into symbol block structure
  h.FirstSymBlk = fout.tellp();
  sb.FilePos[0] = sizeof(sb)+fout.tellp();
  fout.write(reinterpret_cast<const char*>(&sb), sizeof(sb));
  // write the line symbol
  fout.write(reinterpret_cast<const char*>(&ls), sizeof(ls));
  // write the setup record
  h.SetupPos = fout.tellp();
  h.SetupSize = 0x438; //size of OCAD6 setup record (sorry for hardcoding) #sizeof(s);
  fout.write(reinterpret_cast<const char*>(&s), 0x438);
  // remember the place where first index block will be
  h.FirstIdxBlk = indexblockplace = fout.tellp();
  // return to the beginning and write the file header
  fout.seekp(0, ofstream::beg);
  fout.write(reinterpret_cast<const char*>(&h), sizeof(h));
  // skip the place where the index block will be (we will write it later)
  fout.seekp(indexblockplace+sizeof(ib), ofstream::beg);
  
  for(Polygons::PolygonList::const_iterator i = polys.begin(); i != polys.end(); i++)
  {
    Polygons::Polygon::const_iterator j = i->begin();
    int pointsremaining = i->size();
    while(pointsremaining)
    {
      // number of points in this polygon
      if(pointsremaining > MAXPOLYPOINTS)
	e.nItem = MAXPOLYPOINTS;
      else 
      {
	if(pointsremaining == MAXPOLYPOINTS)
	  e.nItem = MAXPOLYPOINTS;
	else
	  e.nItem = pointsremaining;
      }
      pointsremaining -= e.nItem;
      if(j != i->begin()) 
      {
	j--;
	e.nItem++;
      }
      int lowLeftX = INT_MAX, lowLeftY = INT_MAX,
	  upperRightX = INT_MIN, upperRightY = INT_MIN, pointIndex = 0;
      for(; j != i->end() && pointIndex < e.nItem; j++)
      {
	// OCAD coordinate system has y axis oriented upwards => y = -j->y
	int x = static_cast<int>((j->x+xOff)*2540/resolution), 
	    y = static_cast<int>(-(j->y+yOff)*2540/resolution);
	if(x < lowLeftX) lowLeftX = x;
	if(y < lowLeftY) lowLeftY = y;
	if(x > upperRightX) upperRightX = x;
	if(y > upperRightY) upperRightY = y;
	e.Poly[pointIndex].setXCoord(x);
	e.Poly[pointIndex].setYCoord(y);
	e.Poly[pointIndex].setFirstCurvePoint(j->curve_control_point == 1);
	e.Poly[pointIndex].setSecondCurvePoint(j->curve_control_point == 2);
	e.Poly[pointIndex].setCornerPoint(j->corner);
	pointIndex++;
      }
      TIndex& curIndex = ib.IndexArr[elementindex++];
      curIndex.LowerLeft.setXCoord(lowLeftX);
      curIndex.LowerLeft.setYCoord(lowLeftY);
      curIndex.UpperRight.setXCoord(upperRightX);
      curIndex.UpperRight.setYCoord(upperRightY);
      curIndex.Pos = fout.tellp();
      curIndex.Len = sizeof(e)-sizeof(e.Poly)+e.nItem*sizeof(TCord);
      curIndex.Sym = e.Sym; // why is this information doubled? (undeletion of objects?)
      fout.write(reinterpret_cast<const char*>(&e), curIndex.Len);
      if(elementindex > 255 && i != polys.end()-1) // we need a new index block
      {                             // in case there this polygon was not the last one
	ib.NextBlock = fout.tellp();
	writeIndexBlock(ib, fout, indexblockplace);
	indexblockplace = ib.NextBlock;
	fout.seekp(indexblockplace+sizeof(ib), ofstream::beg);
	elementindex = 0;
	memset(&ib, 0, sizeof(ib));
      }
    }
  }
  // clean up the last block
  writeIndexBlock(ib, fout, indexblockplace);
  fout.close();
}
