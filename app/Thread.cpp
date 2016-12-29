#ifndef USING_PCH
#endif /* USING_PCH */

#include "UIProgressDialog.H"
#include "Vectorizer.H"
#include "Thread.H"

//@{ 
//! \ingroup gui

/*! \class ClassificationThread
  \brief Subclass of Thread, used for running classification process. */
/*! \var UIProgressDialog* ClassificationThread::p 
  UIProgressDialog to be passed to performClassification. Can be 0. */
/*! \var Vectorizer* ClassificationThread::v 
  Vectorizer on which to run performClassification. */

//! Default constructor, marked private.
ClassificationThread::ClassificationThread() {}

/*! Constructor.  
  \param[in] v Vectorizer on which to run performClassification.
  \param[in] p UIProgressDialog to be passed to performClassification. Can be 0.
  */
ClassificationThread::ClassificationThread(Vectorizer* v, UIProgressDialog* p)
{
  this->v = v;
  this->p = p;
}

//! Reimplemented Thread method, calls Vectorizer::performClassification.
void ClassificationThread::run() 
{
  v->performClassification(p);
}

//@}
