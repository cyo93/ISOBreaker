#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    connectSignals();
    QIcon icon;
}

MainWindow::~MainWindow()
{
    for (size_t i = 0; i < discs.size(); i++) {
        delete discs[i];
    }
}

void MainWindow::addToHistory(FileTree* tree) {
    if (history.size() >= MAX_HISTORY && history_index == history.size()) {
        history.erase(history.begin() + 0);
    }
    if (history_index != history.size() - 1 && history.size() != 0) {
        history.erase((history.begin() + history_index + 1), history.end());
    }
    history.push_back(tree);
    history_index = history.size() - 1;
}

void MainWindow::goBack() {
    if (history_index > 0) {
        history_index--;
        listFileTree(history[history_index], true);
    }
}

void MainWindow::goForward() {
    if (history_index + 1 < history.size()) {
        history_index++;
        listFileTree(history[history_index], true);
    }
}

void MainWindow::connectSignals() {
    connect(ui.actionOpen, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui.actionExit, &QAction::triggered, this, &MainWindow::exit);
    connect(ui.actionClose, &QAction::triggered, this, &MainWindow::closeAll);
    connect(ui.back, &QPushButton::clicked, this, &MainWindow::goBack);
    connect(ui.closeButton, &QPushButton::clicked, this, &MainWindow::closeDisc);
    connect(ui.forward, &QPushButton::clicked, this, &MainWindow::goForward);
    connect(ui.fileTree, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(popUpMenu(const QPoint&)));
    connect(ui.files, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(popUpMenuFileList(const QPoint&)));
    connect(ui.files, &QTreeWidget::itemDoubleClicked, this, &MainWindow::openFolder);
    connect(ui.disclist, &QComboBox::currentIndexChanged, this, &MainWindow::listTree);
    connect(ui.fileTree, &QTreeWidget::currentItemChanged, this, &MainWindow::listItems);
    connect(ui.upFolder, &QPushButton::clicked, this, &MainWindow::upOneFolder);
}

void MainWindow::insertRoot(QIcon icon, QString name, QVariant object, QString type) {
    QTreeWidgetItem* item = new QTreeWidgetItem(ui.fileTree);
    item->setText(0, name);
    item->setIcon(0, icon);
    item->setData(2, 0, object);
    item->setData(3, 0, type);
    ui.fileTree->addTopLevelItem(item);
}

void MainWindow::insertChild(QTreeWidgetItem* parent, QIcon icon, QString name) {
    QTreeWidgetItem* item = new QTreeWidgetItem(ui.fileTree);
    item->setText(0, name);
    parent->addChild(item);
}

void MainWindow::listTree() {
    size_t index = ui.disclist->currentIndex();
    ui.fileTree->clear();
    if (ui.disclist->count() != 0) {
        for (size_t i = 0; i < discs[index]->getTracksSize(); i++) {
            QIcon icon;
            if (discs[index]->tracks[i]->disc->disctype == "audio") {
                icon.addFile("Icons\\Audio.svg");
            }
            else {
                icon.addFile("Icons\\Disc.svg");
            }
            QString id;
            if (discs[index]->tracks[i]->disc->pvd != nullptr) {
                id = QString(discs[index]->tracks[i]->disc->pvd->volIdentifier);
                id = isEmpty(id.toStdWString()) ? "" : id;
            }
            else {
                id = "";
            }
            insertRoot(icon, (id != "" ? id : "Track " + QString::number(i + 1)), QVariant::fromValue(discs[index]->tracks[i]), "disc");
        }
    }
}

void MainWindow::listItems() {
    ui.files->clear();
    current = nullptr;
    history.clear();
    history_index = 0;
    if (ui.fileTree->currentItem() != nullptr) {
        Track* track = ui.fileTree->currentItem()->data(2, 0).value<Track*>();
        FileTree* tree = track->disc->filetree;
        listFileTree(tree, false);
    }
}

