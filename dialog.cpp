#include "dialog.h"
#include "ui_dialog.h"
#include "spellchecker.h"
#include "spellcheckdialog.h"

#include <QMessageBox>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    connect(ui->buttonCheckSpelling, SIGNAL(clicked()), this, SLOT(checkSpelling()));
}


Dialog::~Dialog()
{
    delete ui;
}


void Dialog::checkSpelling()
{
    QString dictPath = "/usr/share/hunspell/hu_HU";
    QString userDict= "/usr/share/hunspell/hu_HU.aff";
    SpellChecker *spellChecker = new SpellChecker(dictPath, userDict);
    SpellCheckDialog *checkDialog = new SpellCheckDialog(spellChecker, this);

    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(QBrush(QColor("#ff6060")));
    highlightFormat.setForeground(QBrush(QColor("#000000")));
    // alternative format
    //highlightFormat.setUnderlineColor(QColor("red"));
    //highlightFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);

    // save the position of the current cursor
    QTextCursor oldCursor = ui->textEdit->textCursor();

    // create a new cursor to walk through the text
    QTextCursor cursor(ui->textEdit->document());

    // Don't call cursor.beginEditBlock(), as this prevents the rewdraw after changes to the content
    // cursor.beginEditBlock();
    while(!cursor.atEnd()) {
        QCoreApplication::processEvents();
        cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor, 1);
        QString word = cursor.selectedText();

        // Workaround for better recognition of words
        // punctuation etc. does not belong to words
        while(!word.isEmpty() && !word.at(0).isLetter() && cursor.anchor() < cursor.position()) {
            int cursorPos = cursor.position();
            cursor.setPosition(cursor.anchor() + 1, QTextCursor::MoveAnchor);
            cursor.setPosition(cursorPos, QTextCursor::KeepAnchor);
            word = cursor.selectedText();
        }

        if(!word.isEmpty() && !spellChecker->spell(word)) {
            QTextCursor tmpCursor(cursor);
            tmpCursor.setPosition(cursor.anchor());
            ui->textEdit->setTextCursor(tmpCursor);
            ui->textEdit->ensureCursorVisible();

            // highlight the unknown word
            QTextEdit::ExtraSelection es;
            es.cursor = cursor;
            es.format = highlightFormat;

            QList<QTextEdit::ExtraSelection> esList;
            esList << es;
            ui->textEdit->setExtraSelections(esList);
            QCoreApplication::processEvents();

            // ask the user what to do
            SpellCheckDialog::SpellCheckAction spellResult = checkDialog->checkWord(word);

            // reset the word highlight
            esList.clear();
            ui->textEdit->setExtraSelections(esList);
            QCoreApplication::processEvents();

            if(spellResult == SpellCheckDialog::AbortCheck)
                break;

            switch(spellResult) {
                case SpellCheckDialog::ReplaceOnce:
                    cursor.insertText(checkDialog->replacement());
                    break;
                case SpellCheckDialog::ReplaceAll:
                    this->replaceAll(cursor.position(), word, checkDialog->replacement());
                    break;

                default:
                    break;
            }
            QCoreApplication::processEvents();
        }
        cursor.movePosition(QTextCursor::NextWord, QTextCursor::MoveAnchor, 1);
    }
    //cursor.endEditBlock();
    ui->textEdit->setTextCursor(oldCursor);

    QMessageBox::information(
                this,
                tr("Finished"),
                tr("The spell check has finished."));
}


void Dialog::replaceAll(int nPos, QString sOld, QString sNew) {
    QTextCursor cursor(ui->textEdit->document());
    cursor.setPosition(nPos-sOld.length(), QTextCursor::MoveAnchor);

    while(!cursor.atEnd()) {
        QCoreApplication::processEvents();
        cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor, 1);
        QString word = cursor.selectedText();

        // Workaround for better recognition of words
        // punctuation etc. does not belong to words
        while(!word.isEmpty() && !word.at(0).isLetter() && cursor.anchor() < cursor.position()) {
            int cursorPos = cursor.position();
            cursor.setPosition(cursor.anchor() + 1, QTextCursor::MoveAnchor);
            cursor.setPosition(cursorPos, QTextCursor::KeepAnchor);
            word = cursor.selectedText();
        }

        if(word == sOld) {
            cursor.insertText(sNew);
            QCoreApplication::processEvents();
        }
        cursor.movePosition(QTextCursor::NextWord, QTextCursor::MoveAnchor, 1);
    }
}
