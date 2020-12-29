#include "./spellchecker.h"

#include <QDebug>
#include <QFile>
#include <QRegularExpression>
#include <QStringList>
#include <QTextCodec>
#include <QTextStream>

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
  _codec = QTextCodec::codecForName(this->_encoding.toLatin1().constData());

  if (!_userDictionary.isEmpty()) {
    QFile userDictonaryFile(_userDictionary);
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
  return _hunspell->spell(_codec->fromUnicode(word).toStdString());
}


QStringList SpellChecker::suggest(const QString &word) {
  int numSuggestions = 0;
  QStringList suggestions;
  std::vector<std::string> wordlist;
  wordlist = _hunspell->suggest(_codec->fromUnicode(word).toStdString());

  numSuggestions = static_cast<int>(wordlist.size());
  if (numSuggestions > 0) {
    suggestions.reserve(numSuggestions);
    for (int i = 0; i < numSuggestions; i++) {
      suggestions << _codec->toUnicode(
                       QByteArray::fromStdString(wordlist[i]));
    }
  }

  return suggestions;
}


void SpellChecker::ignoreWord(const QString &word) {
  put_word(word);
}


void SpellChecker::put_word(const QString &word) {
  _hunspell->add(_codec->fromUnicode(word).constData());
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

