#include "utility\helper.h"

QByteArray Helper::BA_HELLO = QByteArray::fromHex("0200480149");
//SS0300 SS0400 SS0500
//sensor status, sensor freq, sensor act
QByteArray Helper::BA_SNRDATA = QByteArray::fromHex("0C00535303005353040053530500FE");
qint64 Helper::MAX_RUNNING_TIME = 2592000;
qint64 Helper::MAX_FILE_SIZE = 1024 * 1024 * 10;
QString Helper::RECIPES_FOLER = "/recipes/";
QString Helper::CONNECTIONS_FOLDER = "/connections/";
QStringList Helper::RECIPE_FIELDS = {"Commands", "Duration", "Interval", "Sensors", "Materials", "Save"};
QStringList Helper::CONN_FIELDS = {"Commands", "Duration", "Interval", "Sensors", "Materials", "Save"};

Helper::Helper() {
    // 53 00 00 FA 06 80 81 81 80 81 81 06 C0 C6 76 77 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 DC 4B 77 77 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 06 14 03 00 00 00 00 00 00 00 00 00 00 12 03 00 00 00 00 00 00 00 00 00 00 C6
}

Helper::~Helper() {
}

///
/// \brief Helper::chksumVerify. verify command checksum ok.
/// \param QByteArray msg
/// \param uint cks
/// \return bool
///
bool Helper::chksumVerify(const QByteArray& msg, uint cks) {
    // qDebug() << calcCks(msg) << msg.toHex() << cks;
    return calcCks(msg) == cks;
}

///
/// \brief Helper::calcCmdGroup. convert command to enums
/// \param QString s
/// \return CygEnums::CommandGroup
///
// CygEnums::CommandGroup Helper::calcCmdGroup(const QString& s) {
//     //SS: 83083
//     //SG: 83071
//     //SL: 83076
//     //RL: 82076
//     //RG: 82071
//     //UG: 85071
//     //QG: 81071
//     if(s.length() != 2) {
//         return CygEnums::StatusMsg;
//     }
//     QByteArray ba_cmd = s.toUtf8();
//     uint cmd_idx = (ba_cmd.at(0) * 1000 + ba_cmd.at(1));
//     qDebug() << s << cmd_idx;
//     return static_cast<CygEnums::CommandGroup>(cmd_idx);
// }

///
/// \brief Helper::calcMsgLen. calculate message length.
/// \param QByteArray len_ba
/// \return uint
///
uint Helper::calcMsgLen(const QByteArray& len_ba) {
    if(len_ba.length() != 2) {
        return 0;
    }
    QByteArray new_len;
    new_len.resize(2);
    new_len[0] = len_ba.at(1);
    new_len[1] = len_ba.at(0);
    bool cvt_ok;
    uint res = new_len.toHex().toUInt(&cvt_ok, 16);
    return cvt_ok ? res : 0;
}

///
/// \brief Helper::calcCks. calculate checksum value.
/// \param QByteArray ba_msg
/// \return uint
///
uint Helper::calcCks(const QByteArray& ba_msg) {
    uint cks_ = 0;
    foreach (auto c, ba_msg) {
        cks_ += static_cast<uint>(c);
    }
    cks_ &= 0xff;
    return cks_;
}

void Helper::calcData(const QByteArray& data, QList<int>* l_act, QList<float>* l_freq, QList<bool>* l_stat) {
    int stat_len = 8;
    int freq_len = 64;
    int act_len = 32;
    if(data.length() != 107) {
        return;
    }
    QByteArray ba_status = data.mid(1, stat_len);
    QByteArray ba_freq = data.mid(10, freq_len);
    QByteArray ba_act = data.mid(75, act_len);
    auto stat = calcStatus(ba_status);
    for (int var = 0; var < 8; ++var) {
        QByteArray act = ba_act.mid(var * 4, 4);
        QByteArray freq = ba_freq.mid(var * 8, 8);
        l_act->append(calcInt(act));
        l_freq->append(calcFreq(freq));
        l_stat->append(stat.at(var));
    }
    // This is available in all editors.
    // qDebug() << __FUNCTION__ << acts << freqs << stats;
}

