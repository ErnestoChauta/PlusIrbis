// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "irbis.h"

namespace irbis {

DatabaseInfo DatabaseInfo::parse(ServerResponse &response)
{
    throw NotImplementedException();
}

std::vector<DatabaseInfo> DatabaseInfo::parse(MenuFile &menu)
{
    throw NotImplementedException();
}

std::wstring DatabaseInfo::toString() const
{
    throw NotImplementedException();
}

}