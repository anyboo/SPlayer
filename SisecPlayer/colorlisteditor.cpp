/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "stdafx.h"
#include <qcolor>
#include <qvariant>
#include <QtGui>

#include "colorlisteditor.h"

#define VCA5_APP_PALETTE_SIZE2 				10
//COLORREF  gPieColours[VCA5_APP_PALETTE_SIZE2] = { RGB2(0, 0, 0), RGB2(150, 75, 0), RGB2(100, 100, 100), RGB2(0, 0, 200), RGB2(0, 150, 0), RGB2(0, 255, 255), RGB2(255, 0, 0), RGB2(200, 0, 200), RGB2(255, 255, 0), RGB2(255, 255, 255) };
char *gPieColourNames[VCA5_APP_PALETTE_SIZE2] = { "Black", "Brown", "Grey", "Blue", "Green", "Cyan", "Red", "Pink", "Yellow", "White" };


ColorListEditor::ColorListEditor(QWidget *widget) : QComboBox(widget)
{
    populateList();
}

//! [0]
QColor ColorListEditor::color() const
{
   // return QVariantValue<QColor>(itemData(currentIndex(), Qt::DecorationRole));
	QVariant t(itemData(currentIndex(), Qt::DecorationRole));
	//return QColor(itemData(currentIndex(), Qt::DecorationRole));
	return t.value<QColor>();
}
//! [0]

//! [1]
void ColorListEditor::setColor(QColor color)
{
    setCurrentIndex(findData(color, int(Qt::DecorationRole)));
}
//! [1]

//! [2]
void ColorListEditor::populateList()
{
  //  QStringList colorNames = QColor::colorNames();

	for (int i = 0; i < VCA5_APP_PALETTE_SIZE2; ++i) {
		QColor color(gPieColourNames[i]);

		insertItem(i, gPieColourNames[i]);
        setItemData(i, color, Qt::DecorationRole);
    }
}
//! [2]