///
/// \brief Helper::calcCmdLen. calculate response length
/// \param QByteArray ba_cmd
/// \return QByteArray
///
QByteArray Helper::calcCmdLen(const QByteArray& ba_cmd) {
    QByteArray ba_len;
    uint len = ba_cmd.length();
    uint low_byte = len % 255;
    uint high_byte = len / 255;
    ba_len.append(low_byte);
    ba_len.append(high_byte);
    return ba_len;
}

///
/// \brief Helper::helloMsg. H1 message
/// \return QByteArray
///
QByteArray Helper::helloMsg() {
    return BA_HELLO;
}

///
/// \brief Helper::helloMsg. H1 message
/// \return QByteArray
///
QByteArray Helper::SnrData() {
    return BA_SNRDATA;
}

///
/// \brief Helper::maxRunningTime
/// \return
///
qint64 Helper::maxRunningTime() {
    return MAX_RUNNING_TIME;
}

///
/// \brief Helper::maxFileSize
/// \return
///
qint64 Helper::maxFileSize() {
    return MAX_FILE_SIZE;
}

///
/// \brief Helper::msgBuilder. build up command to qbytearray format
/// \param QString group
/// \param uint id
/// \param uint chs
/// \return QByteArray
///
QByteArray Helper::msgBuilder(const QString& group, uint id, uint chs) {
    QByteArray ba_cmd = group.toUtf8();
    // ba_cmd.append(id);
    // CygEnums::CommandGroup cmd_group = Helper::calcCmdGroup(group);
    // if(cmd_group == CygEnums::RemoteMatl
    //         || cmd_group == CygEnums::StatusMatl
    //         || cmd_group == CygEnums::UpdateGen
    //         || cmd_group == CygEnums::StatusSnsr) {
    //     ba_cmd.append(chs);
    // }
    // ba_cmd.append(chs);
    return ba_cmd;
}

// QStringList Helper::listConfigFile(CygEnums::FileCatalog catalog) {
//     QStringList sl_files;
//     QString file_dir = QCoreApplication::applicationDirPath() + (catalog == CygEnums::Recipe ? RECIPES_FOLER : CONNECTIONS_FOLDER);
//     QDir dir(file_dir);
//     if (!dir.exists()) {
//         return sl_files;
//     }
//     dir.setFilter(QDir::Dirs | QDir::Files);
//     dir.setSorting(QDir::DirsFirst);
//     QFileInfoList files = dir.entryInfoList();
//     foreach (QFileInfo fi, files) {
//         if (fi.fileName() == "." || fi.fileName() == "..") {
//             continue;
//         }
//         if (fi.isDir()) {
//             continue;
//         } else {
//             QString s_filePath = fi.filePath();
//             int i_slashIdx = s_filePath.lastIndexOf('/');
//             int i_dotIdx = s_filePath.lastIndexOf('.');
//             sl_files << s_filePath.mid(i_slashIdx + 1, i_dotIdx - i_slashIdx - 1);
//         }
//     }
//     sl_files.sort();
//     return sl_files;
// }

// void Helper::delConfigFile(const QString& file_name, CygEnums::FileCatalog catalog) {
//     QString file_path = QCoreApplication::applicationDirPath() + (catalog == CygEnums::Recipe ? RECIPES_FOLER : CONNECTIONS_FOLDER) + file_name + ".ini";
//     QFile file(file_path);
//     if (file.exists()) {
//         file.remove();
//     }
// }

// QMap<QString, QString> Helper::readConfigFile(const QString& file_name, CygEnums::FileCatalog catalog) {
//     QString file_path = QCoreApplication::applicationDirPath() + (catalog == CygEnums::Recipe ? RECIPES_FOLER : CONNECTIONS_FOLDER) + file_name + ".ini";
//     QSettings ini_file(file_path, QSettings::IniFormat);
//     QMap<QString, QString> settings;
//     QFile file(file_path);
//     if (!file.exists()) {
//         return settings;
//     }
//     foreach (auto field, RECIPE_FIELDS) {
//         settings.insert(field, ini_file.value(field).toString());
//     }
//     return settings;
// }


