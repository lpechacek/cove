#include <fstream>
#include <iosfwd>
#include <iostream>
#include "OcadFileStructs.H"

using namespace std;

ostream& operator<<(ostream& s, const TCord& c)
{
  s << "[" << c.xCoord() << ", " << c.yCoord() << ", "
    << (c.isFirstCurvePoint()?"FirstCurvePoint ":"")
    << (c.isSecondCurvePoint()?"SecondCurvePoint ":"")
    << (c.isDoubleLineHasNoLeft()?"DoubleLineHasNoLeft ":"")
    << (c.isCornerPoint()?"CornerPoint ":"")
    << (c.isFirstHolePoint()?"FirstHolePoint ":"")
    << (c.isDoubleLineHasNoRight()?"DoubleLineHasNoRight ":"")
    << (c.isDashPoint()?"DashPoint ":"") << "]";
  return s;
}

int main(int argc, char** argv)
{
  if(argc < 1) { cerr << "usage: ocddump <file>" << endl; return 1; }
  ifstream fin(argv[1], ifstream::binary);
  TFileHeader h;
  fin.read(reinterpret_cast<char*>(&h), sizeof(h));

  cout << "HEADER -----------------------------" << endl;
  cout << "OCADMark: " << hex << h.OCADMark << dec << endl;
  if(h.OCADMark != 0x0cad) { cout << "invalid mark" << endl; return 2; }

  cout << "SectionMark: " << hex << h.SectionMark << dec << endl;
  cout << "Version: " << h.Version << "; Subversion: " << h.Subversion << endl;
  cout << "FirstSymBlk: " << hex << h.FirstSymBlk << endl;
  cout << "FirstIdxBlk: " << h.FirstIdxBlk << endl;
  cout << "SetupPos: " << h.SetupPos << dec << endl;
  cout << "SetupSize: " << h.SetupSize << endl;
  cout << "InfoPos: " << hex << h.InfoPos << dec << endl;
  cout << "InfoSize: " << h.InfoSize << endl;
  cout << "FirstStIndexBlk: " << hex << h.FirstStIndexBlk << dec << endl;

  uint32_t nextblock = h.FirstIdxBlk;
  do 
  {
    cout << "INDEXBLOCK "<<hex<<nextblock<<dec<<" --------------------" << endl;
    fin.seekg(nextblock);
    TIndexBlock ib;
    fin.read(reinterpret_cast<char*>(&ib), sizeof(ib));
    cout << "NextBlock: " << hex << ib.NextBlock << dec << endl;
    for(int j = 0; j < 256; j++)
    {
      cout << "INDEX - " << j << " -----------" << endl;
      cout << " Bounds: " << ib.IndexArr[j].LowerLeft << ib.IndexArr[j].UpperRight << endl;
      cout << " Pos: " << hex << ib.IndexArr[j].Pos << dec << endl;
      cout << " Len: " << ib.IndexArr[j].Len << endl;
      cout << " Sym: " << ib.IndexArr[j].Sym << endl;
      if(ib.IndexArr[j].Pos)
      {
//	cout << " ELEMENT -------------" << endl;
	fin.seekg(ib.IndexArr[j].Pos);
	TElement e;
	fin.read(reinterpret_cast<char*>(&e), ib.IndexArr[j].Len);
	cout << " Sym: " << e.Sym << endl;
	cout << " Otp: " << int(e.Otp) << " (";
	switch(e.Otp) {
	  case 1: cout << "point object"; break;
	  case 2: cout << "line or line text object"; break;
	  case 3: cout << "area object"; break;
	  case 4: cout << "unformatted text object"; break;
	  case 5: cout << "formatted text object or rectangle object"; break;
	  default: cout << "UNKNOWN";
	}
	cout << ")" << endl;
	cout << " nItem: " << e.nItem << endl;
	cout << " nText: " << e.nText << endl;
	cout << " Ang: " << e.Ang << endl;
	cout << " Sym: " << e.Sym << endl;
	cout << " Poly: ";
	for(int j = 0; j < e.nItem; j++)
	{
	  cout << e.Poly[j] << "; ";
	}
	cout << endl;
      }
    }
    nextblock = ib.NextBlock;
  }
  while(nextblock);

  return 0;
}
