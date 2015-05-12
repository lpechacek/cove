#ifndef __VECTORIZER_H__
#define __VECTORIZER_H__

#ifndef USING_PCH
#include <QImage>
#include <QColor>
#endif /* USING_PCH */

#include "MapColor.H"

class ProgressObserver
{
  public:
    virtual ~ProgressObserver();
    virtual void percentageChanged(int percentage) = 0;
    virtual bool getCancelPressed() = 0;
};

class Vectorizer
{
  public:
    enum ColorSpace
    { COLSPC_RGB, COLSPC_HSV };
    enum LearningMethod 
    { KOHONEN_CLASSIC, KOHONEN_BATCH };
    enum AlphaStrategy
    { ALPHA_CLASSIC, ALPHA_NEUQUANT };
    enum PatternStrategy
    { PATTERN_RANDOM, PATTERN_NEUQUANT };
    enum MorphologicalOperation
    { EROSION, DILATION, THINNING_ROSENFELD, PRUNING };

  protected:
    enum VectorizerState 
    { NO_IMAGE, IMAGE, CLASSIFIED_IMAGE, BW_IMAGE, THINNED_IMAGE } state;
    QImage sourceImage;
    QImage classifiedImage;
    QImage bwImage;
    QImage thinnedBWImage;
    MapColor** sourceImageColors;
    MapColor* mc;
    int numberOfSourceImageColors, E;
    double initAlpha, q, minAlpha, p, quality;
    LearningMethod learnMethod;
    ColorSpace colorSpace;
    AlphaStrategy alphaStrategy;
    PatternStrategy patternStrategy;

    bool fixState(VectorizerState s, ProgressObserver* progressObserver = 0);
    
  public:
    Vectorizer();
    Vectorizer(QImage& im);
    virtual ~Vectorizer();
    virtual void setClassificationMethod(LearningMethod learnMethod);
    virtual void setColorSpace(ColorSpace colorSpace);
    virtual void setP(double p);
    virtual void setAlphaStrategy(AlphaStrategy alphaStrategy);
    virtual void setPatternStrategy(PatternStrategy patternStrategy);
    virtual void setInitAlpha(double initAlpha);
    virtual void setMinAlpha(double minAlpha);
    virtual void setQ(double q);
    virtual void setE(int E);
    virtual void setNumberOfColors(int nColors);
    virtual void setInitColors(QRgb* initColors);
    virtual bool performClassification(ProgressObserver* progressObserver = 0);
    QRgb* getClassifiedColors();
    int getNumberOfClassifiedColors ();
    virtual QImage getClassifiedImage(double* qualityPtr = 0,
	ProgressObserver* progressObserver = 0);
    virtual QImage getBWImage(bool* selectedColors,
	ProgressObserver* progressObserver = 0);
    virtual QImage getTransformedImage(MorphologicalOperation mo,
	ProgressObserver* progressObserver = 0);
    static QImage getTransformedImage(QImage bwImage, MorphologicalOperation mo,
	ProgressObserver* progressObserver = 0);
};

#endif
