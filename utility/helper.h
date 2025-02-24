﻿#ifndef HELPER_H
#define HELPER_H


class Helper {
  public:
    Helper();
    ~Helper();


    static bool chksumVerify(const QByteArray& msg, uint cks);

    // static CygEnums::CommandGroup calcCmdGroup(const QString& s);

    static uint calcMsgLen(const QByteArray& len_ba);

    static uint calcCks(const QByteArray& ba_msg);

    static QByteArray calcCmdLen(const QByteArray& ba_cmd);

    static QByteArray helloMsg();
    static qint64 maxRunningTime();
    static qint64 maxFileSize();

    static QByteArray msgBuilder(const QString& group, uint id, uint chs = 0);

    static QByteArray cygCmdsBuilder(const QStringList& cmds, uint snr_chs, uint malt_chs);

    static QString genRunTime(uint time);

    static QString hexFormat(const QByteArray& byte_array, const QString& sign = " ");

    static QString strMid(const QString& src, QString start, QString end);

    static double calcFreq(const QByteArray& resp);

    static float calcReal(const QByteArray& resp);

    static int calcInt(const QByteArray& resp);

    // static QStringList listConfigFile(CygEnums::FileCatalog catalog = CygEnums::FileCatalog::Recipe);

    // static void delConfigFile(const QString& file_name, CygEnums::FileCatalog catalog = CygEnums::FileCatalog::Recipe);

    // static QMap<QString, QString> readConfigFile(const QString& file_name, CygEnums::FileCatalog catalog = CygEnums::FileCatalog::Recipe);

    static QByteArray BA_HELLO;
    static qint64 MAX_RUNNING_TIME;
    static qint64 MAX_FILE_SIZE;
    static QString RECIPES_FOLER;
    static QString CONNECTIONS_FOLDER;

  private:
    const double ic6_freq_const_ = 0.000873114913702011;
    static QStringList RECIPE_FIELDS;
    static QStringList CONN_FIELDS;

    QMap<QString, QString> cmd_map = {
        {"QueryGen", "QG"},
        {"UpdateGen", "UG"},

        {"QuerySrc", "QC"},
        {"UpdateSrc", "UC"},

        {"QueryInputN", "QI"},
        {"UpdateInputN", "UI"},

        {"QueryLogi", "QL"},
        {"UpdateLogi", "UL"},

        {"QueryMatl", "QM"},
        {"UpdateMatl", "UM"},

        {"QueryMatlN", "QN"},
        {"UpdateMatlN", "UN"},

        {"QueryOutputN", "QO"},
        {"UpdateOutputN", "UO"},

        {"QueryProcL", "QP"},
        {"UpdateProcL", "UP"},

        {"QueryProcS", "QS"},
        {"UpdateProcS", "US"},

        {"QueryOutputTp", "QT"},
        {"UpdateOutputTp", "UT"},

        {"QueryUserMsg", "QV"},
        {"UpdateUserMsg", "UV"},

        {"StatusGen", "SG"},
        {"StatusMatl", "SL"},

        {"StatusSnsr", "SS"},
        {"RemoteMatl", "RL"},
        {"RemoteGen", "RG"},

    };
};

#endif // HELPER_H
