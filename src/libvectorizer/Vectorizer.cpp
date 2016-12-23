#include "Vectorizer.H"
#include "MapColor.H"
#include "AlphaGetter.H"
#include "PatternGetter.H"
#include "Morphology.H"

//@{
/*! \defgroup libvectorizer Library for color classification and BW morphology */

/*! \class ProgressObserver 
 * \ingroup libvectorizer
 * \brief Anyone who wants to monitor operation's progress must implement this
 * interface. 
 * 
 * This class provides interface for classes that would like to monitor
 * progress of long lasting operations like
 * Vectorizer::performClassification(ProgressObserver*).  The \a percentageChanged(int)
 * method is called occasionally thtough the progress of the work and \a
 * getCancelPressed() is called to obtain information whether the process
 * should continue or not. The class implementing ProgressObserver will usually
 * be something like a progress dialog with progress bar and a cancel button.
 */

/*! \fn virtual void ProgressObserver::percentageChanged(int percentage) = 0;
 * This method is called occasionally with \a percentage equal toratio of work
 * done. The \a percentage is between 0 and 100 inclusive.
 * \param[in] percentage The amount of work done. 
 */

/*! \fn virtual bool ProgressObserver::getCancelPressed() = 0;
 * Returns the boolean value indicating whether the work should progress or
 * finish as soon as possible. Once the ProgressObserver returns true it must
 * return true on all subsequent calls.
 * \return Value indicating whether the process should continue (false) or abort (true).
 */

/*! Destroys ProgressObserver object.
 */
ProgressObserver::~ProgressObserver()
{
}

/*! \class Vectorizer
 * \ingroup libvectorizer
 * \brief Facade for vectorization process. 
 *
 * This class provides facade for underlaying classes MapColor (and its derivates),
 * KohonenMap, Morphology and several others. The usual way how to cope with this class is
 * - construct Vectorizer with an QImage (Vectorizer(QImage& im))
 * - set number of recognized colors, learning method and parameters (or leave
 *   them untouched when you are happy with them)
 * - call performClassification()
 * - get the resulting colors with getClassifiedColors()
 * - obtain the classified image with getClassifiedImage()
 * - after selecting the colors for separation call getBWImage()
 * - and in case of need thin the lines in the image with getThinnedImage()
 */

/*! 
 * \enum Vectorizer::ColorSpace
 * Type of color space used.
 */
/*! \var Vectorizer::ColorSpace Vectorizer::COLSPC_RGB
 * Colors are from RGB space.
 */
/*! \var Vectorizer::ColorSpace Vectorizer::COLSPC_HSV
 * Colors are from HSV space.
 */
/*! \enum Vectorizer::LearningMethod 
 * Learning method to be used.
 */
/*! \var Vectorizer::LearningMethod Vectorizer::KOHONEN_CLASSIC
 * Classic Kohonen learning with random pattern selection and stepwise lowered
 * speed of learning.
 */
/*! \var Vectorizer::LearningMethod Vectorizer::KOHONEN_BATCH
 * Kohonens' batch learning equivalent to ISODATA.
 */ 
/*! \enum Vectorizer::AlphaStrategy 
 Possible alpha selection strategies for KOHONEN_CLASSIC learning. */
/*! \var Vectorizer::AlphaStrategy Vectorizer::ALPHA_CLASSIC 
 Alpha starts at initAlpha, does E cycles of learning, gets multiplied by q and
 does and returns to the previous step (E cycles) until it falls bellow
 minAlpha. \sa ClassicAlphaGetter */
/*! \var Vectorizer::AlphaStrategy Vectorizer::ALPHA_NEUQUANT 
 Not implemented yet. */

/*! \enum Vectorizer::PatternStrategy 
 Selection of pixels from source image. */
/*! \var Vectorizer::PatternStrategy Vectorizer::PATTERN_RANDOM 
 Random selection, used with KOHONEN_CLASSIC learning. */
/*! \var Vectorizer::PatternStrategy Vectorizer::PATTERN_NEUQUANT 
 Not implemented yet. */
 
/*! \enum Vectorizer::MorphologicalOperation 
 Enum of different possible morphological operations. */
/*! \var Vectorizer::MorphologicalOperation Vectorizer::EROSION 
 *  Perform erosion on BW image.*/
