#include "Book.h"
#include <QFormLayout>
#include <QLabel>
#include <QDebug>

Book::Book(const QString& title, const QString& author, int year) {
    this->title = title;
    this->author = author;
    this->year = year;
}

QString Book::getTitle() const {
    return title;
}

QString Book::getSummary() const {
    return QString("%1 (di %2, %3)").arg(title, author).arg(year);
}

QJsonObject Book::toJson() const {
    QJsonObject json;
    json["type"] = "Book";
    json["title"] = title;
    json["author"] = author;
    json["year"] = year;
    return json;
}

void Book::fromJson(const QJsonObject& json) {
    title = json["title"].toString();
    author = json["author"].toString();
    year = json["year"].toInt();
}

QWidget* Book::createDetailWidget(QWidget* parent) const {
    QWidget* widget = new QWidget(parent);
    QFormLayout* layout = new QFormLayout(widget);
    layout->addRow("Titolo:", new QLabel(title));
    layout->addRow("Autore:", new QLabel(author));
    layout->addRow("Anno:", new QLabel(QString::number(year)));
    return widget;
}

Media* Book::clone() const {
    return new Book(*this);
}

void Book::performAction() const {
    qDebug() << "Sto aprendo il libro:" << title;
}

QString Book::getAuthor() const {
    return author;
}

int Book::getYear() const {
    return year;
}
