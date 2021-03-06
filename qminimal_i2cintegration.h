/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QPLATFORMINTEGRATION_MINIMAL_H
#define QPLATFORMINTEGRATION_MINIMAL_H

#include <qpa/qplatformintegration.h>
#include <qpa/qplatformscreen.h>
#include "ssd1306.h"

QT_BEGIN_NAMESPACE

class QMinimal_i2cScreen : public QPlatformScreen
{
public:
    QMinimal_i2cScreen();
    ~QMinimal_i2cScreen();
    QRect geometry() const {
        return mGeometry;
    }
    int depth() const {
        return mDepth;
    }
    QImage::Format format() const {
        return mFormat;
    }

public:
    QRect mGeometry;
    int mDepth;
    QImage::Format mFormat;
    QSize mPhysicalSize;
    SSD1306 *lcd_1;
};

class QMinimal_i2cIntegration : public QPlatformIntegration
{
public:
    enum Options { // Options to be passed on command line or determined from environment
        DebugBackingStore = 0x1
    };

    explicit QMinimal_i2cIntegration(const QStringList &parameters);
    ~QMinimal_i2cIntegration();

    bool hasCapability(QPlatformIntegration::Capability cap) const;
    QPlatformFontDatabase *fontDatabase() const;

    QPlatformWindow *createPlatformWindow(QWindow *window) const;
    QPlatformBackingStore *createPlatformBackingStore(QWindow *window) const;
    QAbstractEventDispatcher *createEventDispatcher() const;
    QMinimal_i2cScreen *mPrimaryScreen;
    QRect geometry(void) const;

    int depth() const {
        return 1;
    };
    QImage::Format format() const {
        return QImage::Format_Mono;
    };

    unsigned options() const {
        return m_options;
    }

    static QMinimal_i2cIntegration *instance();

private:
    unsigned m_options;
    QPlatformFontDatabase *m_fontDb;
};

QT_END_NAMESPACE

#endif