/*! \var Vectorizer::MorphologicalOperation Vectorizer::DILATION
 *  Perform dilation on BW image. */
/*! \var Vectorizer::MorphologicalOperation Vectorizer::THINNING_ROSENFELD
 *  Perform thinning on BW image.  */
/*! \var Vectorizer::MorphologicalOperation Vectorizer::PRUNING
 *  Perform pruning on BW image, i.e. line endpoint removal.  */

/*! \enum Vectorizer::VectorizerState
  Enum of different facade states. */
/*! \var Vectorizer::VectorizerState Vectorizer::NO_IMAGE
   No image has been set.  This state cannot be skipped, use Vectorizer(QImage& im) constructor. */
/*! \var Vectorizer::VectorizerState Vectorizer::IMAGE 
   Source image gets classified by running performClassification, then proceeds to CLASSIFIED_IMAGE. */
/*! \var Vectorizer::VectorizerState Vectorizer::CLASSIFIED_IMAGE
   Classified image has been created. This state cannot be skipped, use getBWImage to create an BW image. */
/*! \var Vectorizer::VectorizerState Vectorizer::BW_IMAGE
   BW Image gets thinned and proceeds to THINNED_IMAGE. */
/*! \var Vectorizer::VectorizerState Vectorizer::THINNED_IMAGE 
   Final state. */
/*! \var Vectorizer::VectorizerState Vectorizer::state
   Data member holding the state of this facade.  */


/*! \var QImage Vectorizer::sourceImage 
 Source image to be operated on. */
/*! \var QImage Vectorizer::classifiedImage 
 Image created by classification. \sa getClassifiedImage */
/*! \var QImage Vectorizer::bwImage 
 BW Image created from classifiedImage by selectinn colors. \sa getBWImage */
/*! \var QImage Vectorizer::thinnedBWImage 
 (bad member name) BW Image after morphological operation. \sa getTransformedImage */
/*! \var MapColor** Vectorizer::sourceImageColors
 Classified color cluster moments. */
/*! \var MapColor* Vectorizer::mc 
 Prototype of mapcolor. */
/*! \var int Vectorizer::numberOfSourceImageColors
 Number of classes (colors) to be used by learning. */ 
/*! \var int Vectorizer::E 
 Parameter E of ClassicAlphaGetter. \sa setE */
/*! \var double Vectorizer::initAlpha 
 Initial alpha of ClassicAlphaGetter. \sa setInitAlpha */
/*! \var double Vectorizer::q
 Parameter q of ClassicAlphaGetter. \sa setQ */
/*! \var double Vectorizer::minAlpha 
 Minimal alpha of ClassicAlphaGetter. \sa setMinAlpha */
/*! \var double Vectorizer::p
 Minkowski metrics parameter. \sa setP */
/*! \var double Vectorizer::quality 
 Quality of learning as returned by getClassifiedImage. \sa getClassifiedImage */
/*! \var LearningMethod Vectorizer::learnMethod 
 Selected learning method. \sa setClassificationMethod */
/*! \var ColorSpace Vectorizer::colorSpace 
 Selected color space. \sa setColorSpace */
/*! \var AlphaStrategy Vectorizer::alphaStrategy 
 Selected alpha selection strategy. \sa setAlphaStrategy */
/*! \var PatternStrategy Vectorizer::patternStrategy 
 Selected pattern selection strategy. \sa setPatternStrategy */

Vectorizer::Vectorizer():
  state(NO_IMAGE),
  sourceImageColors(0),
  mc(new MapColorRGB(2.0)),
  E(100000),
  initAlpha(0.1),
  q(0.5),
  minAlpha(1e-6),
  p(2),
  learnMethod(KOHONEN_CLASSIC),
  colorSpace(COLSPC_RGB),
  alphaStrategy(ALPHA_CLASSIC),
  patternStrategy(PATTERN_RANDOM)
{
}

//! Constructs Vectorizer with im as sourceImage.
Vectorizer::Vectorizer(QImage& im)
{
  new(this) Vectorizer();
  sourceImage = im;
  if(im.format() == QImage::Format_Mono ||
      im.format() == QImage::Format_MonoLSB)
  {
    state = IMAGE;
  }
  else
  {
    bwImage = im;
    state = BW_IMAGE;
  }
}

