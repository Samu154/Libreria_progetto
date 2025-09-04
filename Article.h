#ifndef ARTICLE_H
#define ARTICLE_H


#include "Media.h"
#include "MediaVisitor.h"
#include <QString>
#include <QJsonObject>


class Article : public Media {
public:
Article(const QString& title = "", const QString& journal = "", int pages = 0);


QString getTitle() const override;
QString getSummary() const override;


QJsonObject toJson() const override;
void fromJson(const QJsonObject& json) override;


Media* clone() const override;
void performAction() const override;


void accept(MediaVisitor& visitor) const override;


QString getJournal() const;
int getPages() const;


private:
QString journal;
int pages;
};


#endif // ARTICLE_H