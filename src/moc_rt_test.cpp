/****************************************************************************
** Meta object code from reading C++ file 'rt_test.h'
**
** Created: Mon Feb 25 00:19:35 2013
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "rt_test.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'rt_test.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_RtTester[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x0a,
      29,   23,    9,    9, 0x0a,
      60,   50,   46,    9, 0x0a,
     146,   87,   46,    9, 0x0a,
     205,  197,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_RtTester[] = {
    "RtTester\0\0runClicked()\0value\0"
    "changeText(bool)\0int\0file,z_op\0"
    "fileSelected(QString,bool)\0"
    "data_set_host,data_set,user_name,user_pass,dataset_id,z_op\0"
    "fileSelected(string,string,string,string,int,bool)\0"
    "locking\0onLockingDisable(bool)\0"
};

const QMetaObject RtTester::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_RtTester,
      qt_meta_data_RtTester, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &RtTester::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *RtTester::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *RtTester::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_RtTester))
        return static_cast<void*>(const_cast< RtTester*>(this));
    return QObject::qt_metacast(_clname);
}

int RtTester::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: runClicked(); break;
        case 1: changeText((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: { int _r = fileSelected((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 3: { int _r = fileSelected((*reinterpret_cast< string(*)>(_a[1])),(*reinterpret_cast< string(*)>(_a[2])),(*reinterpret_cast< string(*)>(_a[3])),(*reinterpret_cast< string(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5])),(*reinterpret_cast< bool(*)>(_a[6])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 4: onLockingDisable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
