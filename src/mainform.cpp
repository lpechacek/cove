#ifndef USING_PCH
#include <math.h>
#include <QApplication>
#include <QClipboard>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QEvent>
#include <QEvent>
#include <QFileDialog>
#include <QImage>
#include <QLocale>
#include <QImageReader>
#include <QList>
#include <QListIterator>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QObjectList>
#include <QPixmap>
#include <QProgressDialog>
#include <QUrl>
#include <stdexcept>
#endif /* USING_PCH */

#include "classificationconfigform.H"
#include "colorseditform.H"
#include "vectorizationconfigform.H"
#include "CustomEvents.H"
#include "FIRFilter.H"
#include "mainform.H"
#include "OcadFile.H"
#include "Polygons.H"
#include "UIProgressDialog.H"
#include "Thread.H"
#include "Vectorizer.H"

using namespace std;

//@{
/*! \defgroup gui GUI classes */

/*! \class mainForm
  \brief Main form that is used as centralWidget() of QMainWindow.
  */

/*! \var Ui::mainForm mainForm::ui
  User interface as created by Qt Designer.
  */

/*! \var UIProgressDialog* mainForm::progressDialog 
  Progress dialog pointer. Is 0 when there is no progress dialog currently displayed.
*/

/*! \var Vectorizer* mainForm::vectorizerApp 
  Vectorizer application.
  \sa Vectorizer
*/

/*! \var QString mainForm::imageFileName 
  Filename of the currently loaded image.
*/

/*! \var QImage mainForm::imageBitmap 
  The currently loaded image.
*/

/*! \var QImage mainForm::classifiedBitmap 
  The classified image on the colors tab.
*/

/*! \var QImage mainForm::bwBitmap 
  The BW image on the thinning tab.
*/

/*! \var bool mainForm::rollbackHistory 
  Boolean indicating whether the history was commited.
  \sa prepareBWImageHistory, commitBWImageHistory
*/

/*! \var QList<QImage> mainForm::bwBitmapHistory 
  Container for all the images in the history.
*/

/*! \var QList<QImage>::iterator mainForm::bwBitmapHistoryIterator 
  Pointer to \a bwBitmapHistory.
*/

/*! \var int mainForm::numberOfColorButtons 
  Number of color buttons in the ColorButtonsGroup on the Colors tab.
  \sa colorButtons
*/

/*! \var QPushButton** mainForm::colorButtons 
  Array of pointers to color buttons in the ColorButtonsGroup on the Colors tab.
  \sa numberOfColorButtons
*/

/*! \var Settings mainForm::settings 
  Current status data (configuraiton variables).
*/

/*! \var QDoubleValidator mainForm::positiveDoubleValid 
  Input validator for LineEdit.
*/


namespace {
  QString dumpMimeData(const QMimeData* md)
  {
    QString m("Flags: ");
    if(md->hasColor()) m.append("hasColor ");
    if(md->hasImage()) m.append("hasImage ");
    if(md->hasHtml()) m.append("hasHtml ");
    if(md->hasText()) m.append("hasText ");
    if(md->hasUrls()) m.append("hasUrls ");
    m.append("\nFormats: ");
    QStringList f = md->formats();
    QStringList::const_iterator i;
    for (i = f.constBegin(); i != f.constEnd(); ++i)
    {
      m.append(*i);
      m.append("\n");
    }

    if(md->hasUrls())
    {
      m.append("URLs: ");
      QList<QUrl> u = md->urls();
      QList<QUrl>::const_iterator ui;
      for (ui = u.constBegin(); ui != u.constEnd(); ++ui)
      {
	m.append(ui->toString());
	m.append("\n");
      }
    }

    if(md->hasText())
    {
      m.append("\ntext: ");
      m.append(md->text());
    }

    if(md->hasHtml())
    {
      m.append("\nHTML: ");
      m.append(md->html());
    }

    return m;
  }
}

//! How long should be temporary message displayed.
const int mainForm::MESSAGESHOWDELAY = 5000;

