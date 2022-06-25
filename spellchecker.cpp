#include "./spellchecker.h"

#include <QDebug>
#include <QFile>
#include <QRegularExpression>
#include <QStringList>
#include <QTextStream>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QTextCodec>
#endif

#include <string>
#include <vector>
#include "hunspell/hunspell.hxx"

SpellChecker::SpellChecker(const QString &dictionaryPath,
                           const QString &userDictionary)
  : _userDictionary(userDictionary) {
  QString dictFile = dictionaryPath + ".dic";
  QString affixFile = dictionaryPath + ".aff";
  QByteArray dictFilePathBA = dictFile.toLocal8Bit();
  QByteArray affixFilePathBA = affixFile.toLocal8Bit();
  _hunspell = new Hunspell(affixFilePathBA.constData(),
                           dictFilePathBA.constData());

  // detect encoding analyzing the SET option in the affix file
  _encoding = QStringLiteral("ISO8859-15");
  QFile _affixFile(affixFile);
  if (_affixFile.open(QIODevice::ReadOnly)) {
    QTextStream stream(&_affixFile);
    QRegularExpression enc_detector(
          QStringLiteral("^\\s*SET\\s+([A-Z0-9\\-]+)\\s*"),
          QRegularExpression::CaseInsensitiveOption);
    QString sLine;
    QRegularExpressionMatch match;
    while (!stream.atEnd()) {
      sLine = stream.readLine();
      if (sLine.isEmpty()) { continue; }
      match = enc_detector.match(sLine);
      if (match.hasMatch()) {
        _encoding = match.captured(1);
        qDebug() << "Encoding set to " + _encoding;
        break;
      }
    }
    _affixFile.close();
  }

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  _codec = QTextCodec::codecForName(this->_encoding.toLatin1().constData());
#else
  _decoder = QStringDecoder(this->_encoding.toLatin1().constData());
  _encoder = QStringEncoder(this->_encoding.toLatin1().constData());
  if (!_decoder.isValid() || !_encoder.isValid()) {
    qWarning() << "Invalid string converter! Decoder:" << _decoder.name()
               << "- Encoder: " << _encoder.name();
  }
#endif

  QFile userDictonaryFile(_userDictionary);
  if (!_userDictionary.isEmpty() && userDictonaryFile.exists()) {
    if (userDictonaryFile.open(QIODevice::ReadOnly)) {
      QTextStream stream(&userDictonaryFile);
      for (QString word = stream.readLine();
           !word.isEmpty();
           word = stream.readLine())
        put_word(word);
      userDictonaryFile.close();
    } else {
      qWarning() << "User dictionary in " << _userDictionary
                 << "could not be opened";
    }
  } else {
    qDebug() << "User dictionary not set.";
  }
}


SpellChecker::~SpellChecker() {
  delete _hunspell;
}


bool SpellChecker::spell(const QString &word) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  return _hunspell->spell(_codec->fromUnicode(word).toStdString());
#else
  return _hunspell->spell(QByteArray(_encoder(word)).toStdString());
#endif
}


QStringList SpellChecker::suggest(const QString &word) {
  int numSuggestions = 0;
  QStringList suggestions;
  std::vector<std::string> wordlist;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  wordlist = _hunspell->suggest(_codec->fromUnicode(word).toStdString());
#else
  wordlist = _hunspell->suggest(QByteArray(_encoder(word)).toStdString());
#endif

  numSuggestions = static_cast<int>(wordlist.size());
  if (numSuggestions > 0) {
    suggestions.reserve(numSuggestions);
    for (int i = 0; i < numSuggestions; i++) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
      suggestions << _codec->toUnicode(
                       QByteArray::fromStdString(wordlist[i]));
#else
      suggestions << _decoder(wordlist[i]);
#endif
    }
  }

  return suggestions;
}


void SpellChecker::ignoreWord(const QString &word) {
  put_word(word);
}


void SpellChecker::put_word(const QString &word) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  _hunspell->add(_codec->fromUnicode(word).constData());
#else
  _hunspell->add(QByteArray(_encoder(word)).constData());
#endif
}


void SpellChecker::addToUserWordlist(const QString &word) {
  put_word(word);
  if (!_userDictionary.isEmpty()) {
    QFile userDictonaryFile(_userDictionary);
    if (userDictonaryFile.open(QIODevice::Append)) {
      QTextStream stream(&userDictonaryFile);
      stream << word << "\n";
      userDictonaryFile.close();
    } else {
      qWarning() << "User dictionary in " << _userDictionary
                 << "could not be opened for appending a new word";
    }
  } else {
    qDebug() << "User dictionary not set.";
  }
}

