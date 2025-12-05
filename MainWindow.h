#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H
#include <QtWidgets/QMainWindow>
#include <QComboBox>
#include "ui_MainWindow.h"
#include "Disc.h"
#include <shobjidl_core.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    std::vector<Disc*> discs;
    void addToHistory(FileTree* tree);
    void connectSignals();
    void insertRoot(QIcon icon, QString name, QVariant object, QString type);
    void insertItem(QIcon icon, QString name, QString lba, QString size, QString date, Directory* dir);
    void insertChild(QTreeWidgetItem* parent, QIcon icon, QString name);
    void listItems();
    void listFileTree(FileTree* tree, bool history);
    std::vector<FileTree*> history;
    FileTree* current;
    size_t items = 0;
    size_t history_index = 0;
    const size_t MAX_HISTORY = 20;
    
public slots:
    void goBack();
    void goForward();
    void listTree();
    void listTrackItems();
    void extractDisc();
    void extractFiles();
    void openFile();
    void openFolder(const QTreeWidgetItem* item);
    void popUpMenu(const QPoint&);
    void popUpMenuFileList(const QPoint& pointer);
    void upOneFolder();
    void closeAll();
    void closeDisc();
    void exit();
private:
    FileTree* cur = nullptr;
    Ui::ISOBreakerQTClass ui;
};
#endif