//! Constructor initializing the menu, actions and connecting signals from actions
mainForm::mainForm(QWidget* parent, Qt::WindowFlags flags):
  QMainWindow(parent, flags), translator(qApp)
{
  QString lang = settings.getString("language");
  if(lang.isNull())
    lang = QLocale::system().name();
  if(lang != "en_US" && lang != "C")
    setLanguage(lang);
  else
    translatorLanguage = "C";

  ui.setupUi(this);

  connect(ui.fileOpenImageAction, SIGNAL(triggered()),
      this, SLOT(on_loadImageButton_clicked()));
  connect(ui.openStatusDataAction, SIGNAL(triggered()),
      this, SLOT(loadStatusData()));
  connect(ui.saveStatusDataAction, SIGNAL(triggered()),
      this, SLOT(saveStatusData()));
  connect(ui.aboutDialogAction, SIGNAL(triggered()), this, SLOT(aboutDialog()));
  connect(ui.exitProgramAction, SIGNAL(triggered()), this, SLOT(close()));
  connect(ui.actionPaste, SIGNAL(triggered()), this, SLOT(clipboardPaste()));
  ui.loadImageButton->addAction(ui.fileOpenImageAction);

  QStringList langs = getAvailableLanguages();
  langs.prepend("C");
  QActionGroup* ag = new QActionGroup(ui.menuLanguage);
  ag->setExclusive(true);
  for(QStringList::const_iterator i = langs.begin();
    i != langs.end(); ++i )
  {
    QLocale locale = QLocale(*i);
    QAction* la = ag->addAction(QLocale::languageToString(locale.language())
	+ " (" + QLocale::countryToString(locale.country()) + ")");
    la->setCheckable(true);
    if(translatorLanguage == *i)
      la->setChecked(true);
    actionToLang.insert(la, *i);
  }
  ui.menuLanguage->addActions(ag->actions());

  vectorizerApp = 0;
  progressDialog = 0;
  colorButtons = 0;
  numberOfColorButtons = 0;

  setTabEnabled(ui.imageTab, true);
  setTabEnabled(ui.thinningTab, false);
  setTabEnabled(ui.colorsTab, false);
//  setAcceptDrops(true);

  QDoubleValidator* dv = new QDoubleValidator(1e-6, INFINITY, 1, this);
  ui.xResEdit->setValidator(dv);
  ui.yResEdit->setValidator(dv);

  ui.howManyColorsSpinBox->setValue(settings.getInt("nColors"));
  rollbackHistory = false;
  bwBitmapHistoryIterator = bwBitmapHistory.begin();
}

//! Clears the Thinning tab, i.e. removes displayed image and polygons
void mainForm::clearBWImageTab()
{
  bwImageClearHistory();
  ui.bwImageView->setPolygons(0);
  ui.saveVectorsButton->setEnabled(false);
  ui.bwImageView->setImage(0);
}

//! Clears the Colors tab, i.e. removes displayed image and color buttons
void mainForm::clearColorsTab()
{
  if(!imageBitmap.isNull())
  {
    ui.classifiedColorsView->setImage(&imageBitmap);
  }
  else
  {
    ui.classifiedColorsView->setImage(0);
  }
  clearColorButtonsGroup();
}

//! Opens 'open file' dialog and then calls \a loadImage to load selected image
void mainForm::on_loadImageButton_clicked()
{
  QString filter(tr("Raster image files"));
  filter.append(" (");
  QList<QByteArray> extensions = QImageReader::supportedImageFormats();
  QList<QByteArray>::iterator it;

  for(it = extensions.begin(); it != extensions.end(); ++it) {
    if(it != extensions.begin())
      filter.append(" ");
    filter.append(QString(*it).prepend("*."));
  }

  filter.append(");;");
  filter.append(tr("All files"));
  filter.append("(*)");

  QString newfilename = QFileDialog::getOpenFileName(this, QString::null,
      QFileInfo(imageFileName).dir().path(), filter);

  if(!newfilename.isEmpty())
  {
    loadImage(newfilename);
  }
}

//! Loads image into form.
void mainForm::loadImage(const QString& newfilename)
{
  imageFileName = newfilename;
  imageBitmap = QImage(newfilename);
  afterLoadImage();
  statusBar()->showMessage(tr("Image %1 loaded.").arg(imageFileName), MESSAGESHOWDELAY);
}

//! Performs modifications of form after image load.
void mainForm::afterLoadImage()
{
  ui.mainTabWidget->setCurrentIndex(ui.mainTabWidget->indexOf(ui.imageTab));
  setTabEnabled(ui.thinningTab, false);
  clearBWImageTab();
  setTabEnabled(ui.colorsTab, false);
  clearColorsTab();
  ui.imageView->setImage(&imageBitmap);
  ui.imageView->reset();
  ui.classifiedColorsView->setImage(&imageBitmap);
  ui.classifiedColorsView->reset();

  QString infotext = QFileInfo(imageFileName).fileName();
  infotext.append(QString(" (%1px x %2px)").arg(imageBitmap.width())
      .arg(imageBitmap.height()));
  ui.imageInfoLabel->setText(infotext);
  ui.xResEdit->setText(QString::number(imageBitmap.dotsPerMeterX()*0.0254, 'f', 0));
  ui.xResEdit->setEnabled(true);
  ui.yResEdit->setText(QString::number(imageBitmap.dotsPerMeterY()*0.0254, 'f', 0));
  ui.yResEdit->setEnabled(true);
  if(!imageBitmap.isNull())
  {
    if(imageBitmap.format() == QImage::Format_Mono ||
	imageBitmap.format() == QImage::Format_MonoLSB)
    {
      setTabEnabled(ui.thinningTab, true);
    } 
    setTabEnabled(ui.colorsTab, true);
  }
}

QStringList mainForm::getAvailableLanguages()
{
  QStringList fileNames = QDir(":/translations")
    .entryList(QStringList("cove_*.qm"), QDir::Files, QDir::Name)
    + QDir(QCoreApplication::applicationDirPath())
    .entryList(QStringList("cove_*.qm"), QDir::Files, QDir::Name);
  
  for(QStringList::iterator i = fileNames.begin(); i != fileNames.end(); ++i)
    *i = i->mid(5,i->length()-8);

  return fileNames;
}

