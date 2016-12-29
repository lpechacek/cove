#ifndef __MAINFORM_H__
#define __MAINFORM_H__

#ifndef USING_PCH
#include <QMainWindow>
#include <QImage>
#include <QList>
#include <QTranslator>
#endif /* USING_PCH */

#include "ui_mainform.h"
#include "Settings.H"
#include "Vectorizer.H"

class ProgressObserver;
class QDragEnterEvent;
class QDropEvent;
class UIProgressDialog;
class Vectorizer;

class mainForm: public QMainWindow
{
  Q_OBJECT

  virtual void dragEnterEvent(QDragEnterEvent* event);
  virtual void dropEvent(QDropEvent* event);

  public:
  Ui::mainForm ui;

  protected:
  int numberOfColorButtons;
  UIProgressDialog* progressDialog;
  Vectorizer* vectorizerApp;
  QString imageFileName;
  QImage imageBitmap;
  QImage classifiedBitmap;
  QImage bwBitmap;
  bool rollbackHistory;
  QList<QImage> bwBitmapHistory;
  QList<QImage>::iterator bwBitmapHistoryIterator;
  QPushButton** colorButtons;
  Settings settings;
  QTranslator translator;
  QString translatorLanguage;
  QMap<QAction*, QString> actionToLang;
  static const int MESSAGESHOWDELAY;

  bool performMorphologicalOperation(Vectorizer::MorphologicalOperation mo);
  void prepareBWImageHistory();
  void bwImageCommitHistory();
  void bwImageClearHistory();
  void clearColorsTab();
  void clearBWImageTab();
  void setTabEnabled(QWidget* tab, bool state);
  void afterLoadImage();
  bool setLanguage(QString lang);
  QStringList getAvailableLanguages();
  QRgb getColorFromImage(const QImage& image);

  public:
  mainForm(QWidget* parent = 0, Qt::WindowFlags flags = 0);
  void loadImage(const QString& newfilename);
  void clearColorButtonsGroup();
  void setColorButtonsGroup(QRgb * colors, int nColors);
  bool* getSelectedColors();
  // unused slots
  void on_runOpeningButton_clicked();
  void on_runClosingButton_clicked();

  public slots:
  void aboutDialog();
  void classificationFinished();
  void clipboardPaste();
  void setInitialColors(bool on);
  void colorButtonToggled(bool on);
  void loadStatusData();
  void saveStatusData();

  void on_xResEdit_textChanged(const QString& s);
  void on_yResEdit_textChanged(const QString& s);
  void on_bwImageSaveButton_clicked();
  void on_classificationOptionsButton_clicked();
  void on_initialColorsButton_clicked();
  void on_howManyColorsSpinBox_valueChanged(int n);
  void on_loadImageButton_clicked();
  void on_mainTabWidget_currentChanged(int w);
  void on_setVectorizationOptionsButton_clicked();
  void on_createVectorsButton_clicked();
  void on_saveVectorsButton_clicked();
  void on_runClassificationButton_clicked();
  void on_runErosionButton_clicked();
  void on_runDilationButton_clicked();
  void on_runThinningButton_clicked();
  void on_runPruningButton_clicked();
  void on_bwImageHistoryBack_clicked();
  void on_bwImageHistoryForward_clicked();
  void on_applyFIRFilterPushButton_clicked();
  void on_menuLanguage_triggered(QAction* a);
};

#endif
