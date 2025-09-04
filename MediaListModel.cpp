#include "MediaListModel.h"
#include "MediaManager.h"
#include "Media.h"

#include <QDebug>

MediaListModel::MediaListModel(MediaManager* manager, QObject* parent)
    : QAbstractListModel(parent), m_manager(nullptr)
{
    if (manager) setManager(manager);
}

int MediaListModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    if (!m_manager) return 0;
    return m_manager->count();
}

QVariant MediaListModel::data(const QModelIndex& index, int role) const {
    if (!m_manager) return {};
    if (!index.isValid()) return {};
    int row = index.row();
    if (row < 0 || row >= m_manager->count()) return {};

    QSharedPointer<Media> m = m_manager->at(row);
    if (!m) return {};

    switch (role) {
    case Qt::DisplayRole:
    case TitleRole:
        return m->getTitle();
    case SummaryRole:
        return m->getSummary();
    case PtrRole:
        return QVariant::fromValue(reinterpret_cast<quintptr>(m.data()));
    default:
        return {};
    }
}

QHash<int, QByteArray> MediaListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[SummaryRole] = "summary";
    roles[PtrRole] = "ptr";
    return roles;
}

void MediaListModel::setManager(MediaManager* manager) {
    if (m_manager == manager) return;

    // disconnect old
    if (m_manager) {
        disconnect(m_manager, &MediaManager::itemAdded, this, &MediaListModel::onItemAdded);
        disconnect(m_manager, &MediaManager::itemRemoved, this, &MediaListModel::onItemRemoved);
        disconnect(m_manager, &MediaManager::itemUpdated, this, &MediaListModel::onItemUpdated);
        disconnect(m_manager, &MediaManager::cleared, this, &MediaListModel::onCleared);
    }

    beginResetModel();
    m_manager = manager;
    endResetModel();

    if (m_manager) {
        connect(m_manager, &MediaManager::itemAdded, this, &MediaListModel::onItemAdded);
        connect(m_manager, &MediaManager::itemRemoved, this, &MediaListModel::onItemRemoved);
        connect(m_manager, &MediaManager::itemUpdated, this, &MediaListModel::onItemUpdated);
        connect(m_manager, &MediaManager::cleared, this, &MediaListModel::onCleared);
    }
}

void MediaListModel::onItemAdded(int index) {
    beginInsertRows(QModelIndex(), index, index);
    endInsertRows();
}

void MediaListModel::onItemRemoved(int index) {
    beginRemoveRows(QModelIndex(), index, index);
    endRemoveRows();
}

void MediaListModel::onItemUpdated(int index) {
    QModelIndex idx = createIndex(index, 0);
    emit dataChanged(idx, idx, {Qt::DisplayRole, TitleRole, SummaryRole});
}

void MediaListModel::onCleared() {
    beginResetModel();
    endResetModel();
}