bool mainForm::setLanguage(QString lang)
{
  if(lang == "C")
  {
    qApp->removeTranslator(&translator);
  }
  else
  {
    QString fname = "cove_"+lang;
    if(translator.load(fname, ":/translations") ||
	translator.load(fname, QCoreApplication::applicationDirPath()))
    {
      qApp->installTranslator(&translator);
    }
    else
    {
      QMessageBox::warning(0, "Translation load failed", 
	  QString("Translation for language '")+lang
	  +"' could not be loaded, continuing with application's native language.",
	  QMessageBox::Ok, QMessageBox::NoButton);
      return false;
    }
  }

  translatorLanguage = lang;
  return true;
}

void mainForm::on_menuLanguage_triggered(QAction* a)
{
  if(actionToLang.contains(a))
  {
    QString lang;
    if((lang = actionToLang.value(a)) != translatorLanguage)
    {
      setLanguage(lang);
      ui.firFilterComboBox->clear();
      ui.retranslateUi(this);
    }
  }
  else
    qWarning("mainForm::on_menuLanguage_triggered: No action associated");
}

//! Changes x resolution of the image when user fills new value into the
// LineEdit on the Image tab.
void mainForm::on_xResEdit_textChanged(const QString& s)
{
  imageBitmap.setDotsPerMeterX(static_cast<int>(s.toDouble()/0.0254));
}

//! Changes y resolution of the image when user fills new value into the
// LineEdit on the Image tab.
void mainForm::on_yResEdit_textChanged(const QString& s)
{
  imageBitmap.setDotsPerMeterY(static_cast<int>(s.toDouble()/0.0254));
}

//! Simple About dialog.
void
mainForm::aboutDialog()
{
  QMessageBox::about(this, tr("About"),
        trUtf8("<h3>COntour VEctorizer</h3><br />"
         "Author: Libor Pecháček<br />" "License: GPL"));
}

/*! Return color randomly created from image pixels.  It takes 5 scanlines and
 * makes an average color of its pixels. */
