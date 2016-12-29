#ifndef __VECTORIZATIONCONFIGFORM_H__
#define __VECTORIZATIONCONFIGFORM_H__

#ifndef USING_PCH
#include <QDialog>
#endif /* USING_PCH */

#include "ui_vectorizationconfigform.h"

class VectorizationConfigForm: public QDialog
{
  Q_OBJECT

  Ui::VectorizationConfigForm ui;
  
  public:
  double speckleSize, joinDistance, distDirBalance, cornerMin, optTolerance;
  bool doConnections, simpleConnectionsOnly; 

  VectorizationConfigForm(QWidget *parent = 0);
  void accept();
  void setValues();

  public slots:
  int exec ();
  void on_simpleConnectionsCheckBox_toggled(bool);
  void on_doConnectionsCheckBox_toggled(bool);
};

#endif
