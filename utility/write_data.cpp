#include "write_data.h"

WriteData::WriteData(const QString& data, const QString& file, const QString& name): data_str_(data), data_file_(file), inst_name_(name) {
    QStringList file_name_list = file.split("/");
    if(file_name_list.count() > 1) {
        file_name_ = file_name_list.last();
    }
}

WriteData::~WriteData() {
}

///
/// \brief WriteData::run
///
void WriteData::run() {
    QFile file = QFile(data_file_);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) { // write error
        emit emit_write_data_res("Write error, please check if the file is in use.");
        return;
    }
    if( file.write(data_str_.toStdString().c_str())) {
        emit emit_write_data_res(QString("%1: %2 records written successfully. (%3)").arg(inst_name_, QString::number(data_str_.count("\n")), file_name_));
        emit emit_file_size(inst_name_, static_cast<float>(file.size() / (1024 * 1024)));
    }
    file.close();
}