QRgb mainForm::getColorFromImage(const QImage& image)
{
  srand(time(NULL));
  unsigned long r, g, b, divisor;
  r = g = b = divisor = 0;
  for(int a = 0; a < 5; a++)
  {
    int line = rand() % image.height();
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

/*! Performs actions requested by clicking 'Now' button on Colors tab.
  \sa classificationFinished() */
void
mainForm::on_runClassificationButton_clicked()
{
  ui.runClassificationButton->setEnabled(false);
  if (vectorizerApp)
  {
    delete vectorizerApp;
    vectorizerApp = 0;
  }
  vectorizerApp = new Vectorizer(imageBitmap);
  progressDialog = new UIProgressDialog(tr("Colors classification in "
	"progress"), tr("Cancel"), this);
  switch(settings.getInt("learnMethod"))
  {
    case 0: vectorizerApp->setClassificationMethod(Vectorizer::KOHONEN_CLASSIC);
	    break;
    case 1: vectorizerApp->setClassificationMethod(Vectorizer::KOHONEN_BATCH);
	    break;
  }
  vectorizerApp->setP(settings.getDouble("p"));
  switch(settings.getInt("colorSpace"))
  {
    case 0: vectorizerApp->setColorSpace(Vectorizer::COLSPC_RGB);
	    break;
    case 1: vectorizerApp->setColorSpace(Vectorizer::COLSPC_HSV);
	    break;
  }
  switch(settings.getInt("alphaStrategy"))
  {
    case 0: vectorizerApp->setAlphaStrategy(Vectorizer::ALPHA_CLASSIC);
	    break;
    case 1: vectorizerApp->setAlphaStrategy(Vectorizer::ALPHA_NEUQUANT);
	    break;
  }
  switch(settings.getInt("patternStrategy"))
  {
    case 0: vectorizerApp->setPatternStrategy(Vectorizer::PATTERN_RANDOM);
	    break;
    case 1: vectorizerApp->setPatternStrategy(Vectorizer::PATTERN_NEUQUANT);
	    break;
  }
  vectorizerApp->setInitAlpha(settings.getDouble("initAlpha"));
  vectorizerApp->setMinAlpha(settings.getDouble("minAlpha"));
  vectorizerApp->setQ(settings.getDouble("q"));
  vectorizerApp->setE(settings.getInt("E"));
  int nColors = settings.getInt("nColors");
  QRgb* colors = 0;
  switch(settings.getInt("initColorsSource"))
  {
    case ColorsEditForm::Random:
	    vectorizerApp->setNumberOfColors(nColors);
	    break;
    case ColorsEditForm::RandomFromImage:
	    colors = new QRgb[nColors];
	    vectorizerApp->setNumberOfColors(nColors);
	    for(int i = 0; i < nColors; i++)
	      colors[i] = getColorFromImage(imageBitmap);
	    break;
    case ColorsEditForm::Predefined:
	    colors = settings.getInitColors(&nColors);
	    vectorizerApp->setNumberOfColors(nColors);
	    break;
  }
  vectorizerApp->setInitColors(colors);
  ClassificationThread* ct = new ClassificationThread(vectorizerApp, progressDialog);
  connect(ct, SIGNAL(finished()), this, SLOT(classificationFinished()));
  ct->start();
  if(colors) delete colors;
}

//! Second part of on_runClassificationButton_clicked(), performs actions after
//the classification is done, i.e. creates classified image if necessary and
//creates color buttons by calling \a setColorButtonsGroup.
void mainForm::classificationFinished()
{
  QRgb *colorsFound = vectorizerApp->getClassifiedColors();
  int n = vectorizerApp->getNumberOfClassifiedColors();
  setColorButtonsGroup(colorsFound, n);
  delete progressDialog; progressDialog = 0;

  // in case there are some colors (learning was not aborted)
  if(n)
  {
    double quality;
    progressDialog = new UIProgressDialog(tr("Creating classified image"),
	tr("Cancel"), this);
    QImage newClassifiedBitmap = vectorizerApp->getClassifiedImage(&quality,
	progressDialog);
    delete progressDialog; progressDialog = 0;
    if(!newClassifiedBitmap.isNull())
    {
      classifiedBitmap = newClassifiedBitmap;
      ui.classifiedColorsView->setImage(&classifiedBitmap);
      ui.learnQualityLabel->setText(QString("LQ: %1").arg(quality));
    }
    else
    {
      clearColorButtonsGroup();
    }
  }
  else
  {
    ui.learnQualityLabel->setText(QString("LQ: -"));
  }
  delete colorsFound;
  ui.runClassificationButton->setEnabled(true);
}

/*! Removes all color buttons from the frame on Colors tab.
  \sa setColorButtonsGroup(QRgb* colors, int nColors) */
void
mainForm::clearColorButtonsGroup()
{
  if(!colorButtons) return; 
  for(int i = 0; i < numberOfColorButtons+1; i++) 
  {
    disconnect(colorButtons[i]);
    ui.gridLayout->removeWidget(colorButtons[i]);
    delete colorButtons[i];
  }
  QPushButton** delPtr = colorButtons;
  numberOfColorButtons = 0;
  colorButtons = 0;
  delete [] delPtr;
}

/*! Creates color buttons in the frame on Colors tab.
  \sa clearColorButtonsGroup() */
void
mainForm::setColorButtonsGroup(QRgb* colors, int nColors)
{
  clearColorButtonsGroup();
  if(!nColors) return;
  colorButtons = new QPushButton*[nColors+1];
  int i;
  for(i = 0; i < nColors; i++)
  {
    QPixmap buttonFace(20, 20);
    buttonFace.fill(QColor(colors[i]));
    QPushButton *button = new QPushButton(buttonFace, QString(), ui.colorButtonsGroup);
    ui.gridLayout->addWidget(button, i%2, i/2);
    colorButtons[i] = button;
    button->setCheckable(true);
    button->setMaximumSize(30, 30);
    button->setMinimumSize(10, 10);
    button->show();
    connect(button, SIGNAL(toggled(bool)), this, SLOT(colorButtonToggled(bool)));
  }  
  QPushButton *button = new QPushButton(ui.colorButtonsGroup);
  ui.gridLayout->addWidget(button, i%2, i/2);
  colorButtons[i] = button;
  button->setMaximumSize(30, 30);
  button->setMinimumSize(10, 10);
  button->setToolTip(tr("Set these colors as initial."));
  button->setText(tr("IC"));
  button->show();
  connect(button, SIGNAL(clicked(bool)), this, SLOT(setInitialColors(bool)));
  ui.colorButtonsGroup->adjustSize();
  numberOfColorButtons = nColors;
}

//! Performs action requested by clicking IC button in the color buttons frame,
//i.e. sets current colors as initial.
void mainForm::setInitialColors(bool /*on*/)
{
  int n = vectorizerApp->getNumberOfClassifiedColors();
  if(!n) return;
  QRgb* colorsFound = vectorizerApp->getClassifiedColors();
  QString* comments = new QString[n];
  settings.setInitColors(colorsFound, comments, n);
  delete [] comments;
  delete [] colorsFound;
  statusBar()->showMessage(tr("Current colors set as initial."), MESSAGESHOWDELAY);
}

//! Invoked by clicking a color button in the color buttons frame, changes the
//classified bitmap palette so that pixels of current selected colors appear in
//dark orange.
void mainForm::colorButtonToggled(bool /*on*/)
{
  int sels = 0;
  QRgb* colorsFound = vectorizerApp->getClassifiedColors();
  bool* selectedColors = getSelectedColors();   
  for(int i = 0; i < numberOfColorButtons; i++) 
  {
    classifiedBitmap.setColor(i, 
	selectedColors[i]?(sels++,qRgb(255,27,0)):colorsFound[i]);
  }
  delete [] selectedColors;
  delete [] colorsFound;
  setTabEnabled(ui.thinningTab, sels != 0);
  ui.classifiedColorsView->setImage(&classifiedBitmap);
  ui.classifiedColorsView->update();
}

//! Returns bool array indicating which colors are selected by color buttons.
bool* mainForm::getSelectedColors()
{
  int n = vectorizerApp->getNumberOfClassifiedColors();
  // do not allow different number of colors and buttons
  if(numberOfColorButtons < n)
  {
    throw logic_error("mainForm::getSelectedColors: more buttons than colors");
  }
  // also no colors is an oddity
  if(!colorButtons)
  {
    throw logic_error("mainForm::getSelectedColors: no buttons in group");
  }
  bool* retval = new bool[n];
  // copy states of the buttons into array
  for(int i = 0; i < numberOfColorButtons; i++) 
  {
    retval[i] = colorButtons[i]->isChecked();
  }
  return retval;  
}

//! Performs generating the BW image on change to Thinning tab.
void mainForm::on_mainTabWidget_currentChanged(int tabindex)
{
  if(ui.mainTabWidget->widget(tabindex) != ui.thinningTab) return;
  clearBWImageTab();
  if((imageBitmap.format() == QImage::Format_Mono ||
      imageBitmap.format() == QImage::Format_MonoLSB) && !numberOfColorButtons)
  {
    bwBitmap = imageBitmap;
    ui.bwImageView->setImage(&bwBitmap);
    ui.bwImageView->reset();
    return;
  }
  bool* selectedColors = getSelectedColors();   
  progressDialog = new UIProgressDialog(tr("Creating B/W image"),
      tr("Cancel"), this);
  QImage newBWBitmap = vectorizerApp->getBWImage(selectedColors, progressDialog);
  delete progressDialog; progressDialog = 0;
  delete selectedColors;
  if(!newBWBitmap.isNull())
  {
    bwBitmap = newBWBitmap;
    ui.bwImageView->setImage(&bwBitmap);
    ui.bwImageView->reset();
    setTabEnabled(ui.thinningTab, true);
  }
  else
  {
    setTabEnabled(ui.thinningTab, false);
  }
}

//! Performs one morphological operation on the bwImage.
bool mainForm::performMorphologicalOperation(Vectorizer::MorphologicalOperation mo)
{
  QString text;
  switch(mo)
  {
    case Vectorizer::THINNING_ROSENFELD: text = tr("Thinning B/W image"); break;
    case Vectorizer::PRUNING: text = tr("Pruning B/W image"); break;
    case Vectorizer::EROSION: text = tr("Eroding B/W image"); break;
    case Vectorizer::DILATION: text = tr("Dilating B/W image"); break;
  }
  progressDialog = new UIProgressDialog(text, tr("Cancel"), this);
  QImage transBitmap = Vectorizer::getTransformedImage(bwBitmap, mo, progressDialog);
  delete progressDialog; progressDialog = 0;
  if(!transBitmap.isNull())
  {
    bwBitmap = transBitmap;
    return true;
  }
  else
  {
    return false;
  }
}

//! Inserts the current displayed image into the history queue.  Pops the last
//image from the history queue in case \a rollbackHistory is set, what means
//the last image was not commited into the queue.
void mainForm::prepareBWImageHistory()
{
  if(rollbackHistory)
  {
    if(!bwBitmapHistory.empty()) bwBitmapHistory.pop_front(); 
  }
  else if(bwBitmapHistoryIterator != bwBitmapHistory.begin())
  {
    bwBitmapHistory.erase(bwBitmapHistory.begin(), bwBitmapHistoryIterator);
    bwBitmapHistory.pop_front(); 
  }
  bwBitmapHistory.push_front(bwBitmap); 
  bwBitmapHistoryIterator = bwBitmapHistory.begin();
  ui.bwImageView->setImage(&bwBitmap);
  rollbackHistory = true;
}

//! Completely clears the history.
void mainForm::bwImageClearHistory()
{
  ui.bwImageHistoryBack->setEnabled(false);
  ui.bwImageHistoryForward->setEnabled(false);
  bwBitmapHistory.clear();
  bwBitmapHistoryIterator = bwBitmapHistory.begin();
  rollbackHistory = false;
}

//! Commits the image into the history queue.
void mainForm::bwImageCommitHistory()
{
  ui.bwImageHistoryBack->setEnabled(true);
  ui.bwImageHistoryForward->setEnabled(false);
  rollbackHistory = false;
}

//! Returns one image back in the history.
void mainForm::on_bwImageHistoryBack_clicked()
{
  // safety check
  if(bwBitmapHistoryIterator == bwBitmapHistory.end()) return;
  if(bwBitmapHistoryIterator == bwBitmapHistory.begin())
  {
    bwBitmapHistory.push_front(bwBitmap); 
    bwBitmapHistoryIterator = bwBitmapHistory.begin();
  }
  if(++bwBitmapHistoryIterator != bwBitmapHistory.end())
  {
    bwBitmap = *bwBitmapHistoryIterator;
    ui.bwImageView->setImage(&bwBitmap);
    if(bwBitmapHistoryIterator+1 == bwBitmapHistory.end())
    {
      ui.bwImageHistoryBack->setEnabled(false);
    }
    ui.bwImageHistoryForward->setEnabled(true);
  }
}

//! Advances one image forward in the history.
void mainForm::on_bwImageHistoryForward_clicked()
{
  if(bwBitmapHistoryIterator != bwBitmapHistory.begin())
  {
    bwBitmap = *(--bwBitmapHistoryIterator);
    ui.bwImageView->setImage(&bwBitmap);
    ui.bwImageHistoryBack->setEnabled(true);
    if(bwBitmapHistoryIterator == bwBitmapHistory.begin())
    {
      ui.bwImageHistoryForward->setEnabled(false);
      bwBitmapHistory.pop_front(); 
      bwBitmapHistoryIterator = bwBitmapHistory.begin();
    }
  }
}

/*! Runs the thinning.
  \sa performMorphologicalOperation */
void mainForm::on_runThinningButton_clicked()
{
  prepareBWImageHistory();
  if(performMorphologicalOperation(Vectorizer::THINNING_ROSENFELD))
    bwImageCommitHistory();
}

/*! Runs the pruning.
  \sa performMorphologicalOperation */
void mainForm::on_runPruningButton_clicked()
{
  prepareBWImageHistory();
  if(performMorphologicalOperation(Vectorizer::PRUNING))
    bwImageCommitHistory();
}

/*! Erodes the image.
  \sa performMorphologicalOperation */
void mainForm::on_runErosionButton_clicked()
{
  prepareBWImageHistory();
  if(performMorphologicalOperation(Vectorizer::EROSION))
    bwImageCommitHistory();
}

/*! Dilates the image.
  \sa performMorphologicalOperation */
void mainForm::on_runDilationButton_clicked()
{
  prepareBWImageHistory();
  if(performMorphologicalOperation(Vectorizer::DILATION))
    bwImageCommitHistory();
}

/*! Opens the image.
  \sa performMorphologicalOperation */
void mainForm::on_runOpeningButton_clicked()
{
  prepareBWImageHistory();
  QImage savedimage = bwBitmap;
  if(performMorphologicalOperation(Vectorizer::EROSION) &&
      performMorphologicalOperation(Vectorizer::DILATION))
    bwImageCommitHistory();
  else
    bwBitmap = savedimage;
}

/*! Closes the image.
  \sa performMorphologicalOperation */
void mainForm::on_runClosingButton_clicked()
{
  prepareBWImageHistory();
  QImage savedimage = bwBitmap;
  if(performMorphologicalOperation(Vectorizer::DILATION) &&
      performMorphologicalOperation(Vectorizer::EROSION))
    bwImageCommitHistory();
  else
    bwBitmap = savedimage;
}

/*! Spawns dialog on classification config options.
  \sa ClassificationConfigForm */
void mainForm::on_classificationOptionsButton_clicked()
{
  ClassificationConfigForm* optionsDialog = new ClassificationConfigForm(this);
  optionsDialog->learnMethod = settings.getInt("learnMethod");
  optionsDialog->colorSpace = settings.getInt("colorSpace");
  optionsDialog->p = settings.getDouble("p");
  optionsDialog->E = settings.getInt("E");
  optionsDialog->Q = settings.getDouble("q");
  optionsDialog->initAlpha = settings.getDouble("initAlpha");
  optionsDialog->minAlpha = settings.getDouble("minAlpha");
  optionsDialog->setValues();
  optionsDialog->exec();
  if(optionsDialog->result() == ClassificationConfigForm::Accepted)
  {
    settings.setInt("E", optionsDialog->E);
    settings.setDouble("q", optionsDialog->Q);
    settings.setDouble("initAlpha", optionsDialog->initAlpha);
    settings.setDouble("minAlpha", optionsDialog->minAlpha); 
    settings.setDouble("learnMethod", optionsDialog->learnMethod);
    settings.setDouble("colorSpace", optionsDialog->colorSpace);
    settings.setDouble("p", optionsDialog->p);
  }
  delete optionsDialog;
}

/*! Spawns initial colors selection dialog.
  \sa ColorsEditForm */
void mainForm::on_initialColorsButton_clicked()
{
  ColorsEditForm* optionsDialog = new ColorsEditForm(this);
  int settingsNColors = settings.getInt("nColors");
  int nColors;
  QString* comments;
  QRgb* colors = settings.getInitColors(&nColors, &comments);
  if(!colors) 
  {
    nColors = settingsNColors;
    colors = new QRgb[nColors];
    comments = new QString[nColors];
    for(int i = 0; i < nColors; i++) 
    {
      colors[i] = qRgb(127,127,127);
      comments[i] = QString(tr("new color"));
    }
  }
  else if(nColors < settingsNColors)
  {
    QRgb* newColors = new QRgb[settingsNColors];
    QString* newComments = new QString[settingsNColors];
    for(int i = 0; i < nColors; i++) 
    {
      newColors[i] = colors[i];
      newComments[i] = comments[i];
    }
    for(int i = nColors; i < settingsNColors; i++) 
    {
      newColors[i] = qRgb(255,255,255);
      newComments[i] = QString(tr("new color"));
    }
    {
      QRgb* toDelete = colors;
      colors = newColors;
      delete [] toDelete; 
    }
    {
      QString* toDelete = comments;
      comments = newComments;
      delete [] toDelete; 
    }
  }
  optionsDialog->setColors(colors, comments, settingsNColors);
  optionsDialog->setColorsSource(
      static_cast<ColorsEditForm::ColorsSource>
      (settings.getInt("initColorsSource")));
  optionsDialog->exec();
  if(optionsDialog->result() == ColorsEditForm::Accepted)
  {
    settings.setInt("initColorsSource", optionsDialog->getColorsSource());
    settings.setInitColors(colors, comments, settingsNColors);
  }
  delete [] colors;
  delete [] comments;
  delete optionsDialog;
}

//! Stores the value of number of colors spinbox located at colors tab.
void mainForm::on_howManyColorsSpinBox_valueChanged(int n)
{
  settings.setInt("nColors", n);
}

//! Spawns dialog on vectorization options.
void mainForm::on_setVectorizationOptionsButton_clicked()
{
  VectorizationConfigForm optionsDialog(this);
  optionsDialog.speckleSize = settings.getDouble("speckleSize");
  optionsDialog.doConnections =
    settings.getInt("doConnections");
  optionsDialog.joinDistance = settings.getDouble("joinDistance");
  optionsDialog.simpleConnectionsOnly =
    settings.getInt("simpleConnectionsOnly");
  optionsDialog.distDirBalance = settings.getDouble("distDirBalance");
  optionsDialog.cornerMin = settings.getDouble("cornerMin");
  optionsDialog.optTolerance = settings.getDouble("optTolerance");
  optionsDialog.exec();
  if(optionsDialog.result() == ClassificationConfigForm::Accepted)
  {
    settings.setDouble("speckleSize",
	optionsDialog.speckleSize);
    settings.setDouble("joinDistance",
	optionsDialog.joinDistance);
    settings.setInt("doConnections",
	optionsDialog.doConnections);
    settings.setInt("simpleConnectionsOnly",
	optionsDialog.simpleConnectionsOnly);
    settings.setDouble("distDirBalance",
	optionsDialog.distDirBalance);
    settings.setDouble("cornerMin",
	optionsDialog.cornerMin);
    settings.setDouble("optTolerance",
	optionsDialog.optTolerance);
  }
}

//! Creates polygons from current bwImage.
void mainForm::on_createVectorsButton_clicked()
{
  Polygons::PolygonList* q = new Polygons::PolygonList;
  Polygons p;

  p.setSpeckleSize(settings.getInt("speckleSize"));
  p.setMaxDistance(settings.getInt("doConnections")
      ? settings.getDouble("joinDistance") : 0);
  p.setSimpleOnly(settings.getInt("simpleConnectionsOnly"));
  p.setDistDirRatio(settings.getDouble("distDirBalance"));
  p.setAlphaMax(settings.getDouble("cornerMin"));
  p.setOptTolerance(settings.getDouble("optTolerance"));
  progressDialog = new UIProgressDialog(tr("Vectorizing"),
      tr("Cancel"), this);
  *q = p.createPolygonsFromImage(bwBitmap, progressDialog);
  delete progressDialog; progressDialog = 0;
  if(q->empty()) 
  {
    delete q;
    return;
  }

  ui.bwImageView->setPolygons(q); 
  ui.saveVectorsButton->setEnabled(true);
  delete q;
}

//! Saves current created polygons to an .OCD file.
void mainForm::on_saveVectorsButton_clicked()
{
  const Polygons::PolygonList* q = ui.bwImageView->polygons();
  if(q)
  {
    QString filter(tr("Ocad files (*.ocd);;All files (*)"));
    QString newfilename = QFileDialog::getSaveFileName(this, QString::null,
	QFileInfo(imageFileName).dir().path(), filter);

    if(!newfilename.isEmpty())
    {
      if(!QFile(newfilename).exists() && !newfilename.endsWith(".ocd", Qt::CaseInsensitive))
	newfilename.append(".ocd");
      OcadFile o(newfilename.toLatin1().constData(),
	  static_cast<int>(round(imageBitmap.dotsPerMeterX()*0.0254)),
	  imageFileName.toLocal8Bit().constData());
      o.insertPolygons(*q, float(-bwBitmap.width())/2, float(-bwBitmap.height())/2);
      statusBar()->showMessage(tr("Polygons have been successfully saved into %1")
	  .arg(newfilename), MESSAGESHOWDELAY);
    }
  }
}

//! Saves current bwImage to an image file.
void mainForm::on_bwImageSaveButton_clicked()
{
  QString filter;
  QList<QByteArray> extensions = QImageReader::supportedImageFormats();
  QList<QByteArray>::iterator it;

  for(it = extensions.begin(); it != extensions.end(); ++it) {
    filter.append(QString(*it).toUpper());
    filter.append(QString(*it).prepend(" (*.").append(");;"));
  }

  filter.append(tr("All files"));
  filter.append("(*)");

  QString selectedFilter("PNG (*.png)");
  QString newfilename = QFileDialog::getSaveFileName(this, QString::null,
      QFileInfo(imageFileName).dir().path(), filter, &selectedFilter);

  if(!newfilename.isEmpty())
  {
    QString suffix = selectedFilter.section(" ", 1, 1);
    suffix = suffix.mid(2, suffix.size()-3);
    if(!newfilename.endsWith(suffix, Qt::CaseInsensitive))
      newfilename.append(suffix);
    bwBitmap.save(newfilename, selectedFilter.section(" ", 0, 0).toLatin1().constData());
    statusBar()->showMessage(tr("Image has been successfully saved into %1")
	.arg(newfilename), MESSAGESHOWDELAY);
  }
}

//! D'n'D methods, nonfunctional yet.
void mainForm::dragEnterEvent(QDragEnterEvent *event)
{
  //if (event->mimeData()->hasFormat("text/uri-list"))
    event->acceptProposedAction();
}

//! D'n'D methods, nonfunctional yet.
void mainForm::dropEvent(QDropEvent *event)
{
  /*  textBrowser->setPlainText(event->mimeData()->text());
      mimeTypeCombo->clear();
      mimeTypeCombo->addItems(event->mimeData()->formats());
   */


  event->acceptProposedAction();
  QMessageBox::information(this, "D'n'D", dumpMimeData(event->mimeData()), QMessageBox::Ok);
}

//! Dispatches clipboard paste.
void mainForm::clipboardPaste()
{
  QClipboard *clipboard = QApplication::clipboard();
  if(!clipboard->image().isNull())
  {
    imageBitmap = clipboard->image();
    imageFileName = tr("Clipboard paste");
    afterLoadImage();
    statusBar()->showMessage(tr("Image pasted from clipboard."), MESSAGESHOWDELAY);
  }
}

//! Loads settings from an XML file.
void mainForm::loadStatusData()
{
  QString filter(tr("XML (*.xml);;All files (*)"));
  QString newfilename = QFileDialog::getOpenFileName(this, QString::null,
      QString::null, filter);

  if(!newfilename.isEmpty())
  {
    if(!QFile(newfilename).exists() && !newfilename.endsWith(".xml", Qt::CaseInsensitive))
      newfilename.append(".xml");
    settings.load(newfilename);

    if(settings.getInt("initColorsSource") == ColorsEditForm::Predefined)
    {
      int nColors;
      delete settings.getInitColors(&nColors);
      ui.howManyColorsSpinBox->setValue(nColors);
    }
    else
    { 
      ui.howManyColorsSpinBox->setValue(settings.getInt("nColors"));
    }
    statusBar()->showMessage(tr("Settings successfully loaded from %1")
	.arg(newfilename), MESSAGESHOWDELAY);
  }
}

//! Saves settings an XML file.
void mainForm::saveStatusData()
{
  QString filter(tr("XML (*.xml);;All files (*)"));
  QString newfilename = QFileDialog::getSaveFileName(this, QString::null,
      QFileInfo(imageFileName).dir().path(), filter);

  if(!newfilename.isEmpty())
  {
    if(!QFile(newfilename).exists() && !newfilename.endsWith(".xml", Qt::CaseInsensitive))
      newfilename.append(".xml");
    settings.save(newfilename);
    statusBar()->showMessage(tr("Settings successfully saved to %1")
	.arg(newfilename), MESSAGESHOWDELAY);
  }
}

//! Qt4 bug workaround method. Indicates tab enabled state by
//uppercasing/lowercasing its title.
void mainForm::setTabEnabled(QWidget* tab, bool state)
{
  int tabindex = ui.mainTabWidget->indexOf(tab);
//  ui.mainTabWidget->setTabText(tabindex,
//      state ? ui.mainTabWidget->tabText(tabindex).toUpper() 
//      : ui.mainTabWidget->tabText(tabindex).toLower());
  ui.mainTabWidget->setTabEnabled(tabindex, state);
}

/*! Applies FIR filter onto \a imageBitmap.
  \sa FIRFilter */
void mainForm::on_applyFIRFilterPushButton_clicked()
{
  if(imageBitmap.isNull()) return;
  FIRFilter f;
  switch(ui.firFilterComboBox->currentIndex())
  {
    case 0: f = FIRFilter(2).binomic(); break;
    case 1: f = FIRFilter(3).binomic(); break;
    case 2: f = FIRFilter(2).box(); break;
    case 3: f = FIRFilter(3).box(); break;
  }

  progressDialog = new UIProgressDialog(tr("Applying FIR Filter on image"),
      tr("Cancel"), this);
  QImage newImageBitmap = f.apply(imageBitmap, qRgb(127,127,127), progressDialog);
  delete progressDialog; progressDialog = 0;
  if(!newImageBitmap.isNull()) 
    imageBitmap = newImageBitmap;
  ui.imageView->setImage(&imageBitmap);
}

//@}
