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


bool MediaManager::loadFromFile(const QString& path, QString* error) {
QFile f(path);
if (!f.open(QIODevice::ReadOnly)) {
if (error) *error = QString("Impossibile aprire il file in lettura: %1").arg(path);
return false;
}
const QByteArray data = f.readAll();
f.close();
QJsonParseError jerr;
QJsonDocument doc = QJsonDocument::fromJson(data, &jerr);
if (jerr.error != QJsonParseError::NoError) {
if (error) *error = QString("Errore parsing JSON: %1").arg(jerr.errorString());
return false;
}
if (!doc.isArray()) {
if (error) *error = QString("Formato file non valido: aspettato array JSON");
return false;
}


QJsonArray array = doc.array();


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