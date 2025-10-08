//
// Created by gorev on 23.09.2025.
//

#include "context-menu/ContentContextMenu.h"

#include "../../include/dialog/create_material_dialog.h"
#include "Editor.h"
#include "FileSystemUtils.h"
#include "IdGenerator.h"

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
        QDir dir(Blainn::Editor::GetInstance().GetContentDirectory());

        // These are relative paths, you can restore full path with content folder path
        config["ID"] = Blainn::GenerateID().str();
        config["Path"] = ToString(dir.relativeFilePath(filePath));
        config["ShaderPath"] = ToString(dir.relativeFilePath(materialDialog.GetShaderPath()));
        config["AlbedoPath"] = ToString(dir.relativeFilePath(materialDialog.GetAlbedoPath()));
        config["NormalPath"] = ToString(dir.relativeFilePath(materialDialog.GetNormalPath()));
        config["MetallicPath"] = ToString(dir.relativeFilePath(materialDialog.GetMetallicPath()));
        config["RoughnessPath"] = ToString(dir.relativeFilePath(materialDialog.GetRoughnessPath()));
        config["AOPath"] = ToString(dir.relativeFilePath(materialDialog.GetAOPath()));

        std::string pathStr = ToString(filePath);
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