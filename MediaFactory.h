#ifndef MEDIAFACTORY_H
#define MEDIAFACTORY_H


#include <QString>
#include <QSharedPointer>
#include <QJsonObject>


class Media; // forward


// MediaFactory: crea istanze concrete di Media a partire da dati (es. JSON)
// - Non ha dipendenze GUI (rispetta il vincolo di separazione Model/GUI)
// - Restituisce QSharedPointer<Media>
// - Fornisce funzioni helper per enumerare i tipi supportati


class MediaFactory {
public:
// Crea un oggetto Media a partire da un QJsonObject che DEVE contenere il campo "type"
// Restituisce un QSharedPointer vuoto in caso di tipo non riconosciuto o errore
static QSharedPointer<Media> createFromJson(const QJsonObject& obj);


// Crea un oggetto Media "vuoto" dato un tipo (es. "Book", "Film", "Article")
// Utile per i dialog di creazione: l'UI popolerà poi i campi.
static QSharedPointer<Media> createEmptyOfType(const QString& type);


// Lista dei tipi supportati (utile per popolare combo in GUI senza hardcode)
static QStringList supportedTypes();
};


#endif // MEDIAFACTORY_H