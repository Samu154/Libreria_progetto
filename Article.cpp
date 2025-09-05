#include "Article.h"
#include <QDebug>


Article::Article(const QString& title, const QString& journal, int pages) {
this->title = title;
this->journal = journal;
this->pages = pages;
}

QString Article::getTitle() const { return title; }

QString Article::getSummary() const {
return QString("%1 (rivista: %2, %3 pagine)").arg(title, journal).arg(pages);
}

QJsonObject Article::toJson() const {
QJsonObject json;
json["type"] = "Article";
json["title"] = title;
json["journal"] = journal;
json["pages"] = pages;
return json;
}

void Article::fromJson(const QJsonObject& json) {
if (json.contains("title") && json["title"].isString())
title = json["title"].toString();
if (json.contains("journal") && json["journal"].isString())
journal = json["journal"].toString();
if (json.contains("pages") && json["pages"].isDouble())
pages = json["pages"].toInt();
}

Media* Article::clone() const { return new Article(*this); }

void Article::performAction() const {
qDebug() << "[Article::performAction] Apri articolo:" << title;
}

void Article::accept(MediaVisitor& visitor) const { visitor.visit(*this); }

QString Article::getJournal() const { return journal; }
int Article::getPages() const { return pages; }