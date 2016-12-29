#ifndef __COLORSEDITFORM_H__
#define __COLORSEDITFORM_H__

#ifndef USING_PCH
#include <QDialog>
#include <QVariant>
#include <QAbstractTableModel>
#endif /* USING_PCH */

#include "ui_colorseditform.h"

class ColorsListModel: public QAbstractTableModel
{
  Q_OBJECT 

  private:
  QRgb* colors;
  QString* comments;
  int nColors;
  ColorsListModel();

  public:
  ColorsListModel(int nColors, QRgb* colors, QString* comments); 
  virtual int rowCount(const QModelIndex&) const;
  virtual int columnCount(const QModelIndex&) const;
  virtual QVariant data(const QModelIndex& index, int role) const;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  virtual Qt::ItemFlags flags(const QModelIndex &index) const;
  virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
};

class ColorsEditingDelegate;

class ColorsEditForm: public QDialog
{
  Q_OBJECT 

  private:
  Ui::ColorsEditForm ui;
  ColorsListModel* m;
  ColorsEditingDelegate* d;

  public:
  enum ColorsSource {Random = 0, RandomFromImage, Predefined };
  ColorsEditForm(QWidget* parent = 0);
  ~ColorsEditForm();
  void setColors(QRgb* colors, QString* comments, int nColors);
  ColorsSource getColorsSource();
  void setColorsSource(ColorsSource s);

  public slots:
  void on_predefinedColorsButton_toggled(bool checked);
};

#endif
