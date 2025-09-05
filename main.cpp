#include <QApplication>
#include <QDebug>
#include <QJsonDocument>
#include "Book.h"
#include "Film.h"
#include "Article.h"
#include "MainWindow.h"
#include <QApplication>

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    Book b("Il nome della rosa", "Umberto Eco", 1980);
    qDebug() << "Titolo:" << b.getTitle();
    qDebug() << "Summary:" << b.getSummary();

    // serializza in json e stampa
    QJsonObject j = b.toJson();
    QJsonDocument doc(j);
    qDebug() << "JSON compatto:" << doc.toJson(QJsonDocument::Compact);

    // test della azione polimorfica
    b.performAction();

    // Non avviamo l'event loop (app.exec()) perché questo main è
    // solo un test console. Se poi hai GUI, usa app.exec().
    return 0;
}
