////////////////////////////////////////////////////////////////////////////////
//
//   Copyright 2011 Mario Negri Institute & Orobix Srl
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//
////////////////////////////////////////////////////////////////////////////////

#include <QStringList>
#include "treeitem.h"

TreeItem::TreeItem(const QVector<QVariant> &data, TreeItem *parent)
{
    parentItem = parent;
    itemData = data;
}

TreeItem::~TreeItem()
{
    qDeleteAll(childItems);
}

TreeItem *TreeItem::child(int number)
{
    return childItems.value(number);
}

int TreeItem::childCount() const
{
    return childItems.count();
}

int TreeItem::childNumber() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}

int TreeItem::columnCount() const
{
    return itemData.count();
}

QVariant TreeItem::data(int column) const
{
    return itemData.value(column);
}

bool TreeItem::insertChildren(int position, int count, int columns)
{
     if (position < 0 || position > childItems.size())
         return false;

     for (int row = 0; row < count; ++row) {
         QVector<QVariant> data(columns);
         TreeItem *item = new TreeItem(data, this);
         childItems.insert(position, item);
     }

     return true;
}

bool TreeItem::insertColumns(int position, int columns)
{
     if (position < 0 || position > itemData.size())
         return false;

     for (int column = 0; column < columns; ++column)
         itemData.insert(position, QVariant());

     foreach (TreeItem *child, childItems)
         child->insertColumns(position, columns);

     return true;
}

TreeItem *TreeItem::parent()
{
     return parentItem;
}

bool TreeItem::removeChildren(int position, int count)
{
     if (position < 0 || position + count > childItems.size())
         return false;

     for (int row = 0; row < count; ++row)
         delete childItems.takeAt(position);

     return true;
}

bool TreeItem::removeColumns(int position, int columns)
{
     if (position < 0 || position + columns > itemData.size())
         return false;

     for (int column = 0; column < columns; ++column)
         itemData.remove(position);

     foreach (TreeItem *child, childItems)
         child->removeColumns(position, columns);

     return true;
}

bool TreeItem::setData(int column, const QVariant &value)
{
     if (column < 0 || column >= itemData.size())
         return false;

     itemData[column] = value;
     return true;
}

