#ifndef MEDIALISTMODEL_H
#define MEDIALISTMODEL_H


#include <QAbstractListModel>
#include <QSharedPointer>
#include <QVector>


class MediaManager;
class Media;


class MediaListModel : public QAbstractListModel {
Q_OBJECT
public:
enum Roles {
TitleRole = Qt::UserRole + 1,
SummaryRole,
PtrRole,
TypeRole
};


explicit MediaListModel(MediaManager* manager = nullptr, QObject* parent = nullptr);


// QAbstractItemModel overrides
int rowCount(const QModelIndex& parent = QModelIndex()) const override;
QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
QHash<int, QByteArray> roleNames() const override;


// utility
void setManager(MediaManager* manager);
MediaManager* manager() const { return m_manager; }


private slots:
void onItemAdded(int index);
void onItemRemoved(int index);
void onItemUpdated(int index);
void onCleared();


private:
MediaManager* m_manager = nullptr; // non ownership
};


#endif // MEDIALISTMODEL_H