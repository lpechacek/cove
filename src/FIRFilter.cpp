#ifndef USING_PCH
#include <QImage>
#endif /* USING_PCH */

#include "FIRFilter.H"
#include "MapColor.H"
#include "Vectorizer.H"

//@{
//! \ingroup gui

/*! \class FIRFilter
  \brief FIR (Finite Image Response) filter that can be applied onto QImage.

  Implemented filters are Box and Binomic of arbitrary size.
 */

/*! \var unsigned FIRFilter::dimension
  FIR filter matrix dimension. */

/*! \var double** FIRFilter::matrix
  FIR filter matrix. */

/*! Constructor, allocates \a matrix.
  \param[in] radius Filter radius. 1 => matrix size 1x1, 2 => 3x3, 3 => 5x5, ... */
FIRFilter::FIRFilter(unsigned radius)
{
  dimension = radius*2-1;
  if(dimension > 0)
  {
    matrix = new double*[dimension];
    for(int i = 0; i < dimension; i++) 
      matrix[i] = new double[dimension];
  }
}

/*! Copy constructor. */
FIRFilter::FIRFilter(FIRFilter& other)
{
  new(this) FIRFilter((other.dimension+1)/2);
  *this = other;
}

/*! Assignment operator. */
FIRFilter& FIRFilter::operator=(const FIRFilter& other)
{
  if(other.dimension != dimension)
  {
    this->~FIRFilter();
    dimension = other.dimension;
    new(this) FIRFilter((dimension+1)/2);
  }

  for(int i = 0; i < dimension; i++)
    for(int j = 0; j < dimension; j++)
      matrix[i][j] = other.matrix[i][j];

  return *this;
}

/*! Destructor, deallocates \a matrix. */
FIRFilter::~FIRFilter()
{
  if(dimension > 0)
  {
    for(int i = 0; i < dimension; i++) 
      delete [] matrix[i];
    delete [] matrix;
  }
}

/*! Fills binomic FIR filter values into \a matrix. */
FIRFilter& FIRFilter::binomic()
{
  double* temprow = new double[dimension+1];
  temprow[0] = 0;
  temprow[dimension] = 0;

  for(int i = 0; i < dimension; i++)
    matrix[0][i] = 0;
  matrix[0][dimension/2] = 1;

  // create binomic coefficients
  while(matrix[0][0] == 0)
  {
    for(int i = 1; i < dimension; i++) 
      temprow[i] = matrix[0][i] + matrix[0][i-1];
    for(int i = 0; i < dimension; i++) 
      matrix[0][i] = temprow[i] + temprow[i+1];
  }

  // create other elements
  double divisor = 0;
  for(int i = 0; i < dimension; i++)
    for(int j = 0; j < dimension; j++)
    {
      matrix[i][j] = matrix[0][i] * matrix[0][j];
      divisor += matrix[i][j];
    }

  // normalize matrix so that sum of all its elements gives 1
  for(int i = 0; i < dimension; i++)
    for(int j = 0; j < dimension; j++)
      matrix[i][j] /= divisor;

  delete [] temprow;

  return *this;
}

/*! Fills box FIR filter values into \a matrix. */
FIRFilter& FIRFilter::box()
{
  double q = 1.0/(dimension*dimension);
  for(int i = 0; i < dimension; i++)
    for(int j = 0; j < dimension; j++)
      matrix[i][j] = q;

  return *this;
}

//! Not implemented yet.
FIRFilter& FIRFilter::a(double /*center*/)
{
  return *this;
}

/*! Applies this FIR filter onto image and returns transformed image.
  \param[in] source Source image.
  \param[in] outOfBoundsColor Color that has the out-of-bounds area.
  \param[in] progressObserver Progress observer.  */
QImage FIRFilter::apply(const QImage& source, QRgb outOfBoundsColor,
    ProgressObserver* progressObserver)
{
  int imwidth = source.width(), imheight = source.height();
  bool cancel = false;
  int progressHowOften = (imheight>100)?imheight/75:1;
  QImage retimage(imwidth, imheight, QImage::Format_RGB32);

  unsigned bordercut = dimension/2;
  MapColorRGB c, p;
  for(int y = 0; !cancel && y < imheight; y++)
  {
    for(int x = 0; x < imwidth; x++)
    {
      c.setRGBTriplet(qRgb(0, 0, 0));
      for(int i = 0; i < dimension; i++)
	for(int j = 0; j < dimension; j++)
	{
	  if(source.valid(x+i-bordercut, y+j-bordercut))
	    p.setRGBTriplet(source.pixel(x+i-bordercut, y+j-bordercut));
	  else
	    p.setRGBTriplet(outOfBoundsColor);
	  p.multiply(matrix[i][j]);
	  c.add(p);
	}
      retimage.setPixel(x, y, c.getRGBTriplet());
    }
    if(progressObserver && !(y%progressHowOften))
    {
      progressObserver->percentageChanged(y*100/imheight);
      cancel = progressObserver->getCancelPressed();
    }
  }
  return cancel ? QImage() : retimage;
}

//@}
