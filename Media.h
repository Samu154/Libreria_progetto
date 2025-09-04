#ifndef MEDIA_H
#define MEDIA_H


#include <QString>
#include <QJsonObject>


class MediaVisitor; // forward declaration (no GUI)


// Interfaccia astratta del dominio: nessuna dipendenza da QWidget/GUI
class Media {
public:
virtual ~Media() = default;


// Dati/contratto di dominio
virtual QString getTitle() const = 0;
virtual QString getSummary() const = 0;


// Persistenza strutturata (consentita dal vincolo 6)
virtual QJsonObject toJson() const = 0;
virtual void fromJson(const QJsonObject& json) = 0;


// Polimorfismo utile (no GUI): clonazione e azione specifica
virtual Media* clone() const = 0;
virtual void performAction() const = 0;


// Visitor per estendere comportamenti dall'esterno (es. GUI builders)
virtual void accept(MediaVisitor& visitor) const = 0;


protected:
QString title;
};


#endif // MEDIA_H