void MainWindow::listFileTree(FileTree* tree, bool history) {
    ui.files->clear();
    Track* track = ui.fileTree->currentItem()->data(2, 0).value<Track*>();
    QIcon folder;
    folder.addFile("Icons\\Folder.svg");
    QIcon file;
    file.addFile("Icons\\File.svg");
    if (tree != nullptr) {
        for (size_t i = 0; i < tree->dirs->size(); i++) {
            if (tree->dirs->at(i)->folder == true) {
                insertItem(folder, QString::fromStdWString(tree->dirs->at(i)->id),
                    (track->disc->disctype == "gamecube") ? QString::fromStdWString(std::to_wstring((tree->dirs->at(i)->startSector) / 2048)) : QString::fromStdWString(std::to_wstring(tree->dirs->at(i)->startSector)),
                    QString::fromStdWString(fileSizeConversion(tree->dirs->at(i)->fileSize)), (tree->dirs->at(i)->date != nullptr) ? QString::fromStdString(dateFormat(tree->dirs->at(i)->date)) : "N/A", tree->dirs->at(i));
            }
            else {
                insertItem(file, QString::fromStdWString(tree->dirs->at(i)->id),
                    (track->disc->disctype == "gamecube") ? QString::fromStdWString(std::to_wstring((tree->dirs->at(i)->startSector) / 2048)) : QString::fromStdWString(std::to_wstring(tree->dirs->at(i)->startSector)),
                    QString::fromStdWString(fileSizeConversion(tree->dirs->at(i)->fileSize)), (tree->dirs->at(i)->date != nullptr) ? QString::fromStdString(dateFormat(tree->dirs->at(i)->date)) : "N/A", tree->dirs->at(i));
            }
        }
    }
    current = tree;
    if (!history) {
        if (current != nullptr) {
            addToHistory(current);
        }
    }
}


void MainWindow::insertItem(QIcon icon, QString name, QString lba, QString size, QString date, Directory* dir) {
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setIcon(0, icon);
    item->setText(0, name);
    item->setData(4, 1, QVariant::fromValue(dir));
    item->setText(1, size);
    item->setText(2, date);
    item->setText(3, lba);
    item->setData(4, 0, QVariant::fromValue(dir));
    ui.files->addTopLevelItem(item);
}

void MainWindow::popUpMenu(const QPoint& pointer) {
    QMenu popup(ui.fileTree);
    Track* track = ui.fileTree->currentItem()->data(2, 0).value<Track*>();
    QAction extract("Extract Track");
    if (track->disc->disctype == "unknown") {
        return;
    }
    else if (track->disc->disctype == "audio") {
        extract.setText("Extract Audio");
    }
    else {
        extract.setText("Extract all files from track");
    }
    
    connect(&extract, &QAction::triggered, this, &MainWindow::extractDisc);
    popup.addAction(&extract);
    popup.exec(ui.fileTree->mapToGlobal(pointer));
}

void MainWindow::popUpMenuFileList(const QPoint& pointer) {
    QMenu popup(ui.files);
    QAction extract("Extract File/Folder");
    connect(&extract, &QAction::triggered, this, &MainWindow::extractFiles);
    popup.addAction(&extract);
    popup.exec(ui.files->mapToGlobal(pointer));
}

void MainWindow::listTrackItems() {
    size_t index = ui.disclist->currentIndex();
}

void MainWindow::closeAll() {
    while (ui.disclist->count() > 0) {
        closeDisc();
    }
}

void MainWindow::closeDisc() {
    size_t index = ui.disclist->currentIndex();
    if (ui.disclist->count() > 0) {
        ui.disclist->removeItem(index);
        delete discs[index];
        discs.erase(discs.begin() + index);
        listTree();
    }
}

void MainWindow::extractDisc() {
    IFileOpenDialog* dialog;
    HRESULT result = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dialog));
    if (SUCCEEDED(result)) {
        DWORD options;
        result = dialog->GetOptions(&options);
        if (SUCCEEDED(result)) {
            result = dialog->SetOptions(options | FOS_PICKFOLDERS);
            if (SUCCEEDED(result)){
                result = dialog->Show(NULL);
                if (SUCCEEDED(result)) {
                    IShellItem* path;
                    result = dialog->GetResult(&path);
                    Track* track = ui.fileTree->currentItem()->data(2,0).value<Track*>();
                    LPWSTR folder;
                    result = path->GetDisplayName(SIGDN_FILESYSPATH, &folder);
                    if (SUCCEEDED(result)) {
                        if (track->disc->disctype == "audio") {
                            track->disc->extractAudio(folder, 1);
                        }
                        else{
                            discs[ui.disclist->currentIndex()]->extractAll(track, folder);
                        }
                        
                    }
                    path->Release();
                }

            }
        }
        dialog->Release();
    }
}

