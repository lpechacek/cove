#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#ifndef USING_PCH
#include <QDomDocument>
#include <QDomElement>
#include <QRgb>
#endif /* USING_PCH */

class QString;

class Settings
{
  struct DoubleParam { const char* name; double value; };
  static DoubleParam doubletab[];
  QDomDocument storage;
  QDomElement findElement(const QString& elemname, const QString& attname) const;

  public:
  Settings();
  bool load(const QString& fname, QString* errmsg = 0);
  bool save(const QString& fname, QString* errmsg = 0);
  double getDouble(const QString& attname) const;
  int getInt(const QString& attname) const;
  QRgb* getInitColors(int* nColors, QString** comments = 0) const;
  QString getString(const QString& attname) const;
  bool setDouble(const QString& attname, double attvalue);
  bool setInt(const QString& attname, int attvalue);
  bool setInitColors(const QRgb* clrs, const QString* comments, const int nColors);
  bool setString(const QString& attname, const QString& attvalue);

  protected:
  QString escape(const QString& plain) const;
  QString unescape(const QString& escaped) const;
  void checkDefaults();
};

#endif
