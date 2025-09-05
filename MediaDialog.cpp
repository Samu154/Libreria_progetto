#include "MediaDialog.h"
#include "MediaFactory.h"
#include "Media.h"
#include "Book.h"
#include "Film.h"
#include "Article.h"

#include <QComboBox>
#include <QStackedWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIntValidator>
#include <QDialogButtonBox>

MediaDialog::MediaDialog(QWidget* parent, QSharedPointer<Media> mediaPassed)
    : QDialog(parent), m_original(mediaPassed)
{
    setWindowTitle(m_original ? tr("Modifica media") : tr("Nuovo media"));
    buildUi();
    if (m_original) populateFromMedia(m_original);
}

MediaDialog::~MediaDialog() = default;

void MediaDialog::buildUi() {
    m_typeCombo = new QComboBox(this);
    m_typeCombo->addItems(MediaFactory::supportedTypes());

    // Book page
    QWidget* bookPage = new QWidget(this);
    QFormLayout* bookLayout = new QFormLayout(bookPage);
    m_bookTitle = new QLineEdit(bookPage);
    m_bookAuthor = new QLineEdit(bookPage);
    m_bookYear = new QLineEdit(bookPage);
    m_bookYear->setValidator(new QIntValidator(0, 9999, this));
    bookLayout->addRow(tr("Titolo:"), m_bookTitle);
    bookLayout->addRow(tr("Autore:"), m_bookAuthor);
    bookLayout->addRow(tr("Anno:"), m_bookYear);

    // Film page
    QWidget* filmPage = new QWidget(this);
    QFormLayout* filmLayout = new QFormLayout(filmPage);
    m_filmTitle = new QLineEdit(filmPage);
    m_filmDirector = new QLineEdit(filmPage);
    m_filmDuration = new QLineEdit(filmPage);
    m_filmDuration->setValidator(new QIntValidator(0, 10000, this));
    filmLayout->addRow(tr("Titolo:"), m_filmTitle);
    filmLayout->addRow(tr("Regista:"), m_filmDirector);
    filmLayout->addRow(tr("Durata (min):"), m_filmDuration);

    // Article page
    QWidget* articlePage = new QWidget(this);
    QFormLayout* articleLayout = new QFormLayout(articlePage);
    m_articleTitle = new QLineEdit(articlePage);
    m_articleJournal = new QLineEdit(articlePage);
    m_articlePages = new QLineEdit(articlePage);
    m_articlePages->setValidator(new QIntValidator(0, 10000, this));
    articleLayout->addRow(tr("Titolo:"), m_articleTitle);
    articleLayout->addRow(tr("Rivista:"), m_articleJournal);
    articleLayout->addRow(tr("Pagine:"), m_articlePages);

    m_stack = new QStackedWidget(this);
    m_stack->addWidget(bookPage);
    m_stack->addWidget(filmPage);
    m_stack->addWidget(articlePage);

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    m_ok = qobject_cast<QPushButton*>(buttons->button(QDialogButtonBox::Ok));
    m_cancel = qobject_cast<QPushButton*>(buttons->button(QDialogButtonBox::Cancel));

    connect(m_typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MediaDialog::onTypeChanged);
    connect(buttons, &QDialogButtonBox::accepted, this, &MediaDialog::onAccept);
    connect(buttons, &QDialogButtonBox::rejected, this, &MediaDialog::onReject);

    QVBoxLayout* main = new QVBoxLayout(this);
    main->addWidget(m_typeCombo);
    main->addWidget(m_stack);
    main->addWidget(buttons);

    setLayout(main);
    onTypeChanged(m_typeCombo->currentIndex());
}

QSharedPointer<Media> MediaDialog::getResult() const {
    return m_result;
}

void MediaDialog::onTypeChanged(int index) {
    if (!m_stack) return;
    m_stack->setCurrentIndex(index);
}

void MediaDialog::populateFromMedia(const QSharedPointer<Media>& media) {
    if (!media) return;
    QJsonObject obj = media->toJson();
    QString type = obj.value("type").toString();
    int idx = MediaFactory::supportedTypes().indexOf(type);
    if (idx >= 0) m_typeCombo->setCurrentIndex(idx);

    if (type == "Book") {
        m_bookTitle->setText(obj.value("title").toString());
        m_bookAuthor->setText(obj.value("author").toString());
        m_bookYear->setText(QString::number(obj.value("year").toInt()));
    } else if (type == "Film") {
        m_filmTitle->setText(obj.value("title").toString());
        m_filmDirector->setText(obj.value("director").toString());
        m_filmDuration->setText(QString::number(obj.value("duration").toInt()));
    } else if (type == "Article") {
        m_articleTitle->setText(obj.value("title").toString());
        m_articleJournal->setText(obj.value("journal").toString());
        m_articlePages->setText(QString::number(obj.value("pages").toInt()));
    }
}

bool MediaDialog::validateInputs() const {
    int idx = m_typeCombo->currentIndex();
    if (idx < 0) return false;
    if (idx == 0) return !m_bookTitle->text().trimmed().isEmpty();
    if (idx == 1) return !m_filmTitle->text().trimmed().isEmpty();
    if (idx == 2) return !m_articleTitle->text().trimmed().isEmpty();
    return false;
}

QSharedPointer<Media> MediaDialog::buildMediaFromForm() const {
    int idx = m_typeCombo->currentIndex();
    QString type = MediaFactory::supportedTypes().value(idx);
    QJsonObject obj;
    if (type == "Book") {
        obj["type"] = "Book";
        obj["title"] = m_bookTitle->text();
        obj["author"] = m_bookAuthor->text();
        obj["year"] = m_bookYear->text().toInt();
    } else if (type == "Film") {
        obj["type"] = "Film";
        obj["title"] = m_filmTitle->text();
        obj["director"] = m_filmDirector->text();
        obj["duration"] = m_filmDuration->text().toInt();
    } else if (type == "Article") {
        obj["type"] = "Article";
        obj["title"] = m_articleTitle->text();
        obj["journal"] = m_articleJournal->text();
        obj["pages"] = m_articlePages->text().toInt();
    }
    return MediaFactory::createFromJson(obj);
}

void MediaDialog::onAccept() {
    if (!validateInputs()) {
        // minimal feedback; replace with QMessageBox for better UX
        return;
    }
    m_result = buildMediaFromForm();
    if (m_result) accept();
}

void MediaDialog::onReject() {
    m_result.clear();
    reject();
}
