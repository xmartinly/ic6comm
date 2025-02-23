#ifndef CYG_ENUMS_H
#define CYG_ENUMS_H
namespace Utility {

    class CygEnums : public QObject {
        Q_OBJECT
      public:
        explicit CygEnums(QObject* parent = nullptr) {
            Q_UNUSED(parent);
        };
        ~CygEnums() {};

        enum CommandGroup {
            //SS: 83083
            //SG: 83071
            //SL: 83076
            //RL: 82076
            //RG: 82071
            //UG: 85071
            //QG: 81071
            StatusMsg       = 0,
            HelloMsg        = 72000,
            QueryGen        = 81071,
            QueryInputN     = 81073,
            QueryLogi       = 81076,
            QueryMatl       = 81077,
            QueryMatlN      = 81078,
            QueryOutputN    = 81079,
            QueryOutputTp   = 81084,
            QueryProcL      = 81080,
            QueryProcS      = 81083,
            QuerySrc        = 81067,
            QueryUserMsg    = 81086,
            RemoteGen       = 82071,
            RemoteMatl      = 82076,
            StatusGen       = 83071,
            StatusMatl      = 83076,
            StatusSnsr      = 83083,
            UpdateGen       = 85071,
            UpdateInputN    = 85073,
            UpdateLogi      = 85076,
            UpdateMatl      = 85077,
            UpdateMatlN     = 85078,
            UpdateOutputN   = 85079,
            UpdateOutputTp  = 85084,
            UpdateProcL     = 85080,
            UpdateProcS     = 85083,
            UpdateSrc       = 85067,
            UpdateUserMsg   = 85086,
        };
        Q_ENUM(CommandGroup)
        Q_DECLARE_FLAGS(CommandGroups, CommandGroup)
        Q_FLAG(CommandGroups)

        enum CommandType {
            LoopCmd   = 0,
            TimelyCmd = 1,
            HelloCmd  = 2,
        };
        Q_ENUM(CommandType)
        Q_DECLARE_FLAGS(CommandTypes, CommandType)
        Q_FLAG(CommandTypes)

        enum RespErr {
            NoErr               = 0,
            IllegalCommand      = 'A',
            IllegalParameterVal = 'B',
            InvalidChecksum     = 'C',
            IllegalID           = 'D',
            DataNAvailable      = 'E',
            CNDNIF              = 'F',
            LenErr              = 'L',
            PriorCmdFailed      = 'P',
            InvalidMsg          = 'I',
            TooManyCmds         = 'M',
            RespLarger          = 'O'
        };
        Q_ENUM(RespErr)
        Q_DECLARE_FLAGS(RespErrs, RespErr)
        Q_FLAG(RespErrs)

        enum FileCatalog {
            Recipe               = 0,
            Connection           = 1,
        };
        Q_ENUM(FileCatalog)
        Q_DECLARE_FLAGS(FileCatalogs, FileCatalog)
        Q_FLAG(FileCatalogs)

        enum ConnType {
            TcpIpConn   = 0,
            SerConn     = 1,
        };
        Q_ENUM(ConnType)
        Q_DECLARE_FLAGS(ConnTypes, ConnType)
        Q_FLAG(ConnTypes)

    };

    Q_DECLARE_OPERATORS_FOR_FLAGS(CygEnums::CommandTypes)
    Q_DECLARE_OPERATORS_FOR_FLAGS(CygEnums::CommandGroups)
    Q_DECLARE_OPERATORS_FOR_FLAGS(CygEnums::RespErrs)
    Q_DECLARE_OPERATORS_FOR_FLAGS(CygEnums::FileCatalogs)
    Q_DECLARE_OPERATORS_FOR_FLAGS(CygEnums::ConnTypes)

}
#endif // CYG_ENUMS_H
