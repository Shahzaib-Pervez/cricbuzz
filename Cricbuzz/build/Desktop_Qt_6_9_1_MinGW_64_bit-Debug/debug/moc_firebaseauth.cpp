/****************************************************************************
** Meta object code from reading C++ file 'firebaseauth.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../firebaseauth.h"
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'firebaseauth.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN12FirebaseAuthE_t {};
} // unnamed namespace

template <> constexpr inline auto FirebaseAuth::qt_create_metaobjectdata<qt_meta_tag_ZN12FirebaseAuthE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "FirebaseAuth",
        "signInSuccess",
        "",
        "idToken",
        "refreshToken",
        "localId",
        "email",
        "displayName",
        "signUpSuccess",
        "signInError",
        "errorMessage",
        "signUpError",
        "profileUpdateSuccess",
        "operation",
        "userData",
        "profileUpdateError",
        "tokenRefreshSuccess",
        "newIdToken",
        "newRefreshToken",
        "tokenRefreshError",
        "onNetworkReplyFinished",
        "QNetworkReply*",
        "reply"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'signInSuccess'
        QtMocHelpers::SignalData<void(const QString &, const QString &, const QString &, const QString &, const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::QString, 4 }, { QMetaType::QString, 5 }, { QMetaType::QString, 6 },
            { QMetaType::QString, 7 },
        }}),
        // Signal 'signUpSuccess'
        QtMocHelpers::SignalData<void(const QString &, const QString &, const QString &, const QString &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::QString, 4 }, { QMetaType::QString, 5 }, { QMetaType::QString, 6 },
        }}),
        // Signal 'signInError'
        QtMocHelpers::SignalData<void(const QString &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 10 },
        }}),
        // Signal 'signUpError'
        QtMocHelpers::SignalData<void(const QString &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 10 },
        }}),
        // Signal 'profileUpdateSuccess'
        QtMocHelpers::SignalData<void(const QString &, const QJsonObject &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 13 }, { QMetaType::QJsonObject, 14 },
        }}),
        // Signal 'profileUpdateError'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 13 }, { QMetaType::QString, 10 },
        }}),
        // Signal 'tokenRefreshSuccess'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 17 }, { QMetaType::QString, 18 },
        }}),
        // Signal 'tokenRefreshError'
        QtMocHelpers::SignalData<void(const QString &)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 10 },
        }}),
        // Slot 'onNetworkReplyFinished'
        QtMocHelpers::SlotData<void(QNetworkReply *)>(20, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 21, 22 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<FirebaseAuth, qt_meta_tag_ZN12FirebaseAuthE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject FirebaseAuth::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12FirebaseAuthE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12FirebaseAuthE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12FirebaseAuthE_t>.metaTypes,
    nullptr
} };

void FirebaseAuth::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<FirebaseAuth *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->signInSuccess((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[5]))); break;
        case 1: _t->signUpSuccess((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[4]))); break;
        case 2: _t->signInError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->signUpError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->profileUpdateSuccess((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[2]))); break;
        case 5: _t->profileUpdateError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 6: _t->tokenRefreshSuccess((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 7: _t->tokenRefreshError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->onNetworkReplyFinished((*reinterpret_cast< std::add_pointer_t<QNetworkReply*>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QNetworkReply* >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (FirebaseAuth::*)(const QString & , const QString & , const QString & , const QString & , const QString & )>(_a, &FirebaseAuth::signInSuccess, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (FirebaseAuth::*)(const QString & , const QString & , const QString & , const QString & )>(_a, &FirebaseAuth::signUpSuccess, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (FirebaseAuth::*)(const QString & )>(_a, &FirebaseAuth::signInError, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (FirebaseAuth::*)(const QString & )>(_a, &FirebaseAuth::signUpError, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (FirebaseAuth::*)(const QString & , const QJsonObject & )>(_a, &FirebaseAuth::profileUpdateSuccess, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (FirebaseAuth::*)(const QString & , const QString & )>(_a, &FirebaseAuth::profileUpdateError, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (FirebaseAuth::*)(const QString & , const QString & )>(_a, &FirebaseAuth::tokenRefreshSuccess, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (FirebaseAuth::*)(const QString & )>(_a, &FirebaseAuth::tokenRefreshError, 7))
            return;
    }
}

const QMetaObject *FirebaseAuth::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FirebaseAuth::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12FirebaseAuthE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int FirebaseAuth::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void FirebaseAuth::signInSuccess(const QString & _t1, const QString & _t2, const QString & _t3, const QString & _t4, const QString & _t5)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2, _t3, _t4, _t5);
}

// SIGNAL 1
void FirebaseAuth::signUpSuccess(const QString & _t1, const QString & _t2, const QString & _t3, const QString & _t4)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2, _t3, _t4);
}

// SIGNAL 2
void FirebaseAuth::signInError(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void FirebaseAuth::signUpError(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void FirebaseAuth::profileUpdateSuccess(const QString & _t1, const QJsonObject & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1, _t2);
}

// SIGNAL 5
void FirebaseAuth::profileUpdateError(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1, _t2);
}

// SIGNAL 6
void FirebaseAuth::tokenRefreshSuccess(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1, _t2);
}

// SIGNAL 7
void FirebaseAuth::tokenRefreshError(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1);
}
QT_WARNING_POP
