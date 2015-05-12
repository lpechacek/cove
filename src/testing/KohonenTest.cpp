//---------------------------------------------------------------------------

#include <math.h>
#include <signal.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#ifdef __LINUX__
#include "KohonenMap.H"
#endif

#ifdef __BORLANDC__
#include <vcl.h>
#include <conio.h>
#include "KohonenMap.H"
#pragma hdrstop
#endif

using namespace std;

#ifdef __BORLANDC__
void F(void) {
  cout << "--END OF RUN--" << endl;
  while(!kbhit());
  getch();
}

void Fs(int s) {
  cout << "--ABORT";
  F();
}
#endif

class IV: public vector<double>, public OrganizableElement {
  public: 
  virtual ~IV() {}

  virtual OrganizableElement* clone() const {
    return new IV(*this);
  }
  
  virtual double distance(const OrganizableElement *y) {
    double sum = 0;
    IV::const_iterator it1, it2;
    for(it1 = this->begin(), it2 = dynamic_cast<const IV *>(y)->begin();
	it1 != this->end(); it1++, it2++) {
      double diffr = (*it1-*it2);
      sum += diffr*diffr;
    }
    return sqrt(sum);
  }

  virtual void add(const OrganizableElement *y) {
    IV::iterator it1;
    IV::const_iterator it2;
    for(it1 = this->begin(), it2 = dynamic_cast<const IV *>(y)->begin(); it1 != this->end(); it1++, it2++)
      *it1 += *it2;
  }

  virtual void substract(const OrganizableElement *y) {
    IV::iterator it1;
    IV::const_iterator it2;
    for(it1 = this->begin(), it2 = dynamic_cast<const IV *>(y)->begin(); it1 != this->end(); it1++, it2++)
      *it1 -= *it2;
  }

  virtual void multiply(const double y) {
    IV::iterator it1;
    for(it1 = this->begin(); it1 != this->end(); it1++)
      *it1 *= y;
  }
};

class AG: public KohonenAlphaGetter {
  static const unsigned int E = 100000;
  double alpha, q;

  public:
  AG(): alpha(1), q(.5) {}
  virtual double getAlpha() {
    alpha *= q;
    cout << "alpha: " << alpha << endl;
    return (alpha>1e-6)?alpha:0;
  }

  virtual unsigned int getE() {
    return E;
  }
};

class PG: public BatchPatternGetter {
  int *patts;
  int npatts, cpatt;
  int *classes;
  IV retvect;

  PG() {}

  public:
  virtual ~PG() { delete [] classes; }
  PG(int *patts, int npatts): patts(patts), npatts(npatts), cpatt(-1) {
    retvect.push_back(0);
    classes = new int[npatts];
  }
  virtual const OrganizableElement *getPattern() {
    if(++cpatt >= npatts) return 0;
    retvect[0] = patts[cpatt];
    return &retvect;
  }
  virtual const int getLastElementClass() const
  {
    return classes[cpatt];
  }
  virtual const int getElementClass(int p) const
  {
    return classes[p];
  }
  virtual void setLastElementClass(int c)
  {
    classes[cpatt] = c;
  }
  virtual void reset()
  {
    cpatt=-1;
  }
};

//---------------------------------------------------------------------------

#ifdef __BORLANDC__
#pragma argsused
#endif
int main(int argc, char* argv[], char *envp[])
{
#ifdef __BORLANDC__
  atexit(F);
  signal(SIGABRT, &Fs);
#endif

  // code here

  #ifdef __BORLANDC__
  randomize();
  #else
  srandom(time(NULL));
  #endif

  int patterns[] = { 1,  2,  3,  6,  7,
                     8,  9, 10, 13, 14,
                    15, 20, 21, 22, 23};
  PG pg(patterns, sizeof(patterns)/sizeof(patterns[0]));
  AG ag;
  KohonenMap km(4);

  int numbclass = km.getNumberOfClasses();

  IV *initvect = new IV[numbclass];
  OrganizableElement** ivptrs = new OrganizableElement*[numbclass];
  for(int i=0; i<numbclass; i++) {
    initvect[i].push_back(15.0*rand()/RAND_MAX);
    ivptrs[i] = &initvect[i];
  }

  cout << "initializing vectors" << endl;
  for(int i=0; i<numbclass; i++) {
    cout << (i?",":"") << initvect[i][0];
  }
  cout << endl;
  km.setClasses(ivptrs);

  cout << "learning" << endl;
  km.performBatchLearning(pg);

  cout << "learning ended" << endl;

  delete [] initvect;

  IV **rivptrs = (IV **)km.getClasses();
  for(int i=0; i<numbclass; i++) {
    cout << (*(rivptrs[i]))[0] << ": ";
    for(int j=0; j<sizeof(patterns)/sizeof(patterns[0]); j++) {
      if(pg.getElementClass(j) == i) cout << patterns[j] << ",";
    }
    cout << endl;
  }

  cout << endl;
  return 0;
}
//---------------------------------------------------------------------------

