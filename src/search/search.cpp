#ifndef USING_PCH
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <QApplication>
#include <QImage>
#include <QPainter>
#include <QString>
#endif /* USING_PCH */

#include "Vectorizer.H"
#include "Settings.H"

using std::cout;
using std::cerr;
using std::endl;

QRgb getColorFromImage(const QImage& image)
{
  srand(time(NULL));
  unsigned long r, g, b, divisor;
  r = g = b = divisor = 0;
  for(int a = 0; a < 5; a++)
  {
    int line = rand()/(RAND_MAX/image.height());
    int w = image.width();
    for(int x = 0; x < w; x++)
    {
      QRgb c = image.pixel(x, line);
      r += qRed(c);
      g += qGreen(c);
      b += qBlue(c);
      divisor++;
    }
  }
  return qRgb(r/divisor, g/divisor, b/divisor);
}

int main( int argc, char ** argv )
{
  // 1% pTab
  /*
     double pTab[] = {
     1.0000000e+000, 1.0202628e+000, 1.0410710e+000, 1.0624560e+000, 1.0844515e+000,
     1.1070936e+000, 1.1304210e+000, 1.1544757e+000, 1.1793029e+000, 1.2049516e+000,
     1.2314749e+000, 1.2589305e+000, 1.2873814e+000, 1.3168964e+000, 1.3475509e+000,
     1.3794274e+000, 1.4126172e+000, 1.4472209e+000, 1.4833498e+000, 1.5211278e+000,
     1.5606930e+000, 1.6021999e+000, 1.6458222e+000, 1.6917555e+000, 1.7402219e+000,
     1.7914738e+000, 1.8458002e+000, 1.9035333e+000, 1.9650578e+000, 2.0308213e+000,
     2.1013491e+000, 2.1772617e+000, 2.2592985e+000, 2.3483491e+000, 2.4454947e+000,
     2.5520644e+000, 2.6697140e+000, 2.8005369e+000, 2.9472251e+000, 3.1133087e+000,
     3.3035248e+000, 3.5244057e+000, 3.7852607e+000, 4.0999051e+000, 4.4899192e+000,
     4.9913432e+000, 5.6701206e+000, 6.6640754e+000, 8.3303839e+000, 1.2089437e+001, INFINITY };
   */
  // 5% pTab
  double pTab[] = {
    1.0000000e+000, 1.1070936e+000, 1.2314749e+000, 1.3794274e+000, 1.5606930e+000,
    1.7914738e+000, 2.1013491e+000, 2.5520644e+000, 3.3035248e+000, 4.9913432e+000, INFINITY };
  QApplication a(argc, argv);
  Settings settings;
  if(!settings.load("sampleSettings.xml"))
  {
    cerr << "settings loading failed" << endl;
    return 1;
  }
  int nClasses = settings.getInt("nColors");
  int nInitColors;
  QRgb* initColors = 0, *optimalColors = 0;
  cout << nClasses << " colors" << endl;
  QImage sourceImage("samplePicture.png");
  if(sourceImage.isNull())
  {
    cerr << "picture loading failed" << endl;
    return 1;
  }
  QImage classImage;
  double quality, bestquality;
  Vectorizer v(sourceImage);
  switch(settings.getInt("learnMethod"))
  {
    case 0: v.setClassificationMethod(Vectorizer::KOHONEN_CLASSIC);
	    cout << "Kohonen learning" << endl;
	    break;
    case 1: v.setClassificationMethod(Vectorizer::KOHONEN_BATCH);
	    cout << "Batch learning" << endl;
	    break;
  }
  cout << "Color space ";
  switch(settings.getInt("colorSpace"))
  {
    case 0: cout << "RGB" << endl; break;
    case 1: cout << "HSV" << endl; break;
  }
  switch(settings.getInt("initColorsSource"))
  {
    case 0: cout << "Random initial colors" << endl; break;
    case 1: cout << "Random initial colors from image" << endl;
	    initColors = new QRgb[nClasses];
	    break;
    case 2: cout << "Predefined initial colors" << endl;
	    initColors = settings.getInitColors(&nInitColors);
	    if(nClasses != nInitColors)
	    {
	      cout << "The number of predefined colors ("
		<< nInitColors << ") does not match"
		" the number of colors (" << nClasses << ")" << endl;
	      return 1;
	    }
	    break;
  }
  v.setNumberOfColors(nClasses);
  int nAttempts = settings.getInt("numberOfLearningAttempts");
  cout << nAttempts << " learning cycles for every value of p" << endl;
  for(unsigned k = 0; k < sizeof(pTab)/sizeof(pTab[0]); k++)
  {
    double p = pTab[k];
    //QImageView* q = new QImageView();
    //q->show();
    cout << "Running with p = " << p << std::endl;
    bestquality = +INFINITY;
    for(int run = 0; run < nAttempts; run++)
    {
      v.setP(p);
      switch(settings.getInt("colorSpace"))
      {
	case 0: v.setColorSpace(Vectorizer::COLSPC_RGB);
		break;
	case 1: v.setColorSpace(Vectorizer::COLSPC_HSV);
		break;
      }
      if(settings.getInt("initColorsSource") == 1)
      {
	cout << "Retrieving initial colors from image" << endl;
	for(int i = 0; i < nClasses; i++)
	  initColors[i] = getColorFromImage(sourceImage);
      }
      v.setInitColors(initColors);
      if(settings.getInt("learnMethod") == 0)
      {
	switch(settings.getInt("alphaStrategy"))
	{
	  case 0: v.setAlphaStrategy(Vectorizer::ALPHA_CLASSIC);
		  break;
	  case 1: v.setAlphaStrategy(Vectorizer::ALPHA_NEUQUANT);
		  break;
	}
	switch(settings.getInt("patternStrategy"))
	{
	  case 0: v.setPatternStrategy(Vectorizer::PATTERN_RANDOM);
		  break;
	  case 1: v.setPatternStrategy(Vectorizer::PATTERN_NEUQUANT);
		  break;
	}
	v.setInitAlpha(settings.getDouble("initAlpha"));
	v.setMinAlpha(settings.getDouble("minAlpha"));
	v.setQ(settings.getDouble("q"));
	v.setE(settings.getInt("E"));
      }

      v.performClassification(0);

      QImage newImage = v.getClassifiedImage(&quality);
      cout << "Finished run " << run << ", quality: " << quality << endl;
      cout.flush();
      if(quality < bestquality)
      {
	bestquality = quality;
	classImage = newImage;
	cout << "Better image" << std::endl;

	QRgb *colorsFound = v.getClassifiedColors();
	int n = v.getNumberOfClassifiedColors();
	cout << n << " colors: ";
	for(int i = 0; i < n; i++)
	{
	  if(i) cout << ',';
	  cout << "["
	    << qRed(colorsFound[i]) << ","
	    << qGreen(colorsFound[i]) << ","
	    << qBlue(colorsFound[i]) << "]";
	}   
	cout << endl;
	if(optimalColors) { delete optimalColors; optimalColors = 0; }
	optimalColors = colorsFound;
	//q->setImage(const_cast<const QImage*>(&classImage));
      }
      //qApp->processEvents();    
    }
    //q->setImage(const_cast<const QImage*>(new QImage(classImage)));
    //qApp->processEvents();    
    const unsigned CSH = 20;
    const unsigned TAH = 80;
    QImage annotatedImage(classImage.size().width(),
	classImage.size().height()+CSH+TAH, QImage::Format_RGB32);
    QPainter aip(&annotatedImage);
    aip.drawImage(0,0,classImage);
    aip.fillRect(0, classImage.size().height(), classImage.size().width(),
	CSH+TAH, Qt::white);
    double rwidth = classImage.size().width()/nClasses;
    QImage textarea(TAH, int(ceil(rwidth))+2, QImage::Format_RGB32);
    QPainter tap(&textarea);
    for(int i = 0; i < nClasses; i++)
    {
      QRect rect(int(i*rwidth), classImage.size().height(), int(ceil(rwidth))+2, CSH);
      aip.fillRect(rect, QColor(optimalColors[i]));
      tap.fillRect(textarea.rect(), Qt::white);
      tap.setPen(QColor(Qt::black));
      tap.drawText(textarea.rect(),
	  Qt::AlignVCenter|Qt::AlignLeft, QString("[%1,%2,%3]")
	    .arg(qRed(optimalColors[i]))
	    .arg(qGreen(optimalColors[i]))
	    .arg(qBlue(optimalColors[i])));
      aip.drawImage(int(i*rwidth), classImage.size().height()+CSH,
	  textarea.transformed(QMatrix(0,1,-1,0,0,0)));
    }
    if(annotatedImage.save(QString("sample%1.png").arg(p, 7, 'f', 5), "PNG"))
      cout << "Image saved succesfully" << std::endl;
    else
      cout << "!!!Problem saving image" << std::endl;
    classImage = QImage();
    cout << std::endl;
    if(optimalColors) { delete optimalColors; optimalColors = 0; }
  }

  return 0;
}
