#include "./spellcheckdialog.h"
#include "ui_spellcheckdialog.h"

#include "./spellchecker.h"

SpellCheckDialog::SpellCheckDialog(SpellChecker *spellChecker, QWidget *parent)
  : QDialog(parent),
    ui(new Ui::SpellCheckDialog) {
  ui->setupUi(this);
  _spellChecker = spellChecker;

  connect(ui->listWidget, SIGNAL(currentTextChanged(QString)),
          ui->ledtReplaceWith, SLOT(setText(QString)));

  connect(ui->btnAddToDict, SIGNAL(clicked()), this, SLOT(addToDict()));
  connect(ui->btnReplaceOnce, SIGNAL(clicked()), this, SLOT(replaceOnce()));
  connect(ui->btnReplaceAll, SIGNAL(clicked()), this, SLOT(replaceAll()));
  connect(ui->btnIgnoreOnce, SIGNAL(clicked()), this, SLOT(ignoreOnce()));
  connect(ui->btnIgnoreAll, SIGNAL(clicked()), this, SLOT(ignoreAll()));
  connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
}


SpellCheckDialog::~SpellCheckDialog() {
  delete ui;
}


SpellCheckDialog::SpellCheckAction SpellCheckDialog::checkWord(const QString &word) {
  _unkownWord = word;
  ui->lblUnknownWord->setText(QString("<b>%1</b>").arg(_unkownWord));

  ui->ledtReplaceWith->clear();

  QStringList suggestions = _spellChecker->suggest(word);
  ui->listWidget->clear();
  ui->listWidget->addItems(suggestions);

  if (suggestions.count() > 0)
    ui->listWidget->setCurrentRow(0, QItemSelectionModel::Select);

  _returnCode = AbortCheck;
  QDialog::exec();
  return _returnCode;
}


QString SpellCheckDialog::replacement() const {
  return ui->ledtReplaceWith->text();
}


void SpellCheckDialog::ignoreOnce() {
  _returnCode = IgnoreOnce;
  accept();
}


void SpellCheckDialog::ignoreAll() {
  _spellChecker->ignoreWord(_unkownWord);
  _returnCode = IgnoreAll;
  accept();
}


void SpellCheckDialog::replaceOnce() {
  _returnCode = ReplaceOnce;
  accept();
}


void SpellCheckDialog::replaceAll() {
  _returnCode = ReplaceAll;
  accept();
}


void SpellCheckDialog::addToDict() {
  _spellChecker->addToUserWordlist(_unkownWord);
  _returnCode = AddToDict;
  accept();
}
