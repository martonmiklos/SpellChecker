#ifndef SPELLCHECKDIALOG_H
#define SPELLCHECKDIALOG_H

#include <QDialog>

class SpellChecker;

namespace Ui {
    class SpellCheckDialog;
}

class SpellCheckDialog : public QDialog
{
    Q_OBJECT

public:
    enum SpellCheckAction {AbortCheck, IgnoreOnce, IgnoreAll, ReplaceOnce, ReplaceAll, AddToDict};

    explicit SpellCheckDialog(SpellChecker *spellChecker, QWidget *parent = 0);
    ~SpellCheckDialog();

    QString replacement() const;

public slots:
    SpellCheckAction checkWord(const QString &word);

protected slots:
    void ignoreOnce();
    void ignoreAll();
    void replaceOnce();
    void replaceAll();
    void addToDict();

private:
    Ui::SpellCheckDialog *ui;
    SpellChecker *_spellChecker;
    QString _unkownWord;
    SpellCheckAction _returnCode;
};

#endif // SPELLCHECKDIALOG_H 
