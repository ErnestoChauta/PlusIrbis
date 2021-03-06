﻿// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "irbis.h"
#include "irbis_internal.h"

#if defined(_MSC_VER)
#pragma warning(disable: 4068)
#endif

#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "cert-err58-cpp"

namespace irbis {

static String formatPair (const String &prefix, const String &value, const String &defaultValue)
{
    if (sameString(value, defaultValue)) {
        return L"";
    }

    return prefix + L"=" + value + L";";
}

/// \brief Формирование текстового представления настроек пользователя.
/// \return Текстовое представление.
String UserInfo::toString() const
{
    return this->name    + L"\r\n"
        + this->password + L"\r\n"
        + formatPair (L"C", this->cataloger,     L"irbisc.ini")
        + formatPair (L"R", this->reader,        L"irbisr.ini")
        + formatPair (L"B", this->circulation,   L"irbisb.ini")
        + formatPair (L"M", this->acquisitions,  L"irbism.ini")
        + formatPair (L"K", this->provision,     L"irbisk.ini")
        + formatPair (L"A", this->administrator, L"irbisa.ini");
}

/// \brief Разбор ответа сервера.
/// \param lines Строки с ответом сервера.
/// \return Вектор пользовательских настроек.
std::vector<UserInfo> UserInfo::parse (const StringList &lines)
{
    std::vector<UserInfo> result;

    if (lines.empty()) {
        return result;
    }

    const std::size_t userCount    = static_cast<std::size_t> (fastParse32 (safeAt (lines, 0)));
    const std::size_t linesPerUser = static_cast<std::size_t> (fastParse32 (safeAt (lines, 1)));
    if (userCount == 0 || linesPerUser == 0) {
        return result;
    }

    std::size_t index = 2;
    for (std::size_t i = 0; i < userCount; i++) {
        if ((lines.size() - index) < linesPerUser) {
            break;
        }

        UserInfo user;
        user.number        = lines[index++];
        user.name          = lines[index++];
        user.password      = lines[index++];
        user.cataloger     = lines[index++];
        user.reader        = lines[index++];
        user.circulation   = lines[index++];
        user.provision     = lines[index++];
        user.administrator = lines[index++];

        result.push_back(user);
    }

    return result;
}

}