void MainWindow::extractFiles() {
    IFileOpenDialog* dialog;
    HRESULT result = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dialog));
    if (SUCCEEDED(result)) {
        DWORD options;
        result = dialog->GetOptions(&options);
        if (SUCCEEDED(result)) {
            result = dialog->SetOptions(options | FOS_PICKFOLDERS);
            if (SUCCEEDED(result)) {
                result = dialog->Show(NULL);
                if (SUCCEEDED(result)) {
                    IShellItem* path;
                    result = dialog->GetResult(&path);
                    Track* track = ui.fileTree->currentItem()->data(2, 0).value<Track*>();
                    LPWSTR folder;
                    result = path->GetDisplayName(SIGDN_FILESYSPATH, &folder);
                    if (SUCCEEDED(result)) {
                        QList<QModelIndex> items = ui.files->selectionModel()->selectedIndexes();
                        for (size_t i = 0; i < items.size(); i++) {
                            QTreeWidgetItem* row = ui.files->itemFromIndex(items[i]);
                            Directory* dir = row->data(4, 0).value<Directory*>();
                            discs[ui.disclist->currentIndex()]->extractFile(dir, folder);
                        }
                    }
                    path->Release();
                }

            }
        }
        dialog->Release();
    }
}

void MainWindow::openFile() {
    IFileOpenDialog* dialog;
    HRESULT result = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dialog));
    if (SUCCEEDED(result)) {
        DWORD options;
        result = dialog->GetOptions(&options);
        if (SUCCEEDED(result)) {
            result = dialog->SetOptions(options | FOS_ALLOWMULTISELECT);
            if (SUCCEEDED(result)) {
                result = dialog->Show(NULL);
                if (SUCCEEDED(result)) {
                    IShellItemArray* filelist;
                    result = dialog->GetResults(&filelist);
                    if (SUCCEEDED(result)) {
                        DWORD filecount = NULL;
                        result = filelist->GetCount(&filecount);
                        if (SUCCEEDED(result)) {
                            for (DWORD i = 0; i < filecount; i++) {
                                IShellItem* item;
                                LPWSTR name;
                                LPWSTR filename;
                                filelist->GetItemAt(i, &item);
                                if (SUCCEEDED(result)) {
                                    result = item->GetDisplayName(SIGDN_FILESYSPATH, &name);
                                    if (SUCCEEDED(result)) {
                                        result = item->GetDisplayName(SIGDN_NORMALDISPLAY, &filename);
                                        if (SUCCEEDED(result)) {
                                        bool exists = false;
                                        for (size_t i = 0; i < ui.disclist->count(); i++) {
                                            if (discs[i]->filename == name) {
                                                exists = true;
                                            }
                                        }
                                        if (!exists) {
                                            discs.push_back(new Disc(name));
                                            ui.disclist->addItem(QString::fromStdWString(filename), QVariant::fromValue(QString::fromStdWString(filename)));
                                        }
                                    }
                                        
                                        
                                    }
                                }
                            }
                            ui.disclist->setCurrentIndex(ui.disclist->count() - 1);
                            listTree();
                        }
                        
                    }
                    filelist->Release();
                }
                
                
            }
        }
        

        
    }
    dialog->Release();
}

void MainWindow::openFolder(const QTreeWidgetItem* item) {
    Directory* dir = item->data(4, 0).value<Directory*>();
    if (dir->folder) {
        listFileTree(dir->next, false);
    }
}

void MainWindow::upOneFolder() {
    if (current->parent != nullptr) {
        listFileTree(current->parent, false);
    }
}

void MainWindow::exit() {
    QGuiApplication::quit();
}