Vectorizer::~Vectorizer()
{
}

/*! Takes minimal necessary actions to perform transition from one state to
 * other. \sa VectorizerState */
bool Vectorizer::fixState(VectorizerState s, ProgressObserver* progressObserver)
{
  bool retval = false;
  switch(state)
  {
    case NO_IMAGE: qWarning("NO_IMAGE state can't be fixed");
		   break;
		   // we have a valid sourceImage, we can classify it
    case IMAGE: retval = performClassification(progressObserver);
		if(retval)
		  retval = !getClassifiedImage(0, progressObserver).isNull();
		if(retval)
		  state = CLASSIFIED_IMAGE;
		if(!retval || s == CLASSIFIED_IMAGE) break;
		// bwImage must be created by calling getBWImage(...)
    case CLASSIFIED_IMAGE: qWarning("BW_IMAGE state can't be skipped");
			   break;
			   // valid bwImage can be thinned
    case BW_IMAGE: retval = getTransformedImage(THINNING_ROSENFELD,
		       progressObserver).isNull();
		   if(retval)
		     state = THINNED_IMAGE;
		   if(s == THINNED_IMAGE) break;
    case THINNED_IMAGE: qWarning("There is nothing more to do");
			break;
  }
  return retval;
}

/*! Choose classification method to be used. 
\sa LearningMethod */
void
Vectorizer::setClassificationMethod(LearningMethod learnMethod)
{
  this->learnMethod = learnMethod;
}

/*! Set color space in which the classification will be performed. */
void Vectorizer::setColorSpace(ColorSpace colorSpace)
{
  this->colorSpace = colorSpace;
  switch(colorSpace)
  {
    case COLSPC_RGB:
      mc = new MapColorRGB(p);
      break;
    case COLSPC_HSV:
      mc = new MapColorHSV(p);
      break;
    default:
      qWarning("UNIMPLEMENTED colorspace");
  }
}

/*! Set Minkowski metrics parameter.
  \sa MapColor */
void
Vectorizer::setP(double p)
{
  this->p = p;
  mc->setP(p);
}

/*! Choose alpha selection method to be used. 
\sa AlphaStrategy */
void
Vectorizer::setAlphaStrategy(AlphaStrategy alphaStrategy)
{
  this->alphaStrategy = alphaStrategy;
}

/*! Choose pattern selection method to be used. 
\sa PatternStrategy */
void
Vectorizer::setPatternStrategy(PatternStrategy patternStrategy)
{
  this->patternStrategy = patternStrategy;
}

/*! Set initial alpha for ClassicAlphaGetter.
\sa ClassicAlphaGetter */
void
Vectorizer::setInitAlpha(double initAlpha)
{
  this->initAlpha = initAlpha;
}

/*! Set minimal alpha for ClassicAlphaGetter.
\sa ClassicAlphaGetter */
void
Vectorizer::setMinAlpha(double minAlpha)
{
  this->minAlpha = minAlpha;
}

/*! Set q for ClassicAlphaGetter.
\sa ClassicAlphaGetter */
void
Vectorizer::setQ(double q)
{
  this->q = q;
}

/*! Set E for ClassicAlphaGetter.
\sa ClassicAlphaGetter */
void
Vectorizer::setE(int E)
{
  this->E = E;
}

/*! Set number of colors to be recognized in the picture.
  */
void
Vectorizer::setNumberOfColors(int nColors)
{
  this->numberOfSourceImageColors = nColors;
  if(sourceImageColors)
  {
    delete [] sourceImageColors;
    sourceImageColors = 0;
  }
}

/*! set initial colors for learning.  Sets random colors in case initColors is 0. */
void
Vectorizer::setInitColors(QRgb* initColors)
{
  if(sourceImageColors)
  {
    delete [] sourceImageColors;
    sourceImageColors = 0;
  }

  sourceImageColors = new MapColor* [numberOfSourceImageColors];

  for(int i = 0; i < numberOfSourceImageColors; i++)
  {
    MapColor *mcptr = dynamic_cast<MapColor*>(mc->clone());
    sourceImageColors[i] = mcptr;
    if(initColors)
    {
      // Get provided colors
      mcptr->setRGBTriplet(initColors[i]);
    }
    else
    {
      // Random colors at beginning
      mcptr->setRGBTriplet(qRgb(rand()/(RAND_MAX/255),
	    rand()/(RAND_MAX/255),
	    rand()/(RAND_MAX/255)));
      // Or linear grayscale
      /* mcptr->setRGBTriplet(qRgb(i*255/numberOfSourceImageColors,
	    i*255/numberOfSourceImageColors, i*255/numberOfSourceImageColors)); */
    }
  }
}

