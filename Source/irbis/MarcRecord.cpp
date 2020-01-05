﻿// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "irbis.h"

#include <iomanip>

#if defined(_MSC_VER)
#pragma warning(disable: 4068)
#endif

#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

namespace irbis {

RecordField& MarcRecord::add(int tag, const String &value)
{
    this->fields.push_back(RecordField { tag, value });
    return this->fields.back();
}

MarcRecord MarcRecord::clone() const
{
    MarcRecord result { this->mfn, this->status, this->version, this->database};
    for (const auto &one : this->fields) {
        result.fields.push_back(one.clone());
    }

    return result;
}

void MarcRecord::decode(const StringList &lines)
{
    if (lines.size() < 2) {
        return;
    }

    // mfn and status of the record
    const auto firstLine = split(lines[0], L'#');
    this->mfn = fastParseUnsigned32(firstLine[0]);
    this->status = fastParseUnsigned32(safeAt(firstLine, 1));

    // version of the record
    const auto secondLine = split(lines[1], L'#');
    this->version = fastParseUnsigned32(safeAt(secondLine, 1));

    // fields
    for (size_t i = 2; i < lines.size(); i++) {
        const auto line = lines[i];
        if (!line.empty()) {
            RecordField field;
            field.decode(line);
            this->fields.push_back(field);
        }
    }
}

bool MarcRecord::deleted() const noexcept
{
    return (this->status & RecordStatus::Deleted) != 0u;
}

String MarcRecord::encode(const String &delimiter = IrbisText::IrbisDelimiter) const
{
    String result = std::to_wstring(this->mfn) + L"#"
            + std::to_wstring(this->status) + delimiter
            + L"0#" + std::to_wstring(this->version) + delimiter;
    for (const auto &field : this->fields) {
        result.append(field.toString());
        result.append(delimiter);
    }

    return result;
}

String MarcRecord::fm(int tag, Char code = 0) const noexcept
{
    for (const auto &field : this->fields) {
        if (field.tag == tag) {
            if (code) {
                for (const auto &subfield : field.subfields) {
                    if (sameChar(subfield.code, code)) {
                        return subfield.value;
                    }
                }
            } else {
                return field.value;
            }
        }
    }

    return String();
}

StringList MarcRecord::fma(int tag, Char code) const
{
    StringList result;
    for (const auto &field : this->fields) {
        if (field.tag == tag) {
            if (code) {
                for (const auto &subfield : field.subfields) {
                    if (sameChar(subfield.code, code)) {
                        if (!subfield.value.empty()) {
                            result.push_back(subfield.value);
                        }
                    }
                }
            } else {
                if (!field.value.empty()) {
                    result.push_back(field.value);
                }
            }
        }
    }

    return result;
}

RecordField* MarcRecord::getField(int tag, int occurrence = 0) const noexcept
{
    for (const auto &field : this->fields) {
        if (field.tag == tag) {
            if (!occurrence) {
                return const_cast<RecordField*> (&field);
            }
        }
    }

    return nullptr;
}

std::vector<RecordField*> MarcRecord::getFields(int tag) const
{
    std::vector<RecordField*> result;
    for (const auto &field : this->fields) {
        if (field.tag == tag) {
            auto ptr = const_cast<RecordField*>(&field);
            result.push_back(ptr);
        }
    }

    return result;
}

MarcRecord& MarcRecord::reset() noexcept
{
    this->mfn = 0;
    this->status = 0;
    this->version = 0;
    this->database = String();

    return *this;
}

bool MarcRecord::verify(bool throwOnError) const
{
    bool result = true;
    for (const RecordField &field : fields) {
        if (!field.verify(throwOnError)) {
            result = false;
        }
    }

    if (!result && throwOnError) {
        throw VerificationException();
    }

    return result;
}

PLUSIRBIS_EXPORTS std::wostream& operator << (std::wostream &stream, const MarcRecord &record)
{
    stream << std::to_wstring(record.mfn) << String(L"#")
        << std::to_wstring(record.status) << std::endl;
    stream << String(L"0#") << record.version << std::endl;
    for (const RecordField &field : record.fields) {
        stream << field << std::endl;
    }

    return stream;
}

}
