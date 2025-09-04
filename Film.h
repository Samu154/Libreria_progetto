#ifndef FILM_H
#define FILM_H


#include "Media.h"
#include "MediaVisitor.h"
#include <QString>
#include <QJsonObject>


class Film : public Media {
public:
Film(const QString& title = "", const QString& director = "", int duration = 0);


QString getTitle() const override;
QString getSummary() const override;


QJsonObject toJson() const override;
void fromJson(const QJsonObject& json) override;


Media* clone() const override;
void performAction() const override;


void accept(MediaVisitor& visitor) const override;


QString getDirector() const;
int getDuration() const;


private:
QString director;
int duration; // in minutes
};


#endif // FILM_H