/*! Runs classfication.  The progress observer is called during work if set.
 * \param[in] progressObserver Pointer to class implementing ProgressObserver.
 * In case it is null pointer no calls take place.
 */
bool Vectorizer::performClassification(ProgressObserver* progressObserver)
{
  // invalidate previous images
  bwImage = classifiedImage = QImage();
  KohonenPatternGetter* pg = 0;

  KohonenMap km(numberOfSourceImageColors);
  OrganizableElement **classes =
    new OrganizableElement *[numberOfSourceImageColors];

  if(!sourceImageColors) 
  {
    setInitColors(0);
  }

  for(int i = 0; i < numberOfSourceImageColors; i++)
  {
    classes[i] = sourceImageColors[i];
  }

  km.setClasses(classes);
  switch(learnMethod)
  {
    case KOHONEN_CLASSIC: 
      {
	switch(patternStrategy)
	{
	  case PATTERN_RANDOM:
	    pg = new RandomPatternGetter(sourceImage, mc);
	    break;
	  default:
	    qWarning("UNIMPLEMENTED pattern getter");
	}

	KohonenAlphaGetter* ag = 0;
	switch(alphaStrategy)
	{
	  case ALPHA_CLASSIC:
	    ag = new ClassicAlphaGetter(initAlpha, q, E, minAlpha, progressObserver);
	    break;
	  default:
	    qWarning("UNIMPLEMENTED alpha getter");
	}

	km.performLearning(*ag, *pg);
	if(ag) delete ag;
	if(pg) delete pg;
      }
      break;
    case KOHONEN_BATCH: 
      {
	SequentialPatternGetter* bg =
	  new SequentialPatternGetter(sourceImage, mc, progressObserver);
	quality = km.performBatchLearning(*bg);
	classifiedImage = *bg->getClassifiedImage();
	delete bg;
      }
      break;
    default: qWarning("UNIMPLEMENTED classification method");
  }

  bool cancel = progressObserver && progressObserver->getCancelPressed();
  if(cancel)
  {
    delete [] sourceImageColors;
    sourceImageColors = 0;
    numberOfSourceImageColors = 0;
  } 
  else
  {
    delete [] classes;
    classes = km.getClasses();

    for(int i = 0; i < numberOfSourceImageColors; i++)
    {
      sourceImageColors[i] = dynamic_cast<MapColor*>(classes[i]);
    }
  }  

  delete [] classes;
  return !cancel;
}

/*! Returns colors found by classification process. */
QRgb*
Vectorizer::getClassifiedColors()
{
  if(!sourceImageColors)
    return 0;

  QRgb *retval = new QRgb[numberOfSourceImageColors];
  for(int i = 0; i < numberOfSourceImageColors; i++)
  {
    retval[i] = sourceImageColors[i]->getRGBTriplet();
  }

  return retval;
}

/*! Returns number of colors to be recognized in the picture.
\sa setNumberOfColors */
int
Vectorizer::getNumberOfClassifiedColors()
{
  return numberOfSourceImageColors;
}

/*! Creates image where original pixel colors are replaced by "closest
 * momentum" colors.  Also computes quality of learning.
 * \param qualityPtr Pointer to double where quality of learning will be stored.
 * \param progressObserver Optional progress observer.
 * \return New image. */
