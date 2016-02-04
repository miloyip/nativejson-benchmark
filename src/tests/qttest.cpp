#include "../test.h"

#if HAS_QT

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>

static void GenStat(Stat& stat, const QJsonValue& v);

static void GenStat(Stat& stat, const QJsonObject& o) {
    for (QJsonObject::const_iterator itr = o.begin(); itr != o.end(); ++itr) {
        GenStat(stat, itr.value());
        stat.stringLength += itr.key().size();
        stat.stringCount++;
        stat.memberCount++;
    }
    stat.objectCount++;
}

static void GenStat(Stat& stat, const QJsonArray& a) {
    for (QJsonArray::const_iterator itr = a.begin(); itr != a.end(); ++itr) {
        GenStat(stat, *itr);
        stat.elementCount++;
    }
    stat.arrayCount++;
}

static void GenStat(Stat& stat, const QJsonValue& v) {
    switch (v.type()) {
    case QJsonValue::Object:
        GenStat(stat, v.toObject());
        break;

    case QJsonValue::Array:
        GenStat(stat, v.toArray());
        break;

    case QJsonValue::String:
        stat.stringCount++;
        stat.stringLength += v.toString().size();
        break;

    case QJsonValue::Double:
        stat.numberCount++;
        break;

    case QJsonValue::Bool:
        if (v.toBool())
            stat.trueCount++;
        else
            stat.falseCount++;
        break;

    case QJsonValue::Null:
        stat.nullCount++;
        break;

    default:
        break;
    }
}

// static void GenStat(Stat& stat, const dynamic& v) {
//     switch (v.type()) {
//     case dynamic::ARRAY:
//         {
//             for (auto &i : v)
//                 GenStat(stat, i);
//             stat.arrayCount++;
//             stat.elementCount += v.size();
//         }
//         break;

//     case dynamic::OBJECT:
//         {
//             auto p = v.items();
//             for (auto& i : p) {
//                 GenStat(stat, i.second);
//                 stat.stringLength += i.first.size();
//             }
//             stat.objectCount++;
//             stat.memberCount += v.size();
//             stat.stringCount += v.size();
//         }
//         break;

//     case dynamic::STRING: 
//         stat.stringCount++;
//         stat.stringLength += v.size();
//         break;

//     case dynamic::INT64:
//     case dynamic::DOUBLE:
//         stat.numberCount++;
//         break;

//     case dynamic::BOOL:
//         if (v.getBool())
//             stat.trueCount++;
//         else
//             stat.falseCount++;
//         break;

//     case dynamic::NULLT:
//         stat.nullCount++;
//         break;
//     }
// }

class QtParseResult : public ParseResultBase {
public:
    QJsonDocument d;
};

class QtStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.constData(); }

    QByteArray s;
};

class QtTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "Qt (C++)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        QtParseResult* pr = new QtParseResult;
        QJsonParseError error;
        pr->d = QJsonDocument::fromJson(QByteArray(json, length), &error);
        if (error.error != QJsonParseError::NoError) {
            delete pr;
            return 0;
        }
        else
    	   return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const QtParseResult* pr = static_cast<const QtParseResult*>(parseResult);
        QtStringResult* sr = new QtStringResult;
        sr->s = pr->d.toJson(QJsonDocument::Compact);
        return sr;
    }
#endif

#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const QtParseResult* pr = static_cast<const QtParseResult*>(parseResult);
        QtStringResult* sr = new QtStringResult;
        sr->s = pr->d.toJson(QJsonDocument::Indented);
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const QtParseResult* pr = static_cast<const QtParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        if (pr->d.isObject())
            GenStat(*stat, pr->d.object());
        else if (pr->d.isArray())
            GenStat(*stat, pr->d.array());
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        QJsonDocument v = QJsonDocument::fromJson(QByteArray(json));
        *d = v.array()[0].toDouble();
        return true;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        QJsonDocument v = QJsonDocument::fromJson(QByteArray(json));
        s = v.array()[0].toString().toStdString();
        return true;
    }
#endif
};

REGISTER_TEST(QtTest);

#endif
