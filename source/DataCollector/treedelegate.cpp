////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2011 Mario Negri Institute & Orobix Srl
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////////

#include <QtGui>
#include "treedelegate.h"

void TreeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
}

QSize TreeDelegate::sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const
{
    return QStyledItemDelegate::sizeHint(option, index);
}

QWidget *TreeDelegate::createEditor(QWidget *parent,const QStyleOptionViewItem &option,const QModelIndex &index) const
{
    QStringList list;
    if (index.column()==1)
        list.append(index.model()->data(index.model()->index(index.row(),10,index.parent())).toStringList());
    else if (index.column()==8)
        list.append(index.model()->data(index.model()->index(index.row(),5,index.parent())).toStringList());

    if (!list.isEmpty()) {
        if (index.column()==1)
            list.prepend("..."); 
        QComboBox *editor = new QComboBox(parent);
        editor->insertItems(0,list);
        connect(editor, SIGNAL(activated(int)), this, SLOT(emitCommitData()));
        return editor;
    } else {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

void TreeDelegate::emitCommitData()
{
    emit commitData(qobject_cast<QWidget *>(sender()));
    emit closeEditor(qobject_cast<QWidget *>(sender()));
}

void TreeDelegate::setEditorData(QWidget *editor,const QModelIndex &index) const
{
    QStringList list;
    if (index.column()==1)
        list.append(index.model()->data(index.model()->index(index.row(),10,index.parent())).toStringList());
    else if (index.column()==8)
        list.append(index.model()->data(index.model()->index(index.row(),5,index.parent())).toStringList());

    if (!list.isEmpty()) {
        QComboBox *comboBox = static_cast<QComboBox *>(editor);
        int currIndex = comboBox->findText(index.model()->data(index).toString())>=0 ? comboBox->findText(index.model()->data(index).toString()): 0;
        comboBox->setCurrentIndex(currIndex);
        comboBox->showPopup();
    } else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void TreeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const
{
    QStringList list;
    if (index.column()==1)
        list.append(index.model()->data(index.model()->index(index.row(),10,index.parent())).toStringList());
    else if (index.column()==8)
        list.append(index.model()->data(index.model()->index(index.row(),5,index.parent())).toStringList());

    if (!list.isEmpty()) {
        QComboBox *comboBox = static_cast<QComboBox*>(editor);
        QString value = comboBox->currentText()=="..." ? "": comboBox->currentText();
        model->setData(index, value, Qt::EditRole);
    } else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

