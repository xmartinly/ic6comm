#include "write_data.h"

WriteData::WriteData(const QString& data, const QString& file): data_str(data), data_file(file) {
}

WriteData::~WriteData() {
}

void WriteData::run() {
    QFile file = QFile(data_file);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        emit emit_write_data_res("Write error, please check if the file is in use.");
        return;
    }
    if( file.write(data_str.toStdString().c_str())) {
        emit emit_write_data_res(data_file + ":" + QString::number(data_str.count("\n")) + "records written successfully.");
    }
    file.close();
}
