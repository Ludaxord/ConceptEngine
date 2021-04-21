/****************************************************************************
** Meta object code from reading C++ file 'CEQD3DWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../Widgets/CEQD3DWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CEQD3DWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ConceptEngine__Editor__Widgets__CEQD3DWidget_t {
    QByteArrayData data[12];
    char stringdata0[206];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ConceptEngine__Editor__Widgets__CEQD3DWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ConceptEngine__Editor__Widgets__CEQD3DWidget_t qt_meta_stringdata_ConceptEngine__Editor__Widgets__CEQD3DWidget = {
    {
QT_MOC_LITERAL(0, 0, 44), // "ConceptEngine::Editor::Widget..."
QT_MOC_LITERAL(1, 45, 17), // "SignalEventHandle"
QT_MOC_LITERAL(2, 63, 0), // ""
QT_MOC_LITERAL(3, 64, 18), // "SignalWidgetResize"
QT_MOC_LITERAL(4, 83, 12), // "SignalUpdate"
QT_MOC_LITERAL(5, 96, 12), // "SignalRender"
QT_MOC_LITERAL(6, 109, 16), // "SignalKeyPressed"
QT_MOC_LITERAL(7, 126, 10), // "QKeyEvent*"
QT_MOC_LITERAL(8, 137, 16), // "SignalMouseMoved"
QT_MOC_LITERAL(9, 154, 12), // "QMouseEvent*"
QT_MOC_LITERAL(10, 167, 18), // "SignalMouseClicked"
QT_MOC_LITERAL(11, 186, 19) // "SignalMouseReleased"

    },
    "ConceptEngine::Editor::Widgets::CEQD3DWidget\0"
    "SignalEventHandle\0\0SignalWidgetResize\0"
    "SignalUpdate\0SignalRender\0SignalKeyPressed\0"
    "QKeyEvent*\0SignalMouseMoved\0QMouseEvent*\0"
    "SignalMouseClicked\0SignalMouseReleased"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ConceptEngine__Editor__Widgets__CEQD3DWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x06 /* Public */,
       3,    0,   55,    2, 0x06 /* Public */,
       4,    0,   56,    2, 0x06 /* Public */,
       5,    0,   57,    2, 0x06 /* Public */,
       6,    1,   58,    2, 0x06 /* Public */,
       8,    1,   61,    2, 0x06 /* Public */,
      10,    1,   64,    2, 0x06 /* Public */,
      11,    1,   67,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7,    2,
    QMetaType::Void, 0x80000000 | 9,    2,
    QMetaType::Void, 0x80000000 | 9,    2,
    QMetaType::Void, 0x80000000 | 9,    2,

       0        // eod
};

void ConceptEngine::Editor::Widgets::CEQD3DWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CEQD3DWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->SignalEventHandle(); break;
        case 1: _t->SignalWidgetResize(); break;
        case 2: _t->SignalUpdate(); break;
        case 3: _t->SignalRender(); break;
        case 4: _t->SignalKeyPressed((*reinterpret_cast< QKeyEvent*(*)>(_a[1]))); break;
        case 5: _t->SignalMouseMoved((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 6: _t->SignalMouseClicked((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 7: _t->SignalMouseReleased((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CEQD3DWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CEQD3DWidget::SignalEventHandle)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CEQD3DWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CEQD3DWidget::SignalWidgetResize)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (CEQD3DWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CEQD3DWidget::SignalUpdate)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (CEQD3DWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CEQD3DWidget::SignalRender)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (CEQD3DWidget::*)(QKeyEvent * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CEQD3DWidget::SignalKeyPressed)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (CEQD3DWidget::*)(QMouseEvent * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CEQD3DWidget::SignalMouseMoved)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (CEQD3DWidget::*)(QMouseEvent * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CEQD3DWidget::SignalMouseClicked)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (CEQD3DWidget::*)(QMouseEvent * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CEQD3DWidget::SignalMouseReleased)) {
                *result = 7;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ConceptEngine::Editor::Widgets::CEQD3DWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_ConceptEngine__Editor__Widgets__CEQD3DWidget.data,
    qt_meta_data_ConceptEngine__Editor__Widgets__CEQD3DWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ConceptEngine::Editor::Widgets::CEQD3DWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ConceptEngine::Editor::Widgets::CEQD3DWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ConceptEngine__Editor__Widgets__CEQD3DWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ConceptEngine::Editor::Widgets::CEQD3DWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void ConceptEngine::Editor::Widgets::CEQD3DWidget::SignalEventHandle()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ConceptEngine::Editor::Widgets::CEQD3DWidget::SignalWidgetResize()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ConceptEngine::Editor::Widgets::CEQD3DWidget::SignalUpdate()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ConceptEngine::Editor::Widgets::CEQD3DWidget::SignalRender()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void ConceptEngine::Editor::Widgets::CEQD3DWidget::SignalKeyPressed(QKeyEvent * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ConceptEngine::Editor::Widgets::CEQD3DWidget::SignalMouseMoved(QMouseEvent * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void ConceptEngine::Editor::Widgets::CEQD3DWidget::SignalMouseClicked(QMouseEvent * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void ConceptEngine::Editor::Widgets::CEQD3DWidget::SignalMouseReleased(QMouseEvent * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
