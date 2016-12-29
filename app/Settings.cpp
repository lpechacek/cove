#ifndef USING_PCH
#include <math.h>
#include <QDateTime>
#include <QDomNode>
#include <QFile>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTextStream>
#endif /* USING_PCH */

#include "Settings.H"

//@{ 
//! \ingroup gui

/*! \class Settings
  \brief Holds the settings.
  */

/*! \struct Settings::DoubleParam */
/*! \var const char* Settings::DoubleParam::name
 */ 
/*! \var double Settings::DoubleParam::value
 */
/*! \var QDomDocument Settings::storage 
 XML storage of the ettings. */

/*! Default parameter settings for Vectorizer */
Settings::DoubleParam Settings::doubletab[] = 
{{"E", 100000}, {"q", 0.5}, {"initAlpha", 0.1},
  {"minAlpha", 1e-6}, {"learnMethod", 0}, {"colorSpace", 0}, {"p", INFINITY},
  {"alphaStrategy", 0}, {"patternStrategy", 0}, {"nColors", 15},
  {"initColorsSource", 1}, {"speckleSize", 5}, {"doConnections", 1},
  {"joinDistance", 25}, {"simpleConnectionsOnly", 0}, {"distDirBalance", 0.5},
  {"cornerMin", 4.0/3}, {"optTolerance", 10}, {0, 0}};

/*! Constructor, creates root element and fills defaults from \a doubletab to
 * \a storage */
Settings::Settings()
{ 
  storage.appendChild(storage.createProcessingInstruction("xml", 
     "version=\"1.0\" encoding=\"utf-8\""));
  storage.appendChild(storage.createElement("covesettings"));

  checkDefaults();
}

/*! Fills missing parameters into \a storage. */
void Settings::checkDefaults()
{
  DoubleParam *dtptr;
  for(dtptr = doubletab; dtptr->name; dtptr++) 
  {
    if(isnan(getDouble(dtptr->name)))
      setDouble(dtptr->name, dtptr->value);
  }
}

/*! Loads \a storage from file.
\param[in] fname Filename to load data from.
\param[in] errmsg In case of error where to store the error message.
\return True when data have been successfully loaded. */
bool Settings::load(const QString& fname, QString* errmsg)
{
  QDomDocument doc;
  QFile file(fname);
  if (!file.open(QIODevice::ReadOnly))
  {
    if(errmsg)
    {
      *errmsg = QObject::tr("Cannot open file", "");
    }
    return false;
  }
  QString em;
  int el, ec;
  if (!doc.setContent(&file, &em, &el, &ec)) 
  {
    if(errmsg)
    {
      QString s = QObject::tr("Error parsing file (\"%1\""
	  " at line %2, column %3)", "");
      *errmsg = s.arg(em).arg(el).arg(ec);
    }
    file.close();
    return false;
  }
  file.close();
  storage = doc;
  checkDefaults();
  return true;
}

/*! Saves \a storage into file.
\param[in] fname Filename to save data to.
\param[in] errmsg In case of error where to store the error message.
\return True when data have been successfully saved. */
bool Settings::save(const QString& fname, QString* errmsg)
{
  QDomElement e = storage.documentElement();
  e.setAttribute("datetime", QDateTime::currentDateTime().toString(Qt::ISODate));

  QString xml = storage.toString();
  QFile file(fname);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QFile::Truncate))
  {
    if(errmsg)
    {
      *errmsg = QObject::tr("Cannot open file");
    }
    return false;
  }

  QTextStream fout(&file);
  fout.setCodec("UTF-8");
  fout << xml;
  file.close();
  return true;
}

/*! Returns double from storage.
  \param[in] attname Name of the variable to be retrieved.
  \return NaN in case the variable name does not exist */
double Settings::getDouble(const QString& attname) const
{
  QString s = getString(attname);
  if(!s.isNull())
  {
    bool ok;
    double rv = s.toDouble(&ok);
    if(!ok)
    {
      if(s == QString::fromLatin1("inf"))
	rv = INFINITY;
      else if(s == QString::fromLatin1("-inf"))
	rv = -INFINITY;
      else
	rv = nan("");
    }
    return rv;
  }
  else
  {
    return nan("");
  }
}

/*! Returns integer from storage.
  \param[in] attname Name of the variable to be retrieved.
  \return Zero in case the variable name does not exist */
int Settings::getInt(const QString& attname) const
{
  QString s = getString(attname);
  if(!s.isNull())
  {
    return s.toInt();
  }
  else
  {
    return 0;
  }
}

