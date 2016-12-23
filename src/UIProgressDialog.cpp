#ifndef USING_PCH
#include <QEvent>
#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#endif /* USING_PCH */

#include "CustomEvents.H"
#include "UIProgressDialog.H"

//@{ 
//! \ingroup gui

/*! \class UIProgressDialog
 * \brief QProgressDialog that implements ProgressObserver and is always
 * modal. InstantiateUIProgressDialog and pass it as ProgressObserver to
 * Vectorizer.
 */
/*! \var bool UIProgressDialog::canceled 
 Boolean indicationg that Cancel buton was pressed. */
/*! \var bool UIProgressDialog::centralWidgedEnabled 
 Boolean holding the enabled state of calling MainWindow's central widget. */
/*! \var bool UIProgressDialog::menuBarEnabled 
 Boolean holding the enabled state of calling MainWindow's menu bar. */
/*! \var QMainWindow* UIProgressDialog::mw 
 Pointer to the caller window. */
/*! \var QProgressBar UIProgressDialog::pBar 
 Progress bar to be inserted into MainWindow's statusbar. */
/*! \var QLabel UIProgressDialog::infoText 
 Text label to be inserted into MainWindow's statusbar. */
/*! \var QPushButton UIProgressDialog::cancelButton 
 Cancel button to be inserted into MainWindow's statusbar. */

/*! Constructor, esentially the same as QProgressDialog's contructor. Sets
 * progressbar range to 0-100 and disables menu and central widget of calling
 * main window.
 */
UIProgressDialog::UIProgressDialog(const QString& labelText,
    const QString& cancelButtonText, QWidget* creator, Qt::WindowFlags):
  QObject(creator), canceled(false),
  infoText(labelText), cancelButton(cancelButtonText)
{
  pBar.setRange(0, 100);
  if((mw = qobject_cast<QMainWindow*>(creator)) != 0)
  {
    QStatusBar* sb = mw->statusBar();
    sb->setSizeGripEnabled(false);
    sb->addWidget(&infoText, 70);
    sb->addWidget(&pBar, 20);
    sb->addWidget(&cancelButton, 10);
    centralWidgedEnabled = mw->centralWidget()->isEnabled();
    menuBarEnabled = mw->menuBar()->isEnabled();
    mw->centralWidget()->setEnabled(false);
    mw->menuBar()->setEnabled(false);
  }
  else
  {
    qWarning("UIProgressDialog not instantiated from QMainWindow");
  }
  connect(&cancelButton, SIGNAL(clicked()), this, SLOT(on_cancelButton_clicked()));
}

/*! Destructor. Sets the value to 100 and removes itself from main window if possible.
 */
UIProgressDialog::~UIProgressDialog()
{
  pBar.setValue(100);
  QStatusBar* sb;
  if((sb = qobject_cast<QStatusBar*>(infoText.parent())) != 0)
  {
    sb->removeWidget(&infoText);
    sb->removeWidget(&pBar);
    sb->removeWidget(&cancelButton);
  }
  if(mw)
  {
    mw->centralWidget()->setEnabled(centralWidgedEnabled);
    mw->menuBar()->setEnabled(menuBarEnabled);
  }
}

/*! Implementation of ProgressObserver abstract method, sends message
 * through Qt's event queue, so that progressbar updates are thread safe.
 */
void UIProgressDialog::percentageChanged(int percentage)
{
  QApplication::postEvent(this, new PercentageChangedEvent(this, percentage));
  qApp->processEvents();
}

/*! Reimplementation of QObject::customEvent(QEvent*). Updates status bar. */
void UIProgressDialog::customEvent(QEvent* ev)
{
  // PercentageChangedEvent
  if(ev->type() == (QEvent::User+1)) 
  {
    PercentageChangedEvent* pcev = dynamic_cast<PercentageChangedEvent*>(ev);
    // set progress bar percentage
    pBar.setValue(pcev->getPercentage());
  }
  qApp->processEvents();
}

/*! Sets canceled to true when cancel button is pressed. */
void UIProgressDialog::on_cancelButton_clicked()
{
  if(!canceled) 
  {
    QString text = infoText.text();
    infoText.setText(text.append(tr(" (cancelling)")));
    canceled = true;
  }
}

/*! Implementation of ProgressObserver abstract method, returns
 * value of \a canceled as set by UIProgressDialog::on_cancelButton_clicked().
 */
bool UIProgressDialog::getCancelPressed()
{
  return canceled;
}

//@}
