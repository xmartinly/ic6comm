#include "write_data.h"

WriteData::WriteData(const QString& data, const QString& file): data_str(data), data_file(file) {
}

WriteData::~WriteData() {
}

void WriteData::run() {
    QFile file = QFile(data_file);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        emit emit_write_data_res(u8"写入错误, 请检查文件是否在使用中");
        return;
    }
    if( file.write(data_str.toStdString().c_str())) {
        emit emit_write_data_res(data_file + ":" + QString::number(data_str.count("\n")) + u8"条 写入正常");
    }
    file.close();
}
