#include "Film.h"
#include <QDebug>


Film::Film(const QString& title, const QString& director, int duration) {
this->title = title;
this->director = director;
this->duration = duration;
}


QString Film::getTitle() const { return title; }


QString Film::getSummary() const {
return QString("%1 (regia: %2, %3 min)").arg(title, director).arg(duration);
}


QJsonObject Film::toJson() const {
QJsonObject json;
json["type"] = "Film";
json["title"] = title;
json["director"] = director;
json["duration"] = duration;
return json;
}


void Film::fromJson(const QJsonObject& json) {
if (json.contains("title") && json["title"].isString())
title = json["title"].toString();
if (json.contains("director") && json["director"].isString())
director = json["director"].toString();
if (json.contains("duration") && json["duration"].isDouble())
duration = json["duration"].toInt();
}


Media* Film::clone() const { return new Film(*this); }


void Film::performAction() const {
// Azione di dominio neutra: ad esempio "riproduci" loggato
qDebug() << "[Film::performAction] Riproduci film:" << title;
}


void Film::accept(MediaVisitor& visitor) const { visitor.visit(*this); }


QString Film::getDirector() const { return director; }
int Film::getDuration() const { return duration; }