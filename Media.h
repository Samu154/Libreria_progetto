#ifndef MEDIA_H
#define MEDIA_H

#include <QString>
#include <QJsonObject>
#include <QWidget>

// Classe astratta che rappresenta un generico media
class Media {
public:
    virtual ~Media() = default;

    // Restituisce il titolo del media
    virtual QString getTitle() const = 0;

    // Restituisce una breve descrizione da visualizzare nella lista
    virtual QString getSummary() const = 0;

    // Serializza l'oggetto in formato JSON
    virtual QJsonObject toJson() const = 0;

    // Carica l'oggetto da una rappresentazione JSON
    virtual void fromJson(const QJsonObject& json) = 0;

    // Crea un widget dettagliato per la visualizzazione o modifica del media
    virtual QWidget* createDetailWidget(QWidget* parent = nullptr) const = 0;

    // Clonazione polimorfica (necessaria per copiare i media in modo corretto)
    virtual Media* clone() const = 0;

    // Operazione specifica del media (es. "apri", "riproduci")
    virtual void performAction() const = 0;

protected:
    QString title;
};

#endif // MEDIA_H