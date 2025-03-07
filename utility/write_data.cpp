#include "write_data.h"

WriteData::WriteData(const QString& data, const QString& file, const QString& name): data_str(data), data_file(file), inst_name(name) {
}

WriteData::~WriteData() {
}

///
/// \brief WriteData::run
///
void WriteData::run() {
    QFile file = QFile(data_file);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) { // write error
        emit emit_write_data_res("Write error, please check if the file is in use.");
        return;
    }
    if( file.write(data_str.toStdString().c_str())) {
        emit emit_write_data_res(data_file + ":" + QString::number(data_str.count("\n")) + "records written successfully.");
        emit emit_file_size(inst_name, static_cast<float>(file.size() / (1024 * 1024)));
    }
    file.close();
}
