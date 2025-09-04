#include "MediaFactory.h"
#include "Media.h"
#include "Book.h"
#include "Film.h"
#include "Article.h"


#include <QDebug>


QSharedPointer<Media> MediaFactory::createFromJson(const QJsonObject& obj) {
if (!obj.contains("type") || !obj["type"].isString()) {
qDebug() << "MediaFactory::createFromJson: missing or invalid 'type' field";
return {};
}


const QString type = obj.value("type").toString();
if (type == "Book") {
QSharedPointer<Book> b(new Book());
b->fromJson(obj);
return b;
}
if (type == "Film") {
QSharedPointer<Film> f(new Film());
f->fromJson(obj);
return f;
}
if (type == "Article") {
QSharedPointer<Article> a(new Article());
a->fromJson(obj);
return a;
}


qDebug() << "MediaFactory::createFromJson: unknown type:" << type;
return {};
}


QSharedPointer<Media> MediaFactory::createEmptyOfType(const QString& type) {
if (type == "Book") {
return QSharedPointer<Media>(new Book());
}
if (type == "Film") {
return QSharedPointer<Media>(new Film());
}
if (type == "Article") {
return QSharedPointer<Media>(new Article());
}
qDebug() << "MediaFactory::createEmptyOfType: unsupported type:" << type;
return {};
}


QStringList MediaFactory::supportedTypes() {
return {"Book", "Film", "Article"};
}