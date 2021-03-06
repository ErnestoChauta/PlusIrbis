// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "irbis.h"

#if defined(_MSC_VER)
#pragma warning(disable: 4068)
#endif

#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

namespace irbis {

/// \brief Добавление дочернего узла с указанным значением.
/// \param name Имя (значение) узла.
/// \return Добавленный узел.
TreeNode& TreeNode::add (const String &name)
{
    this->children.emplace_back (name);
    return this->children.back();
}

/// \brief Добавление дочернего узла с указанным значением.
/// \param name Имя (значение) узла.
/// \return Добавленный узел.
TreeNode& TreeNode::add (String &&name)
{
    this->children.emplace_back (std::move (name));
    return this->children.back();
}

}
