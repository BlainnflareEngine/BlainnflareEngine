//
// Created by gorev on 25.09.2025.
//

#include "dialog/import_asset_dialog.h"

#include "FileSystemUtils.h"
#include "ui_import_asset_dialog.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

namespace editor
{
import_asset_dialog::import_asset_dialog(const ImportAssetInfo &info, QWidget *parent)
    : QDialog(parent)
    , m_info(info)
    , ui(new Ui::import_asset_dialog)
{
    ui->setupUi(this);

    m_main_layout = new QVBoxLayout();
    m_main_layout->setSpacing(10);
    m_main_layout->setContentsMargins(20, 20, 20, 20);
    m_main_layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    setLayout(m_main_layout);

    m_label = new QLabel(this);
    m_label->setText(m_info.path);
    m_label->adjustSize();
    m_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_btn_layout = new QHBoxLayout(this);
    m_btn_layout->setSpacing(5);
    m_btn_layout->setAlignment(Qt::AlignRight);
    m_btn_layout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    m_btn_confirm = new QPushButton("Confirm", this);
    m_btn_confirm->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(m_btn_confirm, &QPushButton::pressed, this, &import_asset_dialog::OnConfirm);

    m_btn_cancel = new QPushButton("Cancel", this);
    m_btn_cancel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(m_btn_cancel, &QPushButton::pressed, this, &import_asset_dialog::OnCancel);

    m_btn_layout->addWidget(m_btn_confirm);
    m_btn_layout->addWidget(m_btn_cancel);

    m_main_layout->addWidget(m_label);
    m_main_layout->addLayout(m_btn_layout);
}


import_asset_dialog::~import_asset_dialog()
{
    delete m_btn_confirm;
    delete m_btn_cancel;
    delete m_btn_layout;
    delete ui;
}


void import_asset_dialog::OnConfirm()
{
    BF_DEBUG("Confirm!");

    close();
}


void import_asset_dialog::OnCancel()
{
    BF_DEBUG("Cancel!");

    close();
}
} // namespace editor
