#ifndef USING_PCH
#include <limits.h>
#include <math.h>
#include <QIntValidator>
#include <QDoubleValidator>
#endif /* USING_PCH */

#include "vectorizationconfigform.H"

VectorizationConfigForm::VectorizationConfigForm (QWidget * parent):
QDialog (parent)
{
  ui.setupUi (this);

  QDoubleValidator *dv;
  ui.shortPathLineEdit->setValidator (new QIntValidator (2, INT_MAX, this));
  ui.joinDistanceLineEdit->setValidator (
      dv = new QDoubleValidator (1, INFINITY, 1, this));
  ui.distDirBalanceLineEdit->
    setValidator (new QDoubleValidator (0, 1, 2, this));
  ui.cornerMinLineEdit->setValidator (new QDoubleValidator (0, 1.4, 2, this));
  ui.optToleranceLineEdit->setValidator (dv);
}

void VectorizationConfigForm::accept()
{
  on_simpleConnectionsCheckBox_toggled(simpleConnectionsOnly);
  on_doConnectionsCheckBox_toggled(doConnections);
  speckleSize = ui.shortPathLineEdit->text ().toDouble ();
  doConnections = ui.doConnectionsCheckBox->checkState () == Qt::Checked;
  joinDistance = ui.joinDistanceLineEdit->text ().toDouble ();
  simpleConnectionsOnly = ui.simpleConnectionsCheckBox->checkState () == Qt::Checked;
  distDirBalance = ui.distDirBalanceLineEdit->text ().toDouble ();
  cornerMin = ui.cornerMinLineEdit->text ().toDouble ();
  optTolerance = ui.optToleranceLineEdit->text ().toDouble ();
  QDialog::accept();
}

int VectorizationConfigForm::exec()
{
  on_simpleConnectionsCheckBox_toggled(simpleConnectionsOnly);
  on_doConnectionsCheckBox_toggled(doConnections);
  ui.shortPathLineEdit
    ->setText(QString::number(speckleSize));
  ui.doConnectionsCheckBox
    ->setCheckState(doConnections ? Qt::Checked : Qt::Unchecked);
  ui.joinDistanceLineEdit
    ->setText(QString::number(joinDistance));
  ui.simpleConnectionsCheckBox
    ->setCheckState(simpleConnectionsOnly ? Qt::Checked : Qt::Unchecked);
  ui.distDirBalanceLineEdit
    ->setText(QString::number(distDirBalance));
  ui.cornerMinLineEdit
    ->setText(QString::number(cornerMin));
  ui.optToleranceLineEdit
    ->setText(QString::number(optTolerance));

  return QDialog::exec();
}

void VectorizationConfigForm::on_doConnectionsCheckBox_toggled(bool state)
{
  ui.joinDistanceLineEdit->setEnabled(state);
  ui.simpleConnectionsCheckBox->setEnabled(state);
  
  if (state)
    ui.distDirBalanceLineEdit->setEnabled(
	ui.simpleConnectionsCheckBox->checkState () != Qt::Checked);
  else
    ui.distDirBalanceLineEdit->setEnabled(false);
}

void VectorizationConfigForm::on_simpleConnectionsCheckBox_toggled(bool state)
{
  ui.distDirBalanceLineEdit->setEnabled(!state);
}
