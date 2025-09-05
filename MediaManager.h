#ifndef MEDIAMANAGER_H
#define MEDIAMANAGER_H

#include <QObject>
#include <QVector>
#include <QSharedPointer>
#include <QString>
#include <QJsonObject>


class Media;

class MediaManager : public QObject {
Q_OBJECT
public:
explicit MediaManager(QObject* parent = nullptr);
~MediaManager() override = default;
void add(const QSharedPointer<Media>& media);
bool remove(int index);
bool update(int index, const QSharedPointer<Media>& media);
QSharedPointer<Media> at(int index) const;
int count() const;
void clear();
bool saveToFile(const QString& path, QString* error = nullptr) const;
bool loadFromFile(const QString& path, QString* error = nullptr);

signals:
void itemAdded(int index);
void itemRemoved(int index);
void itemUpdated(int index);
void cleared();

private:
QVector<QSharedPointer<Media>> m_items;
QSharedPointer<Media> createMediaFromJson(const QJsonObject& obj) const;
};

#endif