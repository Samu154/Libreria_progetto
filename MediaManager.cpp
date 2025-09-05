#include "MediaManager.h"
#include "Media.h"
#include "Book.h"
#include "Film.h"
#include "Article.h"
#include "MediaFactory.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QDebug>


MediaManager::MediaManager(QObject* parent)
: QObject(parent)
{}

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
        arr = doc.array();
    } else if (doc.isObject()) {
        QJsonObject root = doc.object();
        if (root.contains("items") && root.value("items").isArray()) {
            arr = root.value("items").toArray();
        } else if (root.contains("items")) {
            if (err) *err = tr("'items' presente ma non è un array nel file: %1").arg(path);
            return false;
        } else {
            if (err) *err = tr("Formato JSON non valido: atteso un array o un oggetto con campo 'items'.");
            return false;
        }
    } else {
        if (err) *err = tr("Formato JSON non valido (né array né object).");
        return false;
    }
    QVector<QSharedPointer<Media>> tmp;
    tmp.reserve(arr.size());
    for (int i = 0; i < arr.size(); ++i) {
        QJsonValue v = arr.at(i);
        if (!v.isObject()) {
            qWarning() << "Elemento non è oggetto JSON in posizione" << i << "- salto";
            continue;
        }
        QJsonObject jo = v.toObject();
        QSharedPointer<Media> m = MediaFactory::createFromJson(jo);
        if (!m) {
            qWarning() << "Elemento JSON non convertibile in Media in posizione" << i << ":" << jo;
            continue;
        }
        tmp.append(m);
    }
    clear();
    for (auto &mm : tmp) add(mm);
    return true;
}