// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "stdafx.h"

NAMESPACE_IRBIS_BEGIN

const char IrbisText::CrLf[] = { 0x0D, 0x0A };
const char IrbisText::Lf[] = { 0x0A };

// ����������� ����� � �����.
const std::wstring IrbisText::IrbisDelimiter = L"\u001F\u001E";

// �������� ������ ����������� �����.
const std::wstring IrbisText::ShortDelimiter = L"\u001E";

// ����������� ����� � MSDOS
const std::wstring IrbisText::MsDosDelimiter = L"\r\n";

// ����������� ����� � UNIX.
const std::wstring IrbisText::UnixDelimiter = L"\n";

const std::wstring IrbisText::SearchDelimiter = L"#";

// ������ ������������ ������ � ����� �� MS-DOS.

std::wstring IrbisText::fromIrbisToDos (std::wstring &text)
{
    throw NotImplementedException();
    //return text.replace(IrbisDelimiter, MsDosDelimiter);
}

// ������ ������������ ������ � MS-DOS �� �����.
std::wstring IrbisText::fromDosToIrbis (std::wstring &text)
{
    throw NotImplementedException();
    //return text.replace(MsDosDelimiter, IrbisDelimiter);
}

// ������ ������������ ������ � MS-DOS �� UNIX.
std::wstring IrbisText::fromDosToUnix(std::wstring &text)
{
    throw NotImplementedException();
    //return text.replace(MsDosDelimiter, UnixDelimiter);
}

// �������� ������ ������� �� ������� (������ ������� �����������).
StringList IrbisText::fromFullDelimiter (const std::wstring &text)
{
    throw NotImplementedException();
    //return text.split(IrbisDelimiter);
}

// ������, ���������� � ����� �� ������� WriteRecord.
StringList IrbisText::fromShortDelimiter(const std::wstring &text)
{
    throw NotImplementedException();
    //return text.split(ShortDelimiter);
}

// ��������� ���� ���� ��� ������ � ��������� ANSI
std::wstring IrbisText::readAllAnsi(const std::wstring &filename)
{
    throw NotImplementedException();
    //QFile file(filename);
    //if (!file.open(QIODevice::ReadOnly)) {
    //    throw IrbisException();
    //}

    //QByteArray bytes = file.readAll();
    //std::wstring result = IrbisEncoding::ansi()->toUnicode(bytes);

    //return result;
}

// ��������� ���� ���� ��� ������ � ��������� UTF-8
std::wstring IrbisText::readAllUtf(const std::wstring &filename)
{
    throw NotImplementedException();
    //QFile file(filename);
    //if (!file.open(QIODevice::ReadOnly)) {
    //    throw IrbisException();
    //}

    //QByteArray bytes = file.readAll();
    //std::wstring result = IrbisEncoding::ansi()->toUnicode(bytes);

    //return result;
}

// ��������� ���� ���� ��� ������ ����� � ��������� ANSI
StringList IrbisText::readAnsiLines(const std::wstring &filename)
{
    throw NotImplementedException();
    //QFile file(filename);
    //if (!file.open(QIODevice::ReadOnly)) {
    //    throw IrbisException();
    //}

    //QStringList result;
    //QTextCodec *ansi = IrbisEncoding::ansi();
    //while (true) {
    //    QByteArray bytes = file.readLine();
    //    if (bytes.isEmpty()) {
    //        break;
    //    }

    //    std::wstring line = ansi->toUnicode(bytes);
    //    result.append(line);
    //}

    //return result;
}

// ��������� ���� ���� ��� ������ ����� � ��������� UTF-8
StringList IrbisText::readUtfLines(const std::wstring &filename)
{
    throw NotImplementedException();
    //QFile file(filename);
    //if (!file.open(QIODevice::ReadOnly)) {
    //    throw IrbisException();
    //}

    //QStringList result;
    //QTextCodec *utf = IrbisEncoding::utf();
    //while (true) {
    //    QByteArray bytes = file.readLine();
    //    if (bytes.isEmpty()) {
    //        break;
    //    }

    //    std::wstring line = utf->toUnicode(bytes);
    //    result.append(line);
    //}

    //return result;
}

NAMESPACE_IRBIS_END
