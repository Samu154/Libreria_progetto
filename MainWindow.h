#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSharedPointer>

class QListView;
class QLineEdit;
class QStackedWidget;
class QLabel;
class QAction;
class QSortFilterProxyModel;
class QScrollArea;
class QWidget;
class QItemSelectionModel;

#include "MediaManager.h"
#include "MediaListModel.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    // file/menu actions
    void onActionOpen();
    void onActionSave();
    void onActionSaveAs();
    void onActionExit();

    // CRUD actions
    void onActionAdd();
    void onActionEdit();
    void onActionDelete();

    // UI interactions
    void onSearchTextChanged(const QString& txt);
    void onListSelectionChanged(const QModelIndex& current, const QModelIndex& previous);
    void onSelectionChanged(const QModelIndex& current, const QModelIndex& previous);

private:
    void setupUi();
    void setupConnections();
    void showDetailForIndex(int idx);

    // widgets
    QListView* m_listView = nullptr;
    QLineEdit* m_searchLineEdit = nullptr;
    QScrollArea* m_detailScroll = nullptr;      // scroll area that contains detailContainer
    QWidget* m_detailContainer = nullptr;       // container where detail widgets are inserted
    QLabel* m_detailTitle = nullptr;            // large title label in detail pane

    // actions
    QAction* m_actionOpen = nullptr;
    QAction* m_actionSave = nullptr;
    QAction* m_actionSaveAs = nullptr;
    QAction* m_actionExit = nullptr;
    QAction* m_actionAdd = nullptr;
    QAction* m_actionEdit = nullptr;
    QAction* m_actionDelete = nullptr;

    // model & manager
    MediaManager m_manager;
    MediaListModel* m_model = nullptr; // owned by Qt parent
    QSortFilterProxyModel* m_proxy = nullptr;

    QString m_currentFilePath;
    bool m_dirty = false;
};

#endif // MAINWINDOW_H