///
/// \brief Helper::cygCmdsBuilder
/// \param QStringList cmds. command catalog
/// \param uint snr_chs. SS group sensor index
/// \param uint malt_chs. SL group material index
/// \param uint sg_opt. SG group options
/// \return QByteArray
///
QByteArray Helper::cygCmdsBuilder(const QStringList& cmds, uint snr_chs, uint malt_chs) {
    QByteArray ba_cmd = {};
    // QList<CygEnums::CommandGroup> types;
    foreach (auto t, cmds) {
        QStringList sl_cmd = t.split("_");
        QString cmd_group = sl_cmd.at(0);
        uint cmd_id = sl_cmd.at(1).toUInt();
        // CygEnums::CommandGroup type = calcCmdGroup(sl_cmd.at(0));
        // types.push_back(type);
        ba_cmd.push_back(msgBuilder(cmd_group, cmd_id, malt_chs));
        // if(type == CygEnums::StatusMatl) {
        //     ba_cmd.push_back(msgBuilder(cmd_group, cmd_id, malt_chs));
        // }
        // if(type == CygEnums::StatusSnsr) {
        //     ba_cmd.push_back(msgBuilder(cmd_group, cmd_id, snr_chs));
        // }
        // if(type == CygEnums::StatusGen) {
        //     ba_cmd.push_back(msgBuilder(cmd_group, cmd_id, snr_chs));
        // }
    }
    QByteArray cmd_len = calcCmdLen(ba_cmd);
    uint cmd_cks = calcCks(ba_cmd);
    ba_cmd.push_front(cmd_len);
    ba_cmd.push_back(cmd_cks);
    return ba_cmd;
}

///
/// \brief Helper::genRunTime. convert seconds to 01:02:03 format
/// \param uint time
/// \return QString
///
QString Helper::genRunTime(uint time) {
    int days = time / (24 * 3600);
    QString days_s = days > 0 ? QString("%1-").arg(days) : "";
    time %= (24 * 3600);
    int hours = time / 3600;
    QString hours_s = QString::number(hours).rightJustified(2, '0');
    time %= 3600;
    int minutes = time / 60;
    QString minutes_s = QString::number(minutes).rightJustified(2, '0');
    time %= 60;
    QString secs_s = QString::number(time).rightJustified(2, '0');
    return QString("%1%2:%3:%4").arg(days_s, hours_s, minutes_s, secs_s);
}

///
/// \brief Helper::hexFormat. format bytearray to "00 01 02 ..."
/// \param QByteArray byte_array
/// \return QString
///
QString Helper::hexFormat(const QByteArray& byte_array, const QString& sign) {
    QStringList hex_list;
    foreach (auto c, byte_array) {
        uint val = static_cast<uint>(c) & 0xff;
        QString hex = QString::number(val, 16).rightJustified(2, '0').toUpper();
        hex_list.append(hex);
    }
    return hex_list.join(sign);
}

QString Helper::strMid(const QString& src, QString start, QString end) {
    int start_idx = src.lastIndexOf(start);
    int end_idx = src.lastIndexOf(end);
    return src.mid(start_idx + 1, end_idx - start_idx - 1);
}


///
/// \brief Helper::calcFreq
/// \param resp
/// \return
///
double Helper::calcFreq(const QByteArray& resp) {
    if(resp.length() != 8) {
        return 0;
    }
    double factor = 0.000873114913702011;
    QDataStream stream(resp);
    stream.setByteOrder(QDataStream::LittleEndian);
    qint64 value;
    // 从流中读取 long 值
    stream >> value;
    return value * factor;
}

///
/// \brief Helper::calcReal
/// \param resp
/// \return
///
float Helper::calcReal(const QByteArray& resp) {
    if(resp.length() != 4) {
        return 0;
    }
    QDataStream stream(resp);
    stream.setByteOrder(QDataStream::LittleEndian);
    float value;
    stream >> value;
    return value;
}

int Helper::calcInt(const QByteArray& resp) {
    if(resp.length() != 4) {
        return 0;
    }
    QDataStream stream(resp);
    stream.setByteOrder(QDataStream::LittleEndian);
    int value;
    stream >> value;
    return value;
}

QList<bool> Helper::calcStatus(const QByteArray& stat) {
    QList<bool> status = {};
    foreach (auto c, stat) {
        status.append( (c & 1) == 0);
    }
    return status;
}


