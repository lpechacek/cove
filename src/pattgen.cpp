#ifndef USING_PCH
#include <iostream>
#include <assert.h>
#include <string.h>
#endif /* USING_PCH */

using namespace std;

static const int NE = 0100;
static const int N  = 0200;
static const int NW = 0400;
static const int E  = 0010;
static const int C  = 0020;
static const int W  = 0040;
static const int SE = 0001;
static const int S  = 0002;
static const int SW = 0004;

class graph {
  static const bool iniMat[9][9];
  bool adjMat[9][9];
  int adjMatSize;
  public:
  graph() {
    for(int i=0;i<9;i++)
      for(int j=0;j<9;j++)
	adjMat[i][j] = 0;
    adjMatSize=0;
  }
  graph(int p) {
    new(this) graph();
    for(int i=0;i<9;i++)
    {
      if(p&0400)
      {
	for(int j=0;j<9;j++)
	{
	  adjMat[i][j] = iniMat[i][j];
	  adjMat[j][i] = iniMat[j][i];
	}
	adjMatSize++;
      }
      p <<= 1;
    }

    int upperBound = 9;
    for(int i=0;i<upperBound;i++)
    {
      if(!adjMat[i][i])
      {
	for(int j=0;j<9;j++)
	  for(int k=i+1;k<9;k++)
	    adjMat[j][k-1] = adjMat[j][k];

	for(int j=0;j<9;j++)
	  for(int k=i+1;k<9;k++)
	    adjMat[k-1][j] = adjMat[k][j];

	i--; upperBound--; adjMat[8][8] = 0;
      }
    }
  }

  void power() {
    bool tmpMat[9][9];
    for(int i=0;i<adjMatSize;i++)
    {
      for(int j=0;j<adjMatSize;j++)
      {
	tmpMat[i][j] = 0;
	for(int k=0;k<adjMatSize;k++)
	{
	  tmpMat[i][j] += adjMat[i][k] * adjMat[k][j];
	}
	tmpMat[i][j] = !!tmpMat[i][j];
      }
    }
    // adjMat = tmpMat;
    memcpy(adjMat, tmpMat, sizeof(tmpMat));
  }

  void print() const;

  bool operator==(const graph& that) const {
    for(int i=0;i<adjMatSize;i++)
      for(int j=0;j<adjMatSize;j++)
	if(this->adjMat[i][j] != that.adjMat[i][j])
	  return false;
    return true;
  }

  int nComponents() {
    graph g = *this;
    graph prev;
    do {
      prev = g;
      g.power();
    } while(!(prev == g));

    int nComp=0;
    for(int i=0;i<adjMatSize;i++)
    {
      if(g.adjMat[i][i])
      {
	for(int k=i+1;k<adjMatSize;k++)
	{
	  if(!memcmp(g.adjMat[i], g.adjMat[k], sizeof(bool)*adjMatSize))
	    g.adjMat[k][k] = 0;
	}
      }
    }

    for(int i=0;i<adjMatSize;i++)
      if(g.adjMat[i][i])
	nComp++;

    return nComp;
  }
};

const bool graph::iniMat[9][9] = {
  {1,1,0,1,0,0,0,0,0},
  {1,1,1,0,1,0,0,0,0},
  {0,1,1,0,0,1,0,0,0},
  {1,0,0,1,1,0,1,0,0},
  {0,1,0,1,1,1,0,1,0},
  {0,0,1,0,1,1,0,0,1},
  {0,0,0,1,0,0,1,1,0},
  {0,0,0,0,1,0,1,1,1},
  {0,0,0,0,0,1,0,1,1}};

void graph::print() const {
  for(int i=0;i<adjMatSize;i++)
  {
    for(int j=0;j<adjMatSize;j++)
    {
      cerr << static_cast<int>(adjMat[i][j]);
    }
    cerr << endl;
  }
}

bool isAllFG(int x, int m) { return (x&m) == m; }
bool isAnyFG(int x, int m) { return x&m; }
bool isAllBG(int x, int m) { return ((~x)&m) == m; }
bool isAnyBG(int x, int m) { return (~x)&m; }
bool hitMiss(int x, int f, int b) { return isAllFG(x,f)&&isAllBG(x,b); }

int rotate(int m)
{
  int ret = 0;
  int topline = (m&0700) >> 6;
  int midline = (m&0070) >> 3;
  int botline = (m&0007);
  ret |= ( (botline&4)     |((midline&4)>>1) |((topline&4)>>2))<<6;
  ret |= (((botline&2)<<1) | (midline&2)     |((topline&2)>>1))<<3;
  ret |= (((botline&1)<<2) |((midline&1)<<1) | (topline&1));
  return ret;
}

void printHitMiss(int f, int b) 
{
  for(int i=0;i<9;i++) {
    if(b&0400 && f&0400)
      cerr << '!';
    else if(b&0400)
      cerr << 'x';
    else if(f&0400)
      cerr << '@';
    else
      cerr << '.';
    if(i%3 == 2)
      cerr << endl;
    f <<= 1;
    b <<= 1;
  }
}

