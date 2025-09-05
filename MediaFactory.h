#ifndef MEDIAFACTORY_H
#define MEDIAFACTORY_H

#include <QString>
#include <QSharedPointer>
#include <QJsonObject>


class Media;

class MediaFactory {
public:
static QSharedPointer<Media> createFromJson(const QJsonObject& obj);
static QSharedPointer<Media> createEmptyOfType(const QString& type);
static QStringList supportedTypes();
};

#endif