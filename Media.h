#ifndef MEDIA_H
#define MEDIA_H

#include <QString>
#include <QJsonObject>


class MediaVisitor;

class Media {
public:
virtual ~Media() = default;
virtual QString getTitle() const = 0;
virtual QString getSummary() const = 0;
virtual QJsonObject toJson() const = 0;
virtual void fromJson(const QJsonObject& json) = 0;
virtual Media* clone() const = 0;
virtual void performAction() const = 0;
virtual void accept(MediaVisitor& visitor) const = 0;

protected:
QString title;
};

#endif