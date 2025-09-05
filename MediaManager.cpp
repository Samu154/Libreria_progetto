#include "MediaManager.h"
#include "Media.h"
#include "Book.h"
#include "Film.h"
#include "Article.h"


#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QDebug>


MediaManager::MediaManager(QObject* parent)
: QObject(parent)
{
}


void MediaManager::add(const QSharedPointer<Media>& media) {
m_items.append(media);
const int idx = m_items.size() - 1;
emit itemAdded(idx);
}


bool MediaManager::remove(int index) {
if (index < 0 || index >= m_items.size()) return false;
m_items.removeAt(index);
emit itemRemoved(index);
return true;
}


bool MediaManager::update(int index, const QSharedPointer<Media>& media) {
if (index < 0 || index >= m_items.size()) return false;
m_items[index] = media;
emit itemUpdated(index);
return true;
}


QSharedPointer<Media> MediaManager::at(int index) const {
if (index < 0 || index >= m_items.size()) return {};
return m_items.at(index);
}


int MediaManager::count() const {
return m_items.size();
}

void MediaManager::clear() {
m_items.clear();
emit cleared();
}


bool MediaManager::saveToFile(const QString& path, QString* error) const {
QJsonArray array;
for (const auto& m : m_items) {
if (!m) continue;
array.append(m->toJson());
}
QJsonDocument doc(array);
QFile f(path);
if (!f.open(QIODevice::WriteOnly)) {
if (error) *error = QString("Impossibile aprire il file in scrittura: %1").arg(path);
return false;
}
qint64 written = f.write(doc.toJson());
f.close();
if (written <= 0) {
if (error) *error = QString("Errore scrittura file: %1").arg(path);
return false;
}
return true;
}


bool MediaManager::loadFromFile(const QString& path, QString* err) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        if (err) *err = tr("Impossibile aprire il file: %1").arg(path);
        return false;
    }

    const QByteArray all = f.readAll();
    f.close();

    QJsonParseError perr;
    QJsonDocument doc = QJsonDocument::fromJson(all, &perr);
    if (perr.error != QJsonParseError::NoError) {
        if (err) *err = tr("Errore parsing JSON: %1").arg(perr.errorString());
        return false;
    }

    QJsonArray arr;

    if (doc.isArray()) {
        // formato semplice: root è direttamente un array di items
        arr = doc.array();
    } else if (doc.isObject()) {
        QJsonObject root = doc.object();
        // se c'è "items" e è un array usalo
        if (root.contains("items") && root.value("items").isArray()) {
            arr = root.value("items").toArray();
        } else if (root.contains("items")) {
            if (err) *err = tr("'items' presente ma non è un array nel file: %1").arg(path);
            return false;
        } else {
            // fallback: se l'oggetto ha campi che somigliano a media (uno solo), potremmo decidere come gestire.
            if (err) *err = tr("Formato JSON non valido: atteso un array o un oggetto con campo 'items'.");
            return false;
        }
    } else {
        if (err) *err = tr("Formato JSON non valido (né array né object).");
        return false;
    }

    // parse items in arr -> creazione media
    QVector<QSharedPointer<Media>> tmp;
    tmp.reserve(arr.size());
    for (int i = 0; i < arr.size(); ++i) {
        QJsonValue v = arr.at(i);
        if (!v.isObject()) {
            qWarning() << "Elemento non è oggetto JSON in posizione" << i << "- salto";
            continue;
        }
        QJsonObject jo = v.toObject();
        // MediaFactory::createFromJson deve restituire QSharedPointer<Media> o nullptr
        QSharedPointer<Media> m = MediaFactory::createFromJson(jo);
        if (!m) {
            qWarning() << "Elemento JSON non convertibile in Media in posizione" << i << ":" << jo;
            continue;
        }
        tmp.append(m);
    }

    // Se vogliamo sostituire il contenuto solo se abbiamo almeno 1 elemento (opzionale)
    // oppure accettiamo anche file vuoti: qui sostituiamo comunque
    clear(); // implementa clear() per svuotare m_items e notificare il model
    for (auto &mm : tmp) add(mm);

    return true;
}


/*QJsonArray array = doc.array();


// ricostruisci i media in memoria
QVector<QSharedPointer<Media>> newItems;
newItems.reserve(array.size());
for (const QJsonValue& v : array) {
if (!v.isObject()) continue;
QJsonObject obj = v.toObject();
QSharedPointer<Media> m = createMediaFromJson(obj);
if (m) newItems.append(m);
}


// sostituisci la collezione corrente con la nuova
m_items = std::move(newItems);
emit cleared();
// emetti itemAdded per ogni elemento (utile per model)
for (int i = 0; i < m_items.size(); ++i) emit itemAdded(i);


return true;
}

QSharedPointer<Media> MediaManager::createMediaFromJson(const QJsonObject& obj) const {
// Atteso: campo "type" presente
QString type = obj.value("type").toString();
if (type == "Book") {
QSharedPointer<Book> b(new Book());
b->fromJson(obj);
return b;
}
if (type == "Film") {
QSharedPointer<Film> f(new Film());
f->fromJson(obj);
return f;
}
if (type == "Article") {
QSharedPointer<Article> a(new Article());
a->fromJson(obj);
return a;
}
// Unknown type -> ignore
qDebug() << "MediaManager: tipo sconosciuto:" << type;
return {};
}