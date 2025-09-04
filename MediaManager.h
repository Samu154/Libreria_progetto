#ifndef MEDIAMANAGER_H
#define MEDIAMANAGER_H


#include <QObject>
#include <QVector>
#include <QSharedPointer>
#include <QString>
#include <QJsonObject>


class Media; // forward


// MediaManager: gestisce la collezione di Media e la persistenza JSON.
// - È un QObject (Qt Core) per permettere segnali verso la GUI/Model
// - Usa QSharedPointer<Media> per ownership sicura


class MediaManager : public QObject {
Q_OBJECT
public:
explicit MediaManager(QObject* parent = nullptr);
~MediaManager() override = default;


// CRUD base
void add(const QSharedPointer<Media>& media); // aggiunge in coda
bool remove(int index); // rimuove by index
bool update(int index, const QSharedPointer<Media>& media); // sostituisce


QSharedPointer<Media> at(int index) const;
int count() const;
void clear();


// Persistenza
bool saveToFile(const QString& path, QString* error = nullptr) const;
bool loadFromFile(const QString& path, QString* error = nullptr);


signals:
// segnali per notificare cambiamenti (utile per MediaListModel / Presenter)
void itemAdded(int index);
void itemRemoved(int index);
void itemUpdated(int index);
void cleared();


private:
QVector<QSharedPointer<Media>> m_items;


// helper factory che costruisce un oggetto Media a partire da QJsonObject
// Implementata internamente per non dipendere da una factory esterna.
QSharedPointer<Media> createMediaFromJson(const QJsonObject& obj) const;
};


#endif // MEDIAMANAGER_H