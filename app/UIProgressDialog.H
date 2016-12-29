#ifndef __PROGRESSDIALOG_H__
#define __PROGRESSDIALOG_H__

#ifndef USING_PCH
#include <QEvent>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QStatusBar>
#endif /* USING_PCH */

#include "Vectorizer.H"

class QMainWindow;

class UIProgressDialog: public QObject, public ProgressObserver
{
  Q_OBJECT
  protected:
    bool canceled, centralWidgedEnabled, menuBarEnabled;
    QMainWindow* mw;
    QProgressBar pBar;
    QLabel infoText;
    QPushButton cancelButton;

  virtual void customEvent(QEvent* ev);

  public:
    UIProgressDialog(const QString& labelText,
    const QString& cancelButtonText, QWidget* creator = 0, Qt::WindowFlags f = 0);
    virtual ~UIProgressDialog();
    virtual void percentageChanged(int percentage);
    virtual bool getCancelPressed();

  public slots:
    void on_cancelButton_clicked();
};

#endif
