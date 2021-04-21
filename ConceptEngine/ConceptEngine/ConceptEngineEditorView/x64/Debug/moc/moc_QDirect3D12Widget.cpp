/****************************************************************************
** Meta object code from reading C++ file 'QDirect3D12Widget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../Widgets/QDirect3D12Widget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QDirect3D12Widget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QDirect3D12Widget_t {
    QByteArrayData data[18];
    char stringdata0[207];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QDirect3D12Widget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QDirect3D12Widget_t qt_meta_stringdata_QDirect3D12Widget = {
    {
QT_MOC_LITERAL(0, 0, 17), // "QDirect3D12Widget"
QT_MOC_LITERAL(1, 18, 17), // "deviceInitialized"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 7), // "success"
QT_MOC_LITERAL(4, 45, 12), // "eventHandled"
QT_MOC_LITERAL(5, 58, 13), // "widgetResized"
QT_MOC_LITERAL(6, 72, 6), // "ticked"
QT_MOC_LITERAL(7, 79, 8), // "rendered"
QT_MOC_LITERAL(8, 88, 26), // "ID3D12GraphicsCommandList*"
QT_MOC_LITERAL(9, 115, 2), // "cl"
QT_MOC_LITERAL(10, 118, 10), // "keyPressed"
QT_MOC_LITERAL(11, 129, 10), // "QKeyEvent*"
QT_MOC_LITERAL(12, 140, 10), // "mouseMoved"
QT_MOC_LITERAL(13, 151, 12), // "QMouseEvent*"
QT_MOC_LITERAL(14, 164, 12), // "mouseClicked"
QT_MOC_LITERAL(15, 177, 13), // "mouseReleased"
QT_MOC_LITERAL(16, 191, 7), // "onFrame"
QT_MOC_LITERAL(17, 199, 7) // "onReset"

    },
    "QDirect3D12Widget\0deviceInitialized\0"
    "\0success\0eventHandled\0widgetResized\0"
    "ticked\0rendered\0ID3D12GraphicsCommandList*\0"
    "cl\0keyPressed\0QKeyEvent*\0mouseMoved\0"
    "QMouseEvent*\0mouseClicked\0mouseReleased\0"
    "onFrame\0onReset"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QDirect3D12Widget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x06 /* Public */,
       4,    0,   72,    2, 0x06 /* Public */,
       5,    0,   73,    2, 0x06 /* Public */,
       6,    0,   74,    2, 0x06 /* Public */,
       7,    1,   75,    2, 0x06 /* Public */,
      10,    1,   78,    2, 0x06 /* Public */,
      12,    1,   81,    2, 0x06 /* Public */,
      14,    1,   84,    2, 0x06 /* Public */,
      15,    1,   87,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      16,    0,   90,    2, 0x08 /* Private */,
      17,    0,   91,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 11,    2,
    QMetaType::Void, 0x80000000 | 13,    2,
    QMetaType::Void, 0x80000000 | 13,    2,
    QMetaType::Void, 0x80000000 | 13,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void QDirect3D12Widget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<QDirect3D12Widget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->deviceInitialized((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->eventHandled(); break;
        case 2: _t->widgetResized(); break;
        case 3: _t->ticked(); break;
        case 4: _t->rendered((*reinterpret_cast< ID3D12GraphicsCommandList*(*)>(_a[1]))); break;
        case 5: _t->keyPressed((*reinterpret_cast< QKeyEvent*(*)>(_a[1]))); break;
        case 6: _t->mouseMoved((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 7: _t->mouseClicked((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 8: _t->mouseReleased((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 9: _t->onFrame(); break;
        case 10: _t->onReset(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (QDirect3D12Widget::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QDirect3D12Widget::deviceInitialized)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (QDirect3D12Widget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QDirect3D12Widget::eventHandled)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (QDirect3D12Widget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QDirect3D12Widget::widgetResized)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (QDirect3D12Widget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QDirect3D12Widget::ticked)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (QDirect3D12Widget::*)(ID3D12GraphicsCommandList * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QDirect3D12Widget::rendered)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (QDirect3D12Widget::*)(QKeyEvent * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QDirect3D12Widget::keyPressed)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (QDirect3D12Widget::*)(QMouseEvent * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QDirect3D12Widget::mouseMoved)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (QDirect3D12Widget::*)(QMouseEvent * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QDirect3D12Widget::mouseClicked)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (QDirect3D12Widget::*)(QMouseEvent * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QDirect3D12Widget::mouseReleased)) {
                *result = 8;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject QDirect3D12Widget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_QDirect3D12Widget.data,
    qt_meta_data_QDirect3D12Widget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *QDirect3D12Widget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QDirect3D12Widget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QDirect3D12Widget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int QDirect3D12Widget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void QDirect3D12Widget::deviceInitialized(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QDirect3D12Widget::eventHandled()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void QDirect3D12Widget::widgetResized()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void QDirect3D12Widget::ticked()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void QDirect3D12Widget::rendered(ID3D12GraphicsCommandList * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QDirect3D12Widget::keyPressed(QKeyEvent * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QDirect3D12Widget::mouseMoved(QMouseEvent * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void QDirect3D12Widget::mouseClicked(QMouseEvent * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void QDirect3D12Widget::mouseReleased(QMouseEvent * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
