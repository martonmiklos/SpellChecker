#include "./spellcheckdialog.h"
#include "ui_spellcheckdialog.h"

#include "./spellchecker.h"

SpellCheckDialog::SpellCheckDialog(SpellChecker *spellChecker, QWidget *parent)
  : QDialog(parent),
    ui(new Ui::SpellCheckDialog) {
  ui->setupUi(this);
  _spellChecker = spellChecker;

  connect(ui->listWidget, &QListWidget::currentTextChanged,
          ui->ledtReplaceWith, &QLineEdit::setText);

  connect(ui->btnAddToDict, &QPushButton::clicked,
          this, &SpellCheckDialog::addToDict);
  connect(ui->btnReplaceOnce, &QPushButton::clicked,
          this, &SpellCheckDialog::replaceOnce);
  connect(ui->btnReplaceAll, &QPushButton::clicked,
          this, &SpellCheckDialog::replaceAll);
  connect(ui->btnIgnoreOnce, &QPushButton::clicked,
          this, &SpellCheckDialog::ignoreOnce);
  connect(ui->btnIgnoreAll, &QPushButton::clicked,
          this, &SpellCheckDialog::ignoreAll);
  connect(ui->btnCancel, &QPushButton::clicked,
          this, &SpellCheckDialog::reject);
}


SpellCheckDialog::~SpellCheckDialog() {
  delete ui;
}


SpellCheckDialog::SpellCheckAction SpellCheckDialog::checkWord(const QString &word) {
  _unkownWord = word;
  ui->lblUnknownWord->setText(QStringLiteral("<b>%1</b>").arg(_unkownWord));

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
