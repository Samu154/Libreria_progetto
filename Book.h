@ -0,0 +1,34 @@
// Book.h
#ifndef BOOK_H
#define BOOK_H

#include "Media.h"
#include <QString>
#include <QJsonObject>
#include <QWidget>

class Book : public Media {
public:
    Book(const QString& title = "", const QString& author = "", int year = 0);

    QString getTitle() const override;
    QString getSummary() const override;

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;

    QWidget* createDetailWidget(QWidget* parent = nullptr) const override;
    Media* clone() const override;

    void performAction() const override; // Es: "Apri il libro"

    QString getAuthor() const;
    int getYear() const;

private:
    QString author;
    int year;
};

#endif // BOOK_H