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


class MainWindow : public QMainWindow {
Q_OBJECT
public:
explicit MainWindow(QWidget* parent = nullptr);
~MainWindow() override;


private slots:
void onActionOpen();
void onActionSave();
void onActionExit();
void onActionAdd();
void onActionEdit();
void onActionDelete();
void onSearchTextChanged(const QString& txt);
void onListSelectionChanged(const QModelIndex& current, const QModelIndex& previous);


private:
void setupUi();
void setupConnections();
void showDetailForIndex(int idx);


// widgets
QListView* m_listView = nullptr;
QLineEdit* m_searchLineEdit = nullptr;
QStackedWidget* m_stackedWidget = nullptr;
QWidget* m_detailPlaceholder = nullptr;


// actions
QAction* m_actionOpen = nullptr;
QAction* m_actionSave = nullptr;
QAction* m_actionExit = nullptr;
QAction* m_actionAdd = nullptr;
QAction* m_actionEdit = nullptr;
QAction* m_actionDelete = nullptr;


// model & manager
MediaManager m_manager;
MediaListModel* m_model = nullptr; // owns by Qt parent
QSortFilterProxyModel* m_proxy = nullptr;


QString m_currentFilePath;
bool m_dirty = false;
};


#endif // MAINWINDOW_H