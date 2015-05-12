//---------------------------------------------------------------------------

#include <math.h>
#include <signal.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#ifdef __LINUX__
#include "KohonenMap.hpp"
#endif

#ifdef __BORLANDC__
#include <vcl.h>
#include <conio.h>
#include "KohonenMap.hpp"
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

class IV: public vector<double>, public InputVector {
  public: 
  virtual ~IV() {}

  virtual InputVector* clone() const {
    return new IV(*this);
  }
  
  virtual double distance(const InputVector *y) {
    double sum = 0;
    IV::const_iterator it1, it2;
    for(it1 = this->begin(), it2 = dynamic_cast<const IV *>(y)->begin();
	it1 != this->end(); it1++, it2++) {
      double diffr = (*it1-*it2);
      sum += diffr*diffr;
    }
    return sqrt(sum);
  }

  virtual void add(const InputVector *y) {
    IV::iterator it1;
    IV::const_iterator it2;
    for(it1 = this->begin(), it2 = dynamic_cast<const IV *>(y)->begin(); it1 != this->end(); it1++, it2++)
      *it1 += *it2;
  }

  virtual void substract(const InputVector *y) {
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

class PG: public KohonenPatternGetter {
  int *patts;
  int npatts;
  IV retvect;

  PG() {}

  public:
  virtual ~PG() {}
  PG(int *patts, int npatts): patts(patts), npatts(npatts) {
    retvect.push_back(0);
  }
  virtual const InputVector *getPattern() {
    retvect[0] = patts[rand()/(RAND_MAX/npatts)];
    return &retvect;
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
  PG pg(patterns, sizeof(patterns)/sizeof(int));
  AG ag;
  KohonenMap km(4);

  int numbclass = km.getNumberOfClasses();

  IV *initvect = new IV[numbclass];
  InputVector** ivptrs = new InputVector*[numbclass];
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
  km.performLearning(ag, pg);

  cout << "learning ended" << endl;

  delete [] initvect;

  IV **rivptrs = (IV **)km.getClasses();
  for(int i=0; i<numbclass; i++) {
    cout << (i?",":"") << (*(rivptrs[i]))[0];
  }

  cout << endl;
  return 0;
}
//---------------------------------------------------------------------------

