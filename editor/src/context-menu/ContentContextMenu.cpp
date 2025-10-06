//
// Created by gorev on 23.09.2025.
//

#include "context-menu/ContentContextMenu.h"

#include "../../include/dialog/create_material_dialog.h"
#include "FileSystemUtils.h"

#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <fstream>
#include <qabstractitemview.h>

namespace editor
{
ContentContextMenu::ContentContextMenu(QAbstractItemView &parent)
    : QObject(&parent)
    , m_parent(parent)
{
}


void ContentContextMenu::CreateFolder(QString dirPath) const
{
    QInputDialog inputDialog(&m_parent);
    inputDialog.setWindowTitle("Create folder");
    inputDialog.setLabelText("Folder name:");
    inputDialog.setTextValue("");
    inputDialog.setOkButtonText("Create");
    inputDialog.setCancelButtonText("Cancel");
    inputDialog.setMinimumSize(200, 100);


    if (inputDialog.exec() == QDialog::Accepted)
    {
        QString folderName = inputDialog.textValue().trimmed();
        if (!folderName.isEmpty())
        {
            QDir dir(dirPath);
            if (!dir.mkdir(folderName))
            {
                QMessageBox::warning(&m_parent, "Error", "Failed to create folder");
            }
        }
    }
}


void ContentContextMenu::CreateScript(const QString &dirPath) const
{
    qDebug() << "TODO: should notify engine that new script was added!";

    QInputDialog inputDialog(&m_parent);
    inputDialog.setWindowTitle("Create script");
    inputDialog.setLabelText("Script name:");
    inputDialog.setTextValue("");
    inputDialog.setOkButtonText("Create");
    inputDialog.setCancelButtonText("Cancel");
    inputDialog.setMinimumSize(200, 100);

    if (inputDialog.exec() == QDialog::Accepted)
    {
        QString filePath = dirPath + QDir::separator() + inputDialog.textValue() + ".lua";
        QFile script(filePath);

        if (!script.open(QIODevice::ReadWrite))
        {
            QMessageBox::warning(&m_parent, "Error", "Failed to create script");
            return;
        }

        script.close();
    }
}


void ContentContextMenu::CreateMaterial(const QString &dirPath) const
{
    create_material_dialog materialDialog(&m_parent);

    if (materialDialog.exec() == QDialog::Accepted)
    {
        QString filePath = dirPath + QDir::separator() + materialDialog.GetMaterialName() + ".mat";

        YAML::Node config;
        UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
        Blainn::uuid uuid = uuidGenerator.getUUID();
        std::string temp;

        config["ID"] = uuid.str();

        temp = materialDialog.GetAlbedoPath().toUtf8().constData();
        config["AlbedoPath"] = temp;

        temp = materialDialog.GetNormalPath().toUtf8().constData();
        config["NormalPath"] = temp;

        temp = materialDialog.GetMetallicPath().toUtf8().constData();
        config["MetallicPath"] = temp;

        temp = materialDialog.GetRoughnessPath().toUtf8().constData();
        config["RoughnessPath"] = temp;

        temp = materialDialog.GetAOPath().toUtf8().constData();
        config["AOPath"] = temp;

        std::string pathStr = filePath.toUtf8().constData();
        const Blainn::Path configFilePath = Blainn::Path(pathStr);
        std::ofstream fout(configFilePath.string());
        fout << config;
    }
}


void ContentContextMenu::OnContextMenu(const QPoint &pos) const
{
    QModelIndex index = m_parent.indexAt(pos);

    if (index.isValid()) return;

    QFileSystemModel *model = qobject_cast<QFileSystemModel *>(m_parent.model());
    if (!model) return;

    QString dirPath = model->rootPath();
    QMenu menu;

    QAction *createFolderAction = menu.addAction("Create folder");
    QAction *createScriptAction = menu.addAction("Create script");
    QAction *createMaterialAction = menu.addAction("Create material");
    QAction *showExplorerAction = menu.addAction("Show in explorer");
    QAction *selectedAction = menu.exec(m_parent.viewport()->mapToGlobal(pos));

    if (selectedAction == createFolderAction) CreateFolder(dirPath);
    else if (selectedAction == createScriptAction) CreateScript(dirPath);
    else if (selectedAction == createMaterialAction) CreateMaterial(dirPath);
    else if (selectedAction == showExplorerAction) OpenFolderExplorer(dirPath);
}
} // namespace editor