QImage
Vectorizer::getClassifiedImage(double *qualityPtr,
		ProgressObserver* progressObserver)
{
  if(classifiedImage.isNull())
  {
    double rowquality;
    quality = 0;
    MapColor *color =
      dynamic_cast<MapColor*>(sourceImageColors[0]->clone());
    KohonenMap km(numberOfSourceImageColors);
    int width = sourceImage.width(), height = sourceImage.height();
    int progressHowOften = (height>100)?height/75:1;
    classifiedImage = QImage(width, height, QImage::Format_Indexed8);
    classifiedImage.setColorCount(numberOfSourceImageColors);

    OrganizableElement **classes =
      new OrganizableElement *[numberOfSourceImageColors];

    for(int i = 0; i < numberOfSourceImageColors; i++)
    {
      classes[i] = sourceImageColors[i];
    }

    km.setClasses(classes);
    delete [] classes;

    bool cancel = false;
    for(int y = 0; !cancel && y < height; y++)
    {
      rowquality = 0;
      for(int x = 0; x < width; x++)
      {
	double distance;
	color->setRGBTriplet(sourceImage.pixel(x, y));
	int index = km.findClosest(*color, distance);
	classifiedImage.setPixel(x, y, index);
	rowquality += color->squares(*sourceImageColors[index]);
      }
      quality += rowquality;
      if(progressObserver && !(y%progressHowOften))
      {
	progressObserver->percentageChanged(y*100/height);
	cancel = progressObserver->getCancelPressed();
      }
    }

    delete color;

    if(cancel) 
    {
      quality = 0;
      classifiedImage = QImage();
    }
  }

  if(!classifiedImage.isNull())
  {
    for(int i = 0; i < numberOfSourceImageColors; i++)
    {
      classifiedImage.setColor(i, sourceImageColors[i]->getRGBTriplet());
    }
  }

  if(qualityPtr)
    *qualityPtr = quality;
  return classifiedImage;
}

/*! Creates BW image where pixels are on positions where any of the selected
 * colors were.  Final image is also held in data member bwImage.
 \param[in] selectedColors Boolean array where true means color is selected and
 pixels of that color will be set to 1 in output image.
 \param[in] progressObserver Progress observer.
 */
QImage
Vectorizer::getBWImage(bool* selectedColors,
		ProgressObserver* progressObserver)
{
  int width = sourceImage.width(), height = sourceImage.height();
  int progressHowOften = (height>100)?height/75:1;
  bwImage = QImage(width, height, QImage::Format_Mono);
  bwImage.setColorCount(2);
  bwImage.setColor(0, qRgb(255,255,255));
  bwImage.setColor(1, qRgb(0,0,0));
  bool cancel = false;
  for(int y = 0; !cancel && y < height; y++)
  {
    for(int x = 0; x < width; x++)
    {
      int colorIndex = classifiedImage.pixelIndex(x, y);
      bwImage.setPixel(x, y, selectedColors[colorIndex]);
    }
    if(progressObserver && !(y%progressHowOften))
    {
      progressObserver->percentageChanged(y*100/height);
      cancel = progressObserver->getCancelPressed();
    }
  }

  if(cancel) 
  {
    bwImage = QImage();
  }

  return bwImage;
}

/*! Performs selected Morphological operation on data member bwImage. 
 \sa MorphologicalOperation getTransformedImage(QImage bwImage, MorphologicalOperation mo, ProgressObserver* progressObserver) */
QImage Vectorizer::getTransformedImage(MorphologicalOperation mo,
	ProgressObserver* progressObserver)
{
  QImage i = Vectorizer::getTransformedImage(bwImage, mo, progressObserver);
  if(!i.isNull())
    bwImage = i;
  return i;
}

/*! Performs selected Morphological operation.
  \param[in] bwImage BW image to be processed.
  \param[in] mo Morphological operation to be performed.
  \param[in] progressObserver Progress observer.
  \return Transformed BW image.
 \sa MorphologicalOperation */
QImage Vectorizer::getTransformedImage(QImage bwImage, MorphologicalOperation mo,
	ProgressObserver* progressObserver)
{
  Morphology* thinner = new Morphology(&bwImage);
  bool ok = false;
  switch(mo) 
  {
    case EROSION: ok = thinner->erosion(progressObserver); break;
    case DILATION: ok = thinner->dilation(progressObserver); break;
    case THINNING_ROSENFELD: ok = thinner->rosenfeld(progressObserver); break;
    case PRUNING: ok = thinner->pruning(progressObserver); break;
    default: qWarning("UNIMPLEMENTED morphological operation");
  }
  if(ok) bwImage = *thinner->getImage();
  delete thinner;
  return ok ? bwImage : QImage();
}

//@}
