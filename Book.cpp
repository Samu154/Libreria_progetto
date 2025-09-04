#include "Book.h"
#include <QDebug>


Book::Book(const QString& title, const QString& author, int year) {
this->title = title;
this->author = author;
this->year = year;
}


QString Book::getTitle() const { return title; }


QString Book::getSummary() const {
return QString("%1 (di %2, %3)").arg(title, author).arg(year);
}


QJsonObject Book::toJson() const {
QJsonObject json;
json["type"] = "Book"; // utile per factory esterna durante il load
json["title"] = title;
json["author"] = author;
json["year"] = year;
return json;
}


void Book::fromJson(const QJsonObject& json) {
// Robustezza minima (vincolo 7)
if (json.contains("title") && json["title"].isString())
title = json["title"].toString();
if (json.contains("author") && json["author"].isString())
author = json["author"].toString();
if (json.contains("year") && json["year"].isDouble())
year = json["year"].toInt();
}


Media* Book::clone() const { return new Book(*this); }


void Book::performAction() const {
// Azione di dominio "neutra" rispetto alla GUI (es. logging)
qDebug() << "[Book::performAction] Apri libro:" << title;
}


void Book::accept(MediaVisitor& visitor) const { visitor.visit(*this); }


QString Book::getAuthor() const { return author; }
int Book::getYear() const { return year; }