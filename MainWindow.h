#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSharedPointer>
#include "MediaManager.h"
#include "MediaListModel.h"


class QListView;
class QLineEdit;
class QStackedWidget;
class QLabel;
class QAction;
class QSortFilterProxyModel;
class QScrollArea;
class QWidget;
class QItemSelectionModel;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onActionOpen();
    void onActionSave();
    void onActionSaveAs();
    void onActionExit();

    void onActionAdd();
    void onActionEdit();
    void onActionDelete();

    void onSearchTextChanged(const QString& txt);
    void onListSelectionChanged(const QModelIndex& current, const QModelIndex& previous);
    void onSelectionChanged(const QModelIndex& current, const QModelIndex& previous);

private:
    void setupUi();
    void setupConnections();
    void showDetailForIndex(int idx);

    QListView* m_listView = nullptr;
    QLineEdit* m_searchLineEdit = nullptr;
    QScrollArea* m_detailScroll = nullptr;
    QWidget* m_detailContainer = nullptr;
    QLabel* m_detailTitle = nullptr;

    QAction* m_actionOpen = nullptr;
    QAction* m_actionSave = nullptr;
    QAction* m_actionSaveAs = nullptr;
    QAction* m_actionExit = nullptr;
    QAction* m_actionAdd = nullptr;
    QAction* m_actionEdit = nullptr;
    QAction* m_actionDelete = nullptr;

    MediaManager m_manager;
    MediaListModel* m_model = nullptr;
    QSortFilterProxyModel* m_proxy = nullptr;

    QString m_currentFilePath;
    bool m_dirty = false;
};

#endif