int connectivityChange(int p, bool insert)
{
  graph thisNeigh(p); // this window
  graph modNeigh(p&0757|(insert<<4)); // window with or without center pixel

  int tc = thisNeigh.nComponents();
  int mc = modNeigh.nComponents();

  cerr << "ncomp: " << tc << "/" << mc << endl;
  return (tc==mc)?0:((tc<mc)?1:-1);
}

int main(void)
{
  bool isDel[512];
  bool isIns[512];
  bool isPrun[512];

  int masksfg[] = {N|NE|E|SE|S|SW|W, N|NE|E|SE|S|SW|W|NW};
  int masksbg[] = {C|NW, C};

  // pixel is an endpoint, i.e. has only one 4-neighbor
  int endpointfg[] = {C|N};
  int endpointbg[] = {E|S|W};

  for(int p=0; p<512; p++)
  {
    printHitMiss(p,0);

    isDel[p] = true;
    isPrun[p] = false;

    if(isAllBG(p,C)) // is there a pixel?
    { cerr<<"No pixel"<<endl; isDel[p] = false; goto insertions; }

    if(isAllFG(p,N|E|S|W)) // is pixel erodable?
    { cerr<<"Nonerodable"<<endl; isDel[p] = false; goto insertions; }

    if(hitMiss(p,C,N|E|S|W)) // pixel is alone - may or may not be deleted
    { cerr<<"Alone"<<endl; isDel[p] = false; goto insertions; }

    // check for endpoint
    for(int neighCase=0; neighCase<sizeof(endpointfg)/sizeof(endpointfg[0]); neighCase++)
    {
      int maskfg = endpointfg[neighCase];
      int maskbg = endpointbg[neighCase];
      assert(!(maskbg&maskfg));
      for(int rot=0; rot<4; rot++)
      {
	if(hitMiss(p,maskfg,maskbg))
	{ isDel[p] = false; isPrun[p] = true; break; }

	maskfg = rotate(maskfg);
	maskbg = rotate(maskbg);
      }
      if(isDel[p] == false)
      {
	cerr<<"Endpoint - NONDELETABLE"<<endl;
	goto insertions;
      }
    }

/*    if(hitMiss(p,C,NE|SE|SW|NW)) // is an end point or provides connectivity
    { cerr<<"Endpoint/connect"<<endl; isDel[p] = false; goto insertions;}
*/
    if(connectivityChange(p, false) == 1) // test connectivity
    {
      cerr<<"Connectivity changed"<<endl;
      isDel[p] = false;
    }
    else
    {
      // safely deletable - does not break conectivity
      cerr<<"Connectivity unchanged >> DELETABLE"<<endl;
    }

insertions:
    isIns[p] = true;

    if(isAllFG(p,C)) // is there a background?
    { cerr<<"Pixel"<<endl; isIns[p] = false; goto finish;}

    if(isAllBG(p,N|E|S|W)) // is pixel dilatable?
    { cerr<<"Nondilatable"<<endl; isIns[p] = false; goto finish; }

/*    isIns[p] = false;
    for(int neighCase=0; neighCase<sizeof(masksfg)/sizeof(masksfg[0]); neighCase++)
    {
      int maskfg = masksfg[neighCase];
      int maskbg = masksbg[neighCase];
      assert(!(maskbg&maskfg));
      for(int rot=0; rot<3; rot++)
      {
	if(hitMiss(p,maskfg,maskbg))
	{ isIns[p] = true; break; }

	maskfg = rotate(maskfg);
	maskbg = rotate(maskbg);
      }
      if(isIns[p] == true)
      {
	cerr<<"Inner - INSERTABLE"<<endl;
	break;
      }
    }
   if(connectivityChange(p, true) == -1) // test connectivity
    {
      cerr<<"Connectivity changed"<<endl;
      isIns[p] = false;
    }
    else*/ 
    {
      cerr<<"Connectivity unchanged >> INSERTABLE"<<endl;
    }
finish: ;
  }
/*
  cerr <<"SUMMARY"<< endl;
  for(int p=0; p<512; p++)
    if(isDel[p]) {
      printHitMiss(p,0);
      cerr << endl;
    }
*/
  cout << "bool isDeletable[512] = {";
  for(int p=0; p<512; p++)
  {
    cout << (p?',':' ');
    if(!(p%8)) cout << endl;
    cout << (isDel[p]?"true":"false");
  }
  cout << "};" << endl << endl;

  cout << "bool isInsertable[512] = {";
  for(int p=0; p<512; p++)
  {
    cout << (p?',':' ');
    if(!(p%8)) cout << endl;
    cout << (isIns[p]?"true":"false");
  }
  cout << "};" << endl << endl;

  cout << "bool isPrunable[512] = {";
  for(int p=0; p<512; p++)
  {
    cout << (p?',':' ');
    if(!(p%8)) cout << endl;
    cout << (isPrun[p]?"true":"false");
  }
  cout << "};" << endl;
  return 0;
}