/*! Returns QRgb table of initial colors stored in element initColors.
  \param[in,out] nColors Pointer to integer where the number of colors will be stored.
  \param[in,out] comments Pointer to QString* where the comments will be stored.  Can be 0 what means that no comments will be retireved.
  \return Array of QRgb, initial colors. 0 when node initColors does not exist. */
QRgb* Settings::getInitColors(int* nColors, QString** comments) const
{
  QRgb* clrs = 0;
  if(comments) *comments = 0;
  QString s = getString("initColors");
  if(!s.isNull())
  {
    QStringList sl = s.split(",");
    int n = sl.size(), j = 0;
    clrs = new QRgb[n];
    if(comments) *comments = new QString[n];
    QStringList::const_iterator i;
    for (i = sl.constBegin(); i != sl.constEnd(); ++i)
    {
      clrs[j] = (*i).section(":", 0, 0).toInt(0, 16) | 0xFF000000;
      if(comments) (*comments)[j] = unescape((*i).section(":", 1, 1));
      j++;
    }
    if(nColors) *nColors = n;
  }
  return clrs;
}

/*! Returns bare string from storage.
  \param[in] attname Name of the variable to be retrieved.
  \return Empty string when variable name does not exist. */
QString Settings::getString(const QString& attname) const
{
  QDomElement element = storage.documentElement();
  for(QDomNode n = element.firstChild(); !n.isNull(); n = n.nextSibling())
  {
    QDomElement e = n.toElement();
    if(!e.isNull() && e.tagName() == attname)
    { 
      return e.attribute("value");
    }
  } 
  return QString();
}

/*! Sets double value in storage.
  \param[in] attname Name of the variable to be set.
  \param[in] attvalue Value associated with the name. */
bool Settings::setDouble(const QString& attname, double attvalue)
{
  setString(attname, QString::number(attvalue));
  return true;
}

/*! Sets integer value in storage.
  \param[in] attname Name of the variable to be set.
  \param[in] attvalue Value associated with the name. */
bool Settings::setInt(const QString& attname, int attvalue)
{
  setString(attname, QString::number(attvalue));
  return true;
}

/*! Sets QRgb table of initial colors stored in element initColors.
  \param[in] clrs Array of QRgb, initial colors.
  \param[in] comments Pointer to QString array where the comments will be stored.
  \param[in] nColors Pointer to integer where the number of colors will be stored.
  \return True when the operation is successful. */
bool Settings::setInitColors(const QRgb* clrs, const QString* comments, const int nColors)
{
  if(clrs)
  {
    QStringList sl;
    for(int i = 0; i < nColors; i++)
    {
      sl.push_back(QString("%1").arg(clrs[i] & 0xFFFFFF, 6, 16, QChar('0'))
	  .append(":").append(escape(comments ? comments[i] : QString())));
    }
    setString("initColors", sl.join(","));
  }
  return true;
}

/*! Sets bare string in storage.
  \param[in] attname Name of the variable to be retrieved.
  \param[in] attvalue String associated with the name. */
bool Settings::setString(const QString& attname, const QString& attvalue)
{
  QDomElement element = storage.documentElement(), e;
  QDomNode n;
  for(n = element.firstChild(); !n.isNull(); n = n.nextSibling())
  {
    e = n.toElement();
    if(!e.isNull() && e.tagName() == attname)
    { 
      break;
    }
  } 
  
  if(n.isNull())
  {
    e = storage.createElement(attname);
    storage.documentElement().appendChild(e);
  }
  e.setAttribute("value", attvalue);
  return true;
}

/*! Escapes unsafe charactes in string.  I.e. %, :, comma, <. */
QString Settings::escape(const QString& plain) const
{
  QString escaped;
  escaped.reserve(static_cast<int>(plain.length() * 1.1));
  for (int i = 0; i < plain.length(); i++)
  {
    switch(int c = plain.at(i).toLatin1())
    {
      case '%':
      case ':':
      case ',':
      case '<':
	escaped += QString("%%1").arg(c, 2, 16, QChar('0'));
	break;
      default:
	escaped += plain.at(i);
    }
  }
  return escaped;
}

/*! Unescapes string escaped by \a escape(). */
QString Settings::unescape(const QString& escaped) const
{
  QString plain;
  plain.reserve(static_cast<int>(escaped.length() / 1.1));
  for (int i = 0; i < escaped.length(); i++)
  {
    if(escaped.at(i) == QChar('%'))
    {
      bool success;
      QChar c = escaped.mid(i+1, 2).toInt(&success, 16);
      if(success) plain += c;
      i += 2;
    }
    else
    {
      plain += escaped.at(i);
    }
  }
  return plain;
}

//@}
