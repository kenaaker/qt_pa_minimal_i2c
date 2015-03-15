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

#include "qminimal_i2cintegration.h"
#include "qminimal_i2cbackingstore.h"

#include <QtPlatformSupport/private/qgenericunixfontdatabase_p.h>
#include <QtPlatformSupport/private/qgenericunixeventdispatcher_p.h>
#include <QtGui/private/qpixmap_raster_p.h>
#include <QtGui/private/qguiapplication_p.h>
#include <qpa/qplatformwindow.h>
#include <qpa/qplatformfontdatabase.h>

QT_BEGIN_NAMESPACE

static const char debugBackingStoreEnvironmentVariable[] = "QT_DEBUG_BACKINGSTORE";

QMinimal_i2cScreen::QMinimal_i2cScreen()
        : mDepth(1), mFormat(QImage::Format_Mono)
{
    int rc;
    lcd_1 = new SSD1306(0x3D, SSD1306_128_64);
    rc = lcd_1->openDevice("/dev/i2c-1");
    if (rc != 0) {
        abort();
    } else {
        rc = lcd_1->initDevice();
        if (rc != 0) {
            abort();
        } /* endif */
    } /* endif */
    mGeometry = QRect(0,0,
                      lcd_1->getWidth(),
                      lcd_1->getHeight());
    mPhysicalSize = QSize(lcd_1->getWidth(),
                          lcd_1->getHeight());
}

QMinimal_i2cScreen::~QMinimal_i2cScreen() {
    lcd_1->closeDevice();
    delete(lcd_1);
}

QMinimal_i2cIntegration::QMinimal_i2cIntegration(const QStringList &)
          : m_fontDb(new QGenericUnixFontDatabase()) {
    if (qEnvironmentVariableIsSet(debugBackingStoreEnvironmentVariable)
        && qgetenv(debugBackingStoreEnvironmentVariable).toInt() > 0) {
        m_options |= DebugBackingStore;
    }

    mPrimaryScreen = new QMinimal_i2cScreen();
    screenAdded(mPrimaryScreen);
}

QMinimal_i2cIntegration::~QMinimal_i2cIntegration() {
    delete mPrimaryScreen;
}

bool QMinimal_i2cIntegration::hasCapability(QPlatformIntegration::Capability cap) const {
    switch (cap) {
    case ThreadedPixmaps: return true;
    case MultipleWindows: return true;
    default: return QPlatformIntegration::hasCapability(cap);
    }
}

QPlatformFontDatabase *QMinimal_i2cIntegration::fontDatabase() const {
    return m_fontDb;
}

QPlatformWindow *QMinimal_i2cIntegration::createPlatformWindow(QWindow *window) const {
    Q_UNUSED(window);
    QPlatformWindow *w = new QPlatformWindow(window);
    w->requestActivateWindow();
    return w;
}

QPlatformBackingStore *QMinimal_i2cIntegration::createPlatformBackingStore(QWindow *window) const {
    QMinimal_i2cBackingStore *plat_BS;

    plat_BS = new QMinimal_i2cBackingStore(window);
    return plat_BS;

}

QAbstractEventDispatcher *QMinimal_i2cIntegration::createEventDispatcher() const {
#ifdef Q_OS_WIN
#ifndef Q_OS_WINRT
    return new QEventDispatcherWin32;
#else // !Q_OS_WINRT
    return new QEventDispatcherWinRT;
#endif // Q_OS_WINRT
#else
    return createUnixEventDispatcher();
#endif
}

QMinimal_i2cIntegration *QMinimal_i2cIntegration::instance() {
    return static_cast<QMinimal_i2cIntegration *>(QGuiApplicationPrivate::platformIntegration());
}

QRect QMinimal_i2cIntegration::geometry(void) const {
    return mPrimaryScreen->mGeometry;
}

QT_END_NAMESPACE
