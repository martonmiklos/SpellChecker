#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <QString>
#include <QStringList>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
class QTextCodec;
#else
#include <QStringDecoder>
#include <QStringEncoder>
#endif
class Hunspell;

class SpellChecker {
 public:
  SpellChecker(const QString &dictionaryPath, const QString &userDictionary);
  ~SpellChecker();

  bool spell(const QString &word);
  QStringList suggest(const QString &word);
  void ignoreWord(const QString &word);
  void addToUserWordlist(const QString &word);

 private:
  void put_word(const QString &word);
  Hunspell *_hunspell;
  QString _userDictionary;
  QString _encoding;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  QTextCodec *_codec;
#else
  QStringDecoder _decoder;
  QStringEncoder _encoder;
#endif
};

#endif // SPELLCHECKER_H 
