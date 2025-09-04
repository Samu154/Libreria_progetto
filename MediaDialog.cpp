#include "MediaDialog.h"
#include <QJsonObject>


MediaDialog::MediaDialog(QWidget* parent, QSharedPointer<Media> mediaPassed)
: QDialog(parent)
, ui(new Ui::MediaDialog)
, m_original(mediaPassed)
{
ui->setupUi(this);


// populate type combo
ui->typeComboBox->addItems(MediaFactory::supportedTypes());


// validators
ui->bookYearLineEdit->setValidator(new QIntValidator(0, 9999, this));
ui->filmDurationLineEdit->setValidator(new QIntValidator(0, 10000, this));
ui->articlePagesLineEdit->setValidator(new QIntValidator(0, 10000, this));


// connections
connect(ui->typeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MediaDialog::onTypeChanged);
connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &MediaDialog::onAccepted);
connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &MediaDialog::onRejected);


// if editing existing media, populate fields
if (m_original) populateFromMedia(m_original);
onTypeChanged(ui->typeComboBox->currentIndex());
}


MediaDialog::~MediaDialog(){ delete ui; }


QSharedPointer<Media> MediaDialog::getResult() const { return m_result; }


QString MediaDialog::selectedType() const { return ui->typeComboBox->currentText(); }


void MediaDialog::onTypeChanged(int index) {
if (!ui) return;
ui->stackedWidget->setCurrentIndex(index);
}

void MediaDialog::populateFromMedia(const QSharedPointer<Media>& media) {
if (!media) return;
QJsonObject obj = media->toJson();
QString type = obj.value("type").toString();
int idx = MediaFactory::supportedTypes().indexOf(type);
if (idx >= 0) ui->typeComboBox->setCurrentIndex(idx);


if (type == "Book") {
ui->bookTitleLineEdit->setText(obj.value("title").toString());
ui->bookAuthorLineEdit->setText(obj.value("author").toString());
ui->bookYearLineEdit->setText(QString::number(obj.value("year").toInt()));
} else if (type == "Film") {
ui->filmTitleLineEdit->setText(obj.value("title").toString());
ui->filmDirectorLineEdit->setText(obj.value("director").toString());
ui->filmDurationLineEdit->setText(QString::number(obj.value("duration").toInt()));
} else if (type == "Article") {
ui->articleTitleLineEdit->setText(obj.value("title").toString());
ui->articleJournalLineEdit->setText(obj.value("journal").toString());
ui->articlePagesLineEdit->setText(QString::number(obj.value("pages").toInt()));
}
}


bool MediaDialog::validateInputs() const {
int idx = ui->typeComboBox->currentIndex();
if (idx < 0) return false;
if (idx == 0) return !ui->bookTitleLineEdit->text().trimmed().isEmpty();
if (idx == 1) return !ui->filmTitleLineEdit->text().trimmed().isEmpty();
if (idx == 2) return !ui->articleTitleLineEdit->text().trimmed().isEmpty();
return false;
}


QSharedPointer<Media> MediaDialog::buildMediaFromForm() const {
int idx = ui->typeComboBox->currentIndex();
QString type = MediaFactory::supportedTypes().value(idx);
QJsonObject obj;
if (type == "Book") {
obj["type"] = "Book";
obj["title"] = ui->bookTitleLineEdit->text();
obj["author"] = ui->bookAuthorLineEdit->text();
obj["year"] = ui->bookYearLineEdit->text().toInt();
} else if (type == "Film") {
obj["type"] = "Film";
obj["title"] = ui->filmTitleLineEdit->text();
obj["director"] = ui->filmDirectorLineEdit->text();
obj["duration"] = ui->filmDurationLineEdit->text().toInt();
} else if (type == "Article") {
obj["type"] = "Article";
obj["title"] = ui->articleTitleLineEdit->text();
obj["journal"] = ui->articleJournalLineEdit->text();
obj["pages"] = ui->articlePagesLineEdit->text().toInt();
}
return MediaFactory::createFromJson(obj);
}

void MediaDialog::onAccepted() {
if (!validateInputs()) {
// simple feedback; you can replace with QMessageBox
return;
}
m_result = buildMediaFromForm();
if (m_result) accept();
}


void MediaDialog::onRejected() {
m_result.clear();
reject();
}