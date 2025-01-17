// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QTest>

#include <QtCore/qloggingcategory.h>
#include <private/qguiapplication_p.h>
#include <qpa/qplatformintegration.h>

#include "../../../../shared/nativewindow.h"

class tst_ForeignWindow: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        auto *platformIntegration = QGuiApplicationPrivate::platformIntegration();
        if (!platformIntegration->hasCapability(QPlatformIntegration::ForeignWindows))
            QSKIP("This platform does not support foreign windows");
    }

    void fromWinId();
    void initialState();
};

void tst_ForeignWindow::fromWinId()
{
    NativeWindow nativeWindow;
    QVERIFY(nativeWindow);

    std::unique_ptr<QWindow> foreignWindow(QWindow::fromWinId(nativeWindow));
    QVERIFY(foreignWindow);
    QVERIFY(foreignWindow->flags().testFlag(Qt::ForeignWindow));
    QVERIFY(foreignWindow->handle());

    // fromWinId does not take (exclusive) ownership of the native window,
    // so deleting the foreign window should not be a problem/cause crashes.
    foreignWindow.reset();
}

void tst_ForeignWindow::initialState()
{
    NativeWindow nativeWindow;
    QVERIFY(nativeWindow);

    // A foreign window can be used to embed a Qt UI in a foreign window hierarchy,
    // in which case the foreign window merely acts as a parent and should not be
    // modified, or to embed a foreign window in a Qt UI, in which case the foreign
    // window must to be able to re-parent, move, resize, show, etc, so that the
    // containing Qt UI can treat it as any other window.

    // At the point of creation though, we don't know what the foreign window
    // will be used for, so the platform should not assume it can modify the
    // window. Any properties set on the native window should persist past
    // creation of the foreign window.

    const QRect initialGeometry(123, 456, 321, 654);
    nativeWindow.setGeometry(initialGeometry);

    std::unique_ptr<QWindow> foreignWindow(QWindow::fromWinId(nativeWindow));
    QCOMPARE(nativeWindow.geometry(), initialGeometry);

    // For extra bonus points, the foreign window should actually
    // reflect the state of the native window.
    QCOMPARE(foreignWindow->geometry(), initialGeometry);
}

#include <tst_foreignwindow.moc>
QTEST_MAIN(tst_ForeignWindow)
