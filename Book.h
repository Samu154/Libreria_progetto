#ifndef BOOK_H
#define BOOK_H


#include "Media.h"
#include "MediaVisitor.h"
#include <QString>
#include <QJsonObject>


class Book : public Media {
public:
Book(const QString& title = "", const QString& author = "", int year = 0);


QString getTitle() const override;
QString getSummary() const override;


QJsonObject toJson() const override;
void fromJson(const QJsonObject& json) override;


Media* clone() const override;
void performAction() const override; // es.: logging/azione dominio


// Hook per comportamenti esterni (GUI, export, ecc.)
void accept(MediaVisitor& visitor) const override;


// Accessors specifici del dominio
QString getAuthor() const;
int getYear() const;


private:
QString author;
int year;
};


#endif // BOOK_H