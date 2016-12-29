#ifndef __CLASSIFICATIONCONFIGFORM_H__
#define __CLASSIFICATIONCONFIGFORM_H__

#ifndef USING_PCH
#include <QDialog>
#endif /* USING_PCH */

#include "ui_classificationconfigform.h"

namespace classificationconfigform_private
{
  class QDoubleInfValidator: public QDoubleValidator
  {
    Q_OBJECT

    public:
      QDoubleInfValidator(QObject* parent);
      QDoubleInfValidator(double bottom, double top, int
	  decimals, QObject* parent);
      virtual QValidator::State validate(QString& input, int& pos) const;
  };
}

class ClassificationConfigForm: public QDialog
{
  Q_OBJECT

  const QIntValidator positiveIntegerValid;
  const QDoubleValidator double01Valid;
  const classificationconfigform_private::QDoubleInfValidator double1InfValid;
  Ui::ClassificationConfigForm ui;
  
  public:
  double initAlpha;
  double minAlpha;
  double Q;
  int E;
  int learnMethod;
  int colorSpace;
  double p;

  ClassificationConfigForm(QWidget *parent = 0);
  void accept();
  void setValues();

  public slots:
  void on_learningMethodComboBox_activated(int);
};

#endif
