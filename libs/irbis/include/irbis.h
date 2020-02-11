﻿// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#ifndef IRBIS_H
#define IRBIS_H

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers

// ReSharper disable CppClangTidyCppcoreguidelinesMacroUsage

#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
// #include <future>
#include <ios>
#include <list>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <vector>

//=========================================================

#if !defined(IRBIS_WINDOWS) && !defined (IRBIS_UNIX)

    #if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(_WINDOWS)

        #define IRBIS_WINDOWS

    #else

        #define IRBIS_UNIX

    #endif

#endif

//=========================================================

#ifdef IRBIS_DYNAMIC_LIBRARY

    #if defined(_MSC_VER)
        // Microsoft
        #define IRBIS_EXPORT __declspec(dllexport)
        #define IRBIS_IMPORT __declspec(dllimport)
    #elif defined(__GNUC__)
        //  GCC
        #define IRBIS_EXPORT __attribute__((visibility("default")))
        #define IRBIS_IMPORT
    #else
        //  do nothing and hope for the best?
        #define IRBIS_EXPORT
        #define IRBIS_IMPORT
        #pragma warning Unknown dynamic link import/export semantics.
    #endif

#else

    #define IRBIS_EXPORT
    #define IRBIS_IMPORT

#endif

//=========================================================

#ifdef PLUSIRBIS_LIBRARY

    #define PLUSIRBIS_EXPORTS IRBIS_EXPORT

#else

    #define PLUSIRBIS_EXPORTS IRBIS_IMPORT

#endif

//=========================================================

#if defined(_MSC_VER)

    #pragma warning(push)
    #pragma warning(disable: 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
    #pragma warning(disable: 4275) // non - DLL-interface class 'class_1' used as base for DLL-interface class 'class_2'

#endif

//=========================================================

namespace irbis
{

class AlphabetTable;
class ChunkedBuffer;
class ClientInfo;
class ClientQuery;
class ClientSocket;
class Connection;
class ConnectionFactory;
class DatabaseInfo;
class Date;
class DirectAccess64;
class Ean13;
class Ean8;
class EmbeddedField;
class Encoding;
class Exemplar;
class File;
class FileSpecification;
class Format;
class FoundLine;
class GblResult;
class GblSettings;
class GblStatements;
class IlfEntry;
class IlfFile;
class IniFile;
class IniLine;
class IniSection;
class IrbisException;
class Isbn;
class Iso2709;
class LiteField;
class LiteRecord;
class LiteSubField;
class MarcRecord;
class MarcRecordList;
class MemoryChunk;
class MenuEntry;
class MenuFile;
class MstControlRecord64;
class MstdDictionaryEntry64;
class MstFile64;
class MstRecord64;
class MstRecordLeader64;
class NetworkException;
class NotImplementedException;
class NumberChunk;
class NumberText;
class OptFile;
class OptLine;
class ParFile;
class PhantomField;
class PhantomRecord;
class PhantomSubField;
class PostingParameters;
class ProcessInfo;
class ProtocolText;
class RawRecord;
class RecordField;
class RecordSerializer;
class Search;
class SearchParameters;
class SearchScenario;
class ServerResponse;
class ServerStat;
class StopWords;
class SubField;
class TableDefinition;
class TermInfo;
class TermParameters;
class TermPosting;
class Text;
class TextNavigator;
class TreeFile;
class TreeNode;
class UserInfo;
class VerificationException;
class Version;
class XrfFile64;
class XrfRecord64;

//=========================================================

using Byte = std::uint8_t;
using Bytes = std::vector<Byte>;
using Char = wchar_t;
using Chars = std::vector<Char>;
using String = std::wstring;
using Flag = std::uint32_t;
using Mfn = std::uint32_t;
using MfnList = std::vector<Mfn>;
using Offset = std::uint64_t;
using StringList = std::vector<String>;
using SubFieldList = std::vector<SubField>;
using RecordFieldList = std::vector<RecordField>;

//=========================================================

/// \brief Результат выполнения функции (с учетом успеха/ошибки).
/// \tparam T Тип результата.
template<class T>
class Result // NOLINT(cppcoreguidelines-pro-type-member-init)
{
public:
    bool success { false }; ///< Признак успешного завершения.
    T result; ///< Результирующее значение (имеет смысл при успешном завершении).
    String errorMessage; ///< Сообщение об ошибке.

    /// \brief Преобразование к логическому значению. true означает успех.
    operator bool() const // NOLINT(hicpp-explicit-conversions)
    {
        return this->success;
    }

    /// \brief Получение результирующего значения.
    operator T() const // NOLINT(hicpp-explicit-conversions)
    {
        return this->result;
    }

    /// \brief Формирование результата успешного выполнения.
    static Result Success (T value)
    {
        Result result;
        result.success = true;
        result.result = value;
        return result;
    }

    /// \brief Формирование признака ошибки.
    static Result Failure (const String &message)
    {
        Result result;
        result.success = false; //-V1048
        result.errorMessage = message;
        return result;
    }
};

//=========================================================

/// \brief Непрерывный кусок памяти.
/// \tparam T Тип элемента.
template<class T>
class Span final
{
public:
    T *ptr { nullptr };  ///< Указатель на начала куска.
    size_t length { 0 }; ///< Длина куска в элементах.

    Span() : ptr(nullptr), length(0) {}; ///< Конструктор по умолчанию.
    Span (T *ptr_, std::size_t length_) noexcept : ptr(ptr_), length(length_) {}
    Span (const T *ptr_, std::size_t length_) noexcept : ptr(const_cast<T*>(ptr_)), length(length_) {}
    Span (const std::vector<T> &vec) : ptr(const_cast<T*>(vec.data())), length(vec.size()) {}
    Span (const std::basic_string<T> &str) : ptr(const_cast<T*>(str.data())), length(str.size()) {}

    /// \brief Спан из ASCIIZ-строки.
    /// \param ptr Указатель на начало строки.
    /// \return Полученный спан.
    static Span<T> fromString (const T *ptr) noexcept
    {
        std::size_t length = 0;
        const T *p = ptr;
        while (*p) {
            ++p;
            ++length;
        }
        return Span<T> (const_cast<T*>(ptr), length);
    }

    /// \brief Сырой указатель на данные.
    /// \return Сырой указатель.
    T* data() const noexcept { return this->ptr; }

    /// \brief Константный сырой указатель на данные.
    /// \return Сырой указатель.
    const T* cdata() const noexcept { return this->ptr; }

    /// \brief Пустой спан?
    /// \return `true` если пустой.
    bool empty() const noexcept { return this->length == 0; }

    /// \brief Длина данных в элементах.
    /// \return Длина.
    std::size_t size() const noexcept { return this->length; }

    /// \brief Оператор индексирования.
    /// \param offset Смещение.
    /// \return Ссылка на элемент с указанным смещением.
    T& operator[](std::ptrdiff_t offset) const noexcept { return this->ptr[offset]; }

    /// \brief Содержит ли спан указанный элемент.
    /// \param val Искомое значение.
    /// \return `true` если содержит.
    bool contains (const T val) const noexcept
    {
        for(const auto item : *this) {
            if (item == val) {
                return true;
            }
        }
        return false;
    }

    /// \brief Заполнение спана указанным значением.
    /// \param val Значение для заполнения.
    /// \return this.
    Span<T>& fill (const T val) noexcept
    {
        for(std::size_t ofs = 0; ofs < this->length; ++ofs) {
            this->ptr[ofs] = val;
        }
        return *this;
    }

    /// \brief Поиск указанного значения в спане с начала.
    /// \param val Искомое значение.
    /// \return Индекс первого вхождения найденного элемента,
    /// либо отрицательное значение.
    std::ptrdiff_t indexOf (const T val) const noexcept
    {
        for (std::size_t i = 0; i < this->length; ++i) {
            if (this->ptr[i] == val) {
                return static_cast<std::ptrdiff_t >(i);
            }
        }
        return -1;
    }

    /// \brief Поиск указанного значения в спане с конца.
    /// \param val Искомое значение.
    /// \return Индекс последнего вхождения найденного элемента,
    /// либо отрицательное значение.
    std::ptrdiff_t lastIndexOf(const T val) const noexcept
    {
        for (auto i = static_cast<std::ptrdiff_t>(this->length - 1); i >= 0; --i) {
            if (this->ptr[i] == val) {
                return i;
            }
        }
        return -1;
    }

    /// \brief Срез спана.
    /// \param start Смещение начала спана.
    /// \param length_ Длина среза.
    /// \return Срез.
    ///
    /// Копирования данных не происходит.
    Span<T> slice (std::ptrdiff_t start, std::ptrdiff_t length_=-1) const noexcept
    {
        if (length_ == -1) {
            length_ = this->length - start;
        }

        if (length_ < 0) {
            length_ = 0;
        }

        return Span<T>(this->ptr + start, length_);
    }

    /// \brief Сырой указатель на начало спана.
    /// \return Указатель.
    T* begin() const noexcept { return &this->ptr[0]; }

    /// \brief Константный сырой указатель на начало спана.
    /// \return Указатель.
    const T* cbegin() const noexcept { return &this->ptr[0]; }

    /// \brief Сырой указатель за концом спана.
    /// \return Указатель.
    T* end() const noexcept { return &this->ptr[this->length]; }

    /// \brief Константный сырой указатель за концом спана.
    /// \return Указатель.
    const T* cend() const noexcept { return &this->ptr[this->length]; }

    /// \brief Конверсия в строку.
    /// \return Строка.
    /// \warning Копирует данные из спана!
    std::basic_string<T> toString() const
    {
        std::basic_string<T> result;
        result.reserve(this->length);
        for (std::size_t ofs = 0; ofs < this->length; ofs++) {
            result.push_back(this->ptr[ofs]);
        }
        return result;
    }

    /// \brief Конверсия в вектор.
    /// \return Вектор.
    /// \warning Копирует данные из спана!
    std::vector<T> toVector() const
    {
        std::vector<T> result;
        result.reserve(this->length);
        for (std::size_t ofs = 0; ofs < this->length; ofs++) {
            result.push_back(this->ptr[ofs]);
        }

        return result;
    }

    /// \brief Совпадает ли начало спана с другим спаном?
    /// \param prefix Префикс.
    /// \return `true` если совпадает.
    bool startsWith (const Span<T> prefix) const noexcept
    {
        if (prefix.size() > this->size()) {
            return false;
        }

        auto p1 = this->cbegin();
        auto p2 = prefix.cbegin(), e2 = prefix.cend();

        while (p2 < e2) {
            if (*p1 != *p2) {
                return false;
            }
            ++p1; ++p2;
        }

        return true;
    }

    /// \brief Совпадает ли конец спана с другим спаном?
    /// \param suffix Суффикс.
    /// \return `true` если совпадает.
    bool endsWith (const Span<T> suffix) const noexcept
    {
        if (suffix.size() > this->size()) {
            return false;
        }

        auto p1 = this->cend() - suffix.size();
        auto p2 = suffix.cbegin(), e2 = suffix.cend();

        while (p2 < e2) {
            if (*p1 != *p2) {
                return false;
            }
            ++p1; ++p2;
        }

        return true;
    }

    /// \brief Поэлементное сравнение двух диапазонов.
    /// \param other Диапазон для сравнения.
    /// \return Результат сравнения:
    /// 0 если все элементы совпадают.
    int compare (const Span<T> other) const
    {
        auto size = this->size();
        if (other.size() < size) {
            size = other.size();
        }

        auto p1 = this->cbegin(), e2 = this->cbegin() + size;
        auto p2 = other.cbegin();

        while (p1 < e2) {
            const int result = *p1 - *p2;
            if (result != 0) {
                return result;
            }
            ++p1; ++p2;
        }

        if (this->size() == other.size()) {
            return 0;
        }

        if (this->size() > other.size()) {
            return 1;
        }

        return -1;
    }

    /// \brief Разбиение текста на фрагменты.
    /// \param delimiter Символ-разделитель.
    /// \return Вектор спанов.
    ///
    /// Пустые фрагменты не включаются в результат.
    std::vector<Span<T>> split (T delimiter) const
    {
        std::vector<Span<T>> result;
        auto start = this->cbegin(), current = start, end = this->cend();

        while (start < end) {
            while (current < end) {
                if (*current == delimiter) {
                    break;
                }
                ++current;
            }
            if (current != start) {
                result.emplace_back(start, current - start);
            }
            start = ++current;
        }

        return result;
    }

    /// \brief Разбиение текста на фрагменты.
    /// \param delimiter Символ-разделитель.
    /// \param nelem Максимальное количество элементов
    /// \return Вектор спанов.
    ///
    /// Пустые фрагменты не включаются в результат.
    std::vector<Span<T>> split (T delimiter, int nelem) const
    {
        std::vector<Span<T>> result;
        auto start = this->cbegin(), current = start, end = this->cend();

        --nelem;
        while (nelem > 0 && start < end) {
            while (current < end) {
                if (*current == delimiter) {
                    break;
                }
                ++current;
            }
            if (current != start) {
                result.emplace_back(start, current - start);
            }
            start = ++current;
            --nelem;
        }
        if (nelem <= 0 && start < end) {
            result.emplace_back(start, end - start);
        }


        return result;
    }

    /// \brief Быстрый и грязный разбор спана как целого числа.
    /// \return Результат разбора.
    ///
    /// \warning Мусор на входе -- мусор на выходе!
    int32_t parseInt32() const noexcept
    {
        int32_t result = 0;
        auto p = this->cbegin(), e = this->cend();
        while (p < e) {
            result = result * 10 + (*p - '0');
            ++p;
        }

        return result;
    }

    /// \brief Быстрый и грязный разбор спана как целого числа.
    /// \return Результат разбора.
    ///
    /// \warning Мусор на входе -- мусор на выходе!
    int64_t parseInt64() const noexcept
    {
        int64_t result = 0;
        auto p = this->cbegin(), e = this->cend();
        while (p < e) {
            result = result * 10 + (*p - '0');
            ++p;
        }

        return result;
    }
};

template <class T>
inline bool operator == (const Span<T> left, const Span<T> right)
{
    return left.compare(right) == 0;
}

template <class T>
inline bool operator == (const Span<T> left, const std::basic_string<T> &right)
{
    return left.compare(Span<T>(right.data(), right.size())) == 0;
}

template <class T>
inline bool operator == (const Span<T> left, const std::vector<T> &right)
{
    return left.compare(Span<T>(right.data(), right.size())) == 0;
}

template <class T>
inline bool operator != (const Span<T> left, const Span<T> right)
{
    return left.compare(right) != 0;
}

template <class T>
inline bool operator != (const Span<T> left, const std::basic_string<T> &right)
{
    return left.compare(Span<T>(right.data(), right.size())) != 0;
}

template <class T>
inline bool operator != (const Span<T> left, const std::vector<T> &right)
{
    return left.compare(Span<T>(right.data(), right.size())) != 0;
}

template <class T>
inline bool operator < (const Span<T> left, const Span<T> right)
{
    return left.compare(right) < 0;
}

template <class T>
inline bool operator < (const Span<T> left, const std::basic_string<T> &right)
{
    return left.compare(Span<T>(right.data(), right.size())) < 0;
}

template <class T>
inline bool operator < (const Span<T> left, const std::vector<T> &right)
{
    return left.compare(Span<T>(right.data(), right.size())) < 0;
}

template <class T>
inline bool operator <= (const Span<T> left, const Span<T> right)
{
    return left.compare(right) <= 0;
}

template <class T>
inline bool operator <= (const Span<T> left, const std::basic_string<T> &right)
{
    return left.compare(Span<T>(right.data(), right.size())) <= 0;
}

template <class T>
inline bool operator <= (const Span<T> left, const std::vector<T> &right)
{
    return left.compare(Span<T>(right.data(), right.size())) <= 0;
}

template <class T>
inline bool operator > (const Span<T> left, const Span<T> right)
{
    return left.compare(right) > 0;
}

template <class T>
inline bool operator > (const Span<T> left, const std::basic_string<T> &right)
{
    return left.compare(Span<T>(right.data(), right.size())) > 0;
}

template <class T>
inline bool operator > (const Span<T> left, const std::vector<T> &right)
{
    return left.compare(Span<T>(right.data(), right.size())) > 0;
}

template <class T>
inline bool operator >= (const Span<T> left, const Span<T> right)
{
    return left.compare(right) >= 0;
}

template <class T>
inline bool operator >= (const Span<T> left, const std::basic_string<T> &right)
{
    return left.compare(Span<T>(right.data(), right.size())) >= 0;
}

template <class T>
inline bool operator >= (const Span<T> left, const std::vector<T> &right)
{
    return left.compare(Span<T>(right.data(), right.size())) >= 0;
}

using CharSpan = Span<char>;
using WideSpan = Span<Char>;
using ByteSpan = Span<Byte>;

PLUSIRBIS_EXPORTS bool sameString (CharSpan first, CharSpan second);
PLUSIRBIS_EXPORTS bool sameString (WideSpan first, WideSpan second);
PLUSIRBIS_EXPORTS CharSpan trimStart (CharSpan text);
PLUSIRBIS_EXPORTS WideSpan trimStart (WideSpan text);
PLUSIRBIS_EXPORTS CharSpan trimEnd (CharSpan text);
PLUSIRBIS_EXPORTS WideSpan trimEnd (WideSpan text);
PLUSIRBIS_EXPORTS CharSpan trim (CharSpan text);
PLUSIRBIS_EXPORTS WideSpan trim (WideSpan text);
PLUSIRBIS_EXPORTS CharSpan toupper (CharSpan text);
PLUSIRBIS_EXPORTS WideSpan toupper (WideSpan text);
PLUSIRBIS_EXPORTS CharSpan tolower (CharSpan text);
PLUSIRBIS_EXPORTS WideSpan tolower (WideSpan text);

//=========================================================

/// \brief Перечислитель объектов. Абстрактный базовый класс.
/// \tparam T Тип перечисляемых объектов
///
/// Поддерживает простой перебор по некоторой коллекции.
template<class T>
class Enumerator
{
public:
    bool moveNext() = 0;   ///< Перемещает перечислитель к следующему элементу коллекции.
    T current() const = 0; ///< Возвращает элемент коллекции, соответствующий текущей позиции перечислителя.
};

//=========================================================

/// \brief Базовое исключение для всех нештатных ситуаций.
class PLUSIRBIS_EXPORTS IrbisException
        : public std::exception
{
public:
};

//=========================================================

/// \brief Файл на сервере не найден.
class PLUSIRBIS_EXPORTS FileNotFoundException
        : public IrbisException
{
    String _fileName;

public:

    explicit FileNotFoundException(const String &fileName)  // NOLINT(modernize-pass-by-value)
        : _fileName(fileName)
    {
    }
};

//=========================================================

/// \brief Штрих-код длиной 13 символов.
class PLUSIRBIS_EXPORTS Ean13 final
{
public:
    static char computeCheckDigit (CharSpan text);
    static Char computeCheckDigit (WideSpan text);
    static bool checkControlDigit (CharSpan text);
    static bool checkControlDigit (WideSpan text);
};

//=========================================================

/// \brief Штрих-код длиной 8 символов.
class PLUSIRBIS_EXPORTS Ean8 final
{
public:
    static char computeCheckDigit (CharSpan text);
    static Char computeCheckDigit (WideSpan text);
    static bool checkControlDigit (CharSpan text);
    static bool checkControlDigit (WideSpan text);
};

//=========================================================

/// \brief INI-файл.
class PLUSIRBIS_EXPORTS IniFile final
{
public:
    std::vector<IniSection> sections; ///< Секции INI-файла.

    IniFile& clear();
    bool containsSection (const String &name) const noexcept;
    IniSection& createSection(const String &name);
    bool modified() const noexcept;
    void notModified();
    std::ptrdiff_t getIndex(const String &name) const noexcept;
    IniSection* getSection(const String &name) const noexcept;
    const String& getValue(const String &sectionName, const String &keyName, const String &defaultValue) const noexcept;
    void parse(const StringList &lines);
    IniFile& removeSection(const String &sectionName);
    IniFile& removeValue(const String &sectionName, const String &keyName);
    IniFile& setValue(const String &sectionName, const String &keyName, const String &value);
    //void write(const std::wstring &filename, QTextCodec *encoding) const;
    //void writeModifiedOnly(QTextStream &stream) const;
};

//=========================================================

/// \brief Строка INI-файла.
///
/// Состоит из ключа и значения, разделённых символом '='.
class PLUSIRBIS_EXPORTS IniLine final
{
public:
    String key;    ///< Ключ (не должен быть пустым).
    String value;  ///< Значение (может быть пустым).

    bool modified() const noexcept;
    void notModified() noexcept;
    void setKey(const String &newKey) noexcept;
    void setValue(const String &newValue) noexcept;
    String toString() const;

private:
    bool _modified { false };
};

//=========================================================

/// \brief Секция INI-файла.
class PLUSIRBIS_EXPORTS IniSection final
{
public:
    String name; ///< Имя секции.
    std::vector<IniLine> lines; ///< Строки, входящие в секцию. \see IniLine

    IniSection& clear();
    bool containsKey(const String &key) const noexcept;
    std::ptrdiff_t getIndex(const String &key) const noexcept;
    IniLine* getLine(const String &key) const noexcept;
    const String& getValue(const String &key, const String &defaultValue) const noexcept;
    bool modified() const noexcept;
    void notModified();
    IniSection& removeValue(const String &key);
    IniSection& setValue(const String &key, const String &value);
    String toString() const;

    const String& operator[] (const String &index) const noexcept;
};

//=========================================================

/// \brief Таблица алфавитных символов.
///
/// Таблица алфавитных символов используется системой ИРБИС
/// при разбиении текста на слова и представляет собой список
/// кодов символов, которые считаются алфавитными.
///
class PLUSIRBIS_EXPORTS AlphabetTable final
{
public:
    const static String FileName;
    std::set<Char> characters;

    /// Конструктор.
    explicit AlphabetTable(const std::vector<Byte> &bytes);

    /// Синглтон.
    static const AlphabetTable& instance();

    /// Проверка, является ли указанный символ буквой.
    bool isAlpha(const Char &c) const;

    /// Парсинг потока.
    static AlphabetTable parse(std::istream &stream);

    /// Чтение локального файла.
    static AlphabetTable readLocalFile(const String &fileName);

    /// Удаление пробелов в начале и в конце строки.
    String trimText(const String &text) const;

    /// Разбиение текста на слова.
    StringList splitWords(const String &text) const;

    /// Верификация таблицы.
    bool verify(bool throwOnError) const;
};

//=========================================================

/// \brief Информация о подключенном клиенте.
class PLUSIRBIS_EXPORTS ClientInfo final
{
public:
    String number;        ///< Порядковый номер.
    String ipAddress;     ///< Адрес клиента.
    String port;          ///< Порт клиента.
    String name;          ///< Логин пользователя.
    String id;            ///< Идентификатор клиентской программы (просто уникальное число).
    String workstation;   ///< Тип клиентского АРМ.
    String registered;    ///< Момент подключения к серверу.
    String acknowledged;  ///< Последнее подтверждение, посланное серверу.
    String lastCommand;   ///< Последняя команда, посланная серверу.
    String commandNumber; ///< Порядковый номер последней программы.
};

//=========================================================

/// \brief Подключение к серверу ИРБИС64.
///
/// Объекты данного типа неперемещаемые.
class PLUSIRBIS_EXPORTS Connection final
{
private:
    bool _connected;
    StringList _databaseStack;
    std::mutex _mutex;

    bool _checkConnection();

    friend class ServerResponse;

public:
    String host;          ///< Адрес сервера в виде строки.
    short port;           ///< Номер порта сервера.
    String username;      ///< Логин пользователя.
    String password;      ///< Пароль пользователя.
    String database;      ///< Имя текущей базы данных.
    String workstation;   ///< Тип клиента.
    int clientId;         ///< Уникальный идентификатор клиента.
    int queryId;          ///< Порядковый номер команды, отсылаемой на сервер.
    int lastError;        ///< Код ошибки, возвращённый сервером в ответ на последнюю команду.
    String serverVersion; ///< Версия сервера (получается при подключении).
    IniFile iniFile;      ///< Содержимое серверного INI-файла для данного клиента.
    int interval { 0 };   ///< Интервал автоматического подтверждения.
    ClientSocket *socket; ///< Клиентский сокет.

    Connection();
    Connection(const Connection&) = delete;
    Connection(Connection&&) = delete;
    Connection& operator=(const Connection&) = delete;
    Connection& operator=(Connection&&) = delete;
    ~Connection();

    bool actualizeDatabase (const String &databaseName);
    bool actualizeRecord (const String &databaseName, int mfn);
    bool connect();
    // std::future<bool> connectAsync();
    bool createDatabase (const String &databaseName, const String &description, bool readerAccess);
    bool createDictionary (const String &databaseName);
    bool deleteDatabase (const String &databaseName);
    bool deleteRecord (int mfn);
    bool connected() const noexcept { return this->_connected; }
    void disconnect();
    // std::future<void> disconnectAsync();
    bool execute (ClientQuery &query);
    // std::future<bool> executeAsync(ClientQuery &query);
    String formatRecord (const String &format, Mfn mfn);
    std::string formatRecordLite (const std::string &format, Mfn mfn);
    String formatRecord (const String &format, const MarcRecord &record);
    DatabaseInfo getDatabaseInfo (const String &databaseName);
    Mfn getMaxMfn (const String &databaseName);
    GblResult globalCorrection (const GblSettings &settings);
    ServerStat getServerStat();
    Version getServerVersion();
    std::vector<UserInfo> getUserList();
    std::vector<DatabaseInfo> listDatabases (const IniFile &iniFile, const String &defaultFileName);
    std::vector<DatabaseInfo> listDatabases (const FileSpecification &specification);
    StringList listFiles (const FileSpecification &specification);
    StringList listFiles (const std::vector<FileSpecification> &specifications);
    std::vector<ProcessInfo> listProcesses();
    StringList listTerms (const String &prefix);
    bool noOp();
    // std::future<bool> noOpAsync();
    void parseConnectionStringUtf (const std::string &connectionString);
    void parseConnectionString (const String &connectionString);
    String popDatabase();
    String printTable (const TableDefinition &definition);
    String pushDatabase (const String &newDatabase);
    Bytes readBinaryFile (const FileSpecification &specification);
    IniFile readIniFile (const FileSpecification &specification);
    MenuFile readMenuFile (const FileSpecification &specification);
    std::vector<TermPosting> readPostings(const PostingParameters &parameters);
    RawRecord readRawRecord (Mfn mfn);
    MarcRecord readRecord (Mfn mfn);
    LiteRecord readLiteRecord (Mfn mfn);
    MarcRecord readRecord (const String &databaseName, Mfn mfn);
    MarcRecord readRecord (const String &databaseName, Mfn mfn, int version);
    std::vector<MarcRecord> readRecords (const MfnList &mfnList);
    std::vector<SearchScenario> readSearchScenario (const FileSpecification &specification);
    std::vector<TermInfo> readTerms (const String &startTerm, int numberOfTerms);
    std::vector<TermInfo> readTerms (const TermParameters &parameters);
    String readTextFile (const FileSpecification &specification);
    std::string readAnsiFile (const FileSpecification &specification);
    StringList readTextFiles (std::vector<FileSpecification> &specifications);
    StringList  readTextLines (const FileSpecification &specification);
    bool reloadDictionary (const String &databaseName);
    bool reloadMasterFile (const String &databaseName);
    bool restartServer();
    String requireTextFile (const FileSpecification &specification);
    MfnList search (const Search &search);
    MfnList search (const String &expression);
    MfnList search (const SearchParameters &parameters);
    String toConnectionString() const;
    bool truncateDatabase (const String &databaseName);
    bool unlockDatabase (const String &databaseName);
    bool unlockRecords (const String &databaseName, const MfnList &mfnList);
    bool updateIniFile (const StringList &lines);
    bool updateUserList (const std::vector<UserInfo> &users);
    int writeRawRecord (RawRecord &record, bool lockFlag, bool actualize, bool dontParseResponse);
    int writeRecord (MarcRecord &record, bool lockFlag = false,
            bool actualize = true, bool dontParseResponse = false);
    bool writeRecords (std::vector<MarcRecord*> &records, bool lockFlag, bool actualize, bool dontParseResponse);
    bool writeRecords (std::vector<MarcRecord> &records, bool lockFlag, bool actualize, bool dontParseResponse);
    void writeTextFile (const FileSpecification &specification);
};

//=========================================================

/// \brief Фабрика подключений.
class PLUSIRBIS_EXPORTS ConnectionFactory
{
public:
    ConnectionFactory() = default;
    ConnectionFactory(const ConnectionFactory &) = delete;
    ConnectionFactory(ConnectionFactory&&) = delete;
    ConnectionFactory& operator=(const ConnectionFactory&) = delete;
    ConnectionFactory& operator=(ConnectionFactory&&) = delete;
    virtual ~ConnectionFactory() = default;

    virtual Connection* GetConnection();
};

//=========================================================

/// \brief Информация о базе данных ИРБИС.
class PLUSIRBIS_EXPORTS DatabaseInfo final
{
public:
    String name;                      ///< Имя базы данных.
    String description;               ///< Описание базы данных в произвольной форме (может быть пустым).
    MfnList logicallyDeletedRecords;  ///< Список логически удалённых записей (может быть пустым).
    MfnList physicallyDeletedRecords; ///< Список физически удалённых записей (может быть пустым).
    MfnList nonActualizedRecords;     ///< Список неактуализированных записей (может быть пустым).
    MfnList lockedRecords;            ///< Список заблокированных записей (может быть пустым).
    Mfn maxMfn { 0 };                 ///< Максимальный MFN для базы данных.
    bool databaseLocked { false };    ///< Признак блокировки базы данных в целом.
    bool readOnly { false };          ///< База данных доступна только для чтения.

    void parse(ServerResponse &response);
    static std::vector<DatabaseInfo> parse(const MenuFile &menu);
    String toString() const;
};

//=========================================================

/// \brief Режим прямого доступа к базе данных.
enum DirectAccessMode : unsigned int
{
    Exclusive = 0u, ///< Эксклюзивный (чтение-запись).
    Shared    = 1u, ///< Разделяемый (чтение-запись).
    ReadOnly  = 2u  ///< Только чтение.
};

//=========================================================

class PLUSIRBIS_EXPORTS DirectAccess64 final
{
public:
    MstFile64 *mst;
    XrfFile64 *xrf;
    String database;

    DirectAccess64(const String &path);
    DirectAccess64(const DirectAccess64 &) = delete;
    DirectAccess64(const DirectAccess64 &&) = delete;
    DirectAccess64& operator = (const DirectAccess64 &) = delete;
    DirectAccess64& operator = (const DirectAccess64 &&) = delete;
    ~DirectAccess64();

    MstRecord64 readRawRecord(unsigned int mfn);
    MarcRecord readRecord(unsigned int mfn);
};

//=========================================================

class PLUSIRBIS_EXPORTS EmbeddedField final
{
public:

    const static char DefaultCode;

    static RecordFieldList getEmbeddedFields(const RecordField &field, char sign = DefaultCode);
};

//=========================================================

/// \brief Простая абстракция кодировки символов.
class PLUSIRBIS_EXPORTS Encoding // abstract
{
    static Encoding *_ansi;
    static Encoding *_utf;

public:
    Encoding() = default;
    virtual ~Encoding() = default;

    virtual Bytes fromUnicode(const String &text) const = 0;
    virtual String toUnicode(const Byte *bytes, std::size_t count) const = 0;

    static Encoding* ansi();
    static String fromAnsi(const Byte *bytes, std::size_t count);
    static String fromUtf(const Byte *bytes, std::size_t count);
    static Bytes toAnsi(const String &text);
    static Bytes toUtf(const String &text);
    static Encoding* utf();
};

class PLUSIRBIS_EXPORTS Cp1251Encoding final
    : public Encoding
{
public:

    virtual Bytes fromUnicode(const String &text) const override;
    virtual String toUnicode(const Byte *bytes, std::size_t count) const override;
};

class PLUSIRBIS_EXPORTS Utf8Encoding final
    : public Encoding
{
public:

    virtual Bytes fromUnicode(const String &text) const override;
    virtual String toUnicode(const Byte *bytes, std::size_t count) const override;
};

//=========================================================

/// \brief Сведения об экземпляре документа (поле 910).
class PLUSIRBIS_EXPORTS Exemplar final
{
public:
    static const int TAG = 910;

    String status;
    String number;
    String date;
    String place;
    String collection;
    String shelf;
    String price;
    String barcode;
    String amount;
    String purpose;
    String coefficient;
    String offBalance;
    String ksuNumber1;
    String actNumber1;
    String channel;
    String onHand;
    String actNumber2;
    String writeOff;
    String completion;
    String actNumber3;
    String moving;
    String newPlace;
    String checkedDate;
    String checkedAmount;
    String realPlace;
    String bindingIndex;
    String bindingNumber;

    RecordField *field { nullptr };

    void applyTo (RecordField &field) const;
    void parse (const RecordField &field_);
    static std::vector<Exemplar> parse (const MarcRecord &record);
};

//=========================================================

/// \brief Спецификация имени файла.
class PLUSIRBIS_EXPORTS FileSpecification final
{
public:
    bool binaryFile { false }; ///< Признак двоичного файла.
    int path { 0 };            ///< Код пути.
    String database;           ///< Имя базы данных (если необходимо).
    String filename;           ///< Имя файла (обязательно).
    String content;            ///< Содержимое файла (если необходимо).

    FileSpecification() = default; ///< Конструктор по умолчанию.
    FileSpecification(int path, const String &filename);
    FileSpecification(int path, const String &database, const String &filename);
    FileSpecification(const FileSpecification &other) = default; ///< Конструктор копирования.
    FileSpecification(FileSpecification &&other) = default; ///< Конструктор перемещения.
    FileSpecification& operator = (const FileSpecification &other); ///< Оператор копирования.
    FileSpecification& operator = (FileSpecification &&other); ///< Оператор перемещения.
    ~FileSpecification() = default; ///< Деструктор.

    static FileSpecification parse(const String &text);
    bool verify(bool throwException) const;

    String toString() const;
};

//=========================================================

/// \brief Одна строка из списка найденных записей.
class PLUSIRBIS_EXPORTS FoundLine final
{
public:
    Mfn mfn {0};         ///< MFN найденной записи.
    String description;  ///< Опциональный текст, например, результат расформатирования записи.

    FoundLine() = default;
    FoundLine(Mfn mfn_, const String &description_) : mfn(mfn_), description(description_) {}
    FoundLine(const FoundLine &other) = default;
    FoundLine(FoundLine &&other) = default;
    FoundLine& operator = (const FoundLine &other) = default;
    FoundLine& operator = (FoundLine &&other) = default;
    ~FoundLine() = default;
};

//=========================================================

/// \brief Параметр глобальной корректировки.
class PLUSIRBIS_EXPORTS GblParameter final
{
public:
    /// Наименование параметра, которое появится в названии столбца, задающего параметр.
    String title;

    /// Значение параметра или пусто, если пользователю
    /// предлагается задать его значение перед выполнением
    /// корректировки. В этой строке можно задать имя файла
    /// меню (с расширением MNU) или имя рабочего листа подполей
    /// (с расширением Wss), которые будут поданы для выбора
    /// значения параметра.
    String value;
};

//=========================================================

class PLUSIRBIS_EXPORTS GblResult final
{
public:

    void parse(const StringList &lines);
};

//=========================================================

/// \brief Оператор глобальной корректировки.
class PLUSIRBIS_EXPORTS GblStatement final
{
public:
    String command;    ///< Команда, например, ADD или DEL.
    String parameter1; ///< Первый параметр, как правило, спецификация поля/подполя.
    String parameter2; ///< Второй параметр, как правило, спецификация повторения.
    String format1;    ///< Первый формат, например, выражение для замены.
    String format2;    ///< Второй формат, например, заменяющее выражение.

    GblStatement() = default;
    GblStatement(const GblStatement &other) = default;
    GblStatement(GblStatement &&other) = default;
    GblStatement& operator = (const GblStatement &other) = default;
    GblStatement& operator = (GblStatement &&other) = default;
    ~GblStatement() = default;
};

//=========================================================

/// \brief Настройки для глобальной корректировки.
class PLUSIRBIS_EXPORTS GblSettings final
{
public:
    bool actualize { false };             ///< Надо ли актуализировать записи?
    bool autoin { false };                ///< Надо ли запускать `autoin.gbl`?
    String database;                      ///< Имя базы данных.
    String fileName;                      ///< Имя файла глобальной корректировки.
    Mfn firstRecord { 0 };                ///< Mfn первой записи.
    bool formalControl { false };         ///< Применять формальный контроль?
    Mfn maxMfn { 0 };                     ///< Максимальный MFN.
    MfnList mfnList;                      ///< Список MFN для обработки.
    Mfn minMfn { 0 };                     ///< Минимальный MFN. 0 означает "все записи в базе".
    Mfn numberOfRecords { 0 };            ///< Количество записей, подлежащих обработке.
    String searchExpression;              ///< Выражение для прямого поиска по словарю.
    String sequentialSearch;              ///< Выражение для последовательного поиска.
    std::vector<GblStatement> statements; ///< Вектор операторов.
    std::vector<GblParameter> parameters; ///< Вектор параметров.

    GblSettings() = default;
    GblSettings(const GblSettings &other) = default;
    GblSettings(GblSettings &&other) = default;
    GblSettings& operator = (const GblSettings &other) = default;
    GblSettings& operator = (GblSettings &&other) = default;
    ~GblSettings() = default;
};

//=========================================================

/// \brief Запись в ILF-файле. Хранит информацию об одном ресурсе.
class PLUSIRBIS_EXPORTS IlfEntry final
{
public:
    String date;               ///< Дата создания.
    String name;               ///< Имя ресурса.
    String description;        ///< Описание в произвольной форме.
    String data;               ///< Собственно данные.
    uint32_t position { 0 };   ///< Смещение данных от начала файла, байты.
    uint32_t number { 0 };     ///< Порядковый номер ресурса.
    uint32_t dataLength { 0 }; ///< Длина данных в байтах.
    short flags { 0 };         ///< Флаги.
    bool deleted { false };    ///< Признак удаленной записи.
};

//=========================================================

/// \brief Обертка над ILF-файлом.
class PLUSIRBIS_EXPORTS IlfFile final
{
public:
    const static std::string MagicString; ///< Магическая строка в заголовке файла.

    std::vector<IlfEntry> entries; ///< Вектор записей.
    uint32_t unknown1 { 0 };       ///< Неизвестно.
    uint32_t slotCount { 0 };      ///< Количество слотов для хранения записей.
    uint32_t entryCount { 0 };     ///< Количество записей.
    uint32_t writeCount { 0 };     ///< Количество модификаций записей.
    uint32_t deleteCount { 0 };    ///< Количество удаленных записей.

    void readLocalFile(const String &fileName);
};

//=========================================================

/// \brief Специфичная для ИРБИС дата.
class PLUSIRBIS_EXPORTS Date final
{
public:
    String text;          ///< Текстовое представление в формате YYYYMMDD.
    struct tm date { 0 }; ///< Разбиение даты на компоненты.

    Date() = default;                               ///< Конструктор перемещения.
    Date(int year, int month, int day);
    explicit Date(const String &text_);
    explicit Date(const struct tm *date_);
    Date(const Date &other) = default;              ///< Конструктор копирования.
    Date(Date &&other) = default;                   ///< Конструктор перемещения.
    Date& operator = (const Date &other) = default; ///< Оператор копирования.
    Date& operator = (Date &&other) = default;      ///< Оператор перемещения.
    ~Date() = default;                              ///< Деструктор.

    static String convert(const struct tm *date);
    static bool parse(const String &text, struct tm *date);
    static Date today();
};

//=========================================================

enum IrbisPath : unsigned int
{
    System = 0u,
    Data = 1u,
    MasterFile = 2u,
    InvertedFile = 3u,
    ParameterFile = 10u,
    FullText = 11u,
    InternalResource = 12u
};

//=========================================================

/// \brief Международный стандартный книжный номер.
class PLUSIRBIS_EXPORTS Isbn final
{
public:
    static bool check978 (WideSpan isbn) noexcept;
    static bool checkControlDigit (WideSpan isbn) noexcept;
};

//=========================================================

class PLUSIRBIS_EXPORTS Iso2709 final
{
public:
    static const int  MarkerLength = 24;
    static const char RecordDelimiter = 0x1D;
    static const char FieldDelimiter = 0x1E;
    static const char SubFieldDelimiter = 0x1F;

    static MarcRecord* readRecord(FILE *device, const Encoding *encoding);
    static void writeRecord(FILE *device, const MarcRecord &record, const Encoding *encoding);
};

//=========================================================

/// \brief Библиографическая запись. Состоит из произвольного количества полей.
class PLUSIRBIS_EXPORTS MarcRecord final
{
public:
    Mfn mfn { 0u };                ///< MFN (порядковый номер в базе) записи.
    Flag status { 0u };            ///< Статус записи. Представляет собой набор флагов.
    unsigned int version { 0u };   ///< Номер версии записи.
    String database;               ///< Имя базы данных.
    std::list<RecordField> fields; ///< Список полей.

    MarcRecord() = default;                                     ///< Конструктор по умолчанию.
    MarcRecord(const MarcRecord &other) = default;              ///< Конструктор копирования.
    MarcRecord(MarcRecord &&other) = default;                   ///< Конструктор перемещения.
    MarcRecord& operator = (const MarcRecord &other) = default; ///< Оператор копирования.
    MarcRecord& operator = (MarcRecord &&other) = default;      ///< Оператор перемещения.
    ~MarcRecord() = default;                                    ///< Деструктор.

    RecordField& add (int tag, const String &value);
    MarcRecord clone() const;
    void decode (const StringList &lines);
    bool deleted() const noexcept;
    String encode (const String &delimiter = L"\u001F\u001E") const;
    String fm (int tag, Char code = 0) const noexcept;
    StringList fma (int tag, Char code = 0) const;
    RecordField* getField (int tag, int occurrence = 0) const noexcept;
    std::vector<RecordField*> getFields (int tag) const;
    MarcRecord& reset() noexcept;
    bool verify (bool throwOnError) const;

    friend PLUSIRBIS_EXPORTS std::wostream& operator << (std::wostream &stream, const MarcRecord &record);
};

//=========================================================

class PLUSIRBIS_EXPORTS MenuEntry final
{
public:
    std::wstring code;
    std::wstring comment;

    std::wstring toString() const;
};

//=========================================================

class PLUSIRBIS_EXPORTS MenuFile final
{
public:
    const static String StopMarker;
    const static Chars Separators;

    String fileName;
    std::list<MenuEntry> entries;

    MenuFile& add(const String &code, const String &comment);
    MenuEntry* getEntry(const String &code) const noexcept;
    MenuEntry* getEntrySensitive(const String &code) const noexcept;
    String* getValue(const String &code) const noexcept;
    String* getValueSensitive(const String &code) const noexcept;
    const String& getValue(const String &code, const String &defaultValue) const noexcept;
    const String& getValueSensitive(const String &code, const String &defaultValue) const noexcept;
    void parse(std::istream &stream);
    void parse(const StringList &lines);
    void parseLocalFile(const std::wstring &filename /* const QTextCodec *encoding */);
};

//=========================================================

#pragma pack(push, 1)
class PLUSIRBIS_EXPORTS MstControlRecord64 final
{
public:
    const static int RecordSize;
    const static long LockFlagPosition;

    uint32_t ctlMfn { 0 };
    uint32_t nextMfn { 0 };
    int64_t nextPosition { 0 };
    uint32_t mftType { 0 };
    uint32_t recCnt { 0 };
    uint32_t reserv1 { 0 };
    uint32_t reserv2 { 0 };
    uint32_t blocked { 0 };

    void read(FILE *file);
};
#pragma pack(pop)

//=========================================================

#pragma pack(push, 1)
class PLUSIRBIS_EXPORTS MstDictionaryEntry64 final
{
public:
    const static int EntrySize;

    int tag { 0 };
    int position { 0 };
    int length { 0 };
    std::wstring text;

    void read(FILE *file);
};
#pragma pack(pop)

//=========================================================

class PLUSIRBIS_EXPORTS MstFile64 final
{
    FILE *_file;

public:
    MstControlRecord64 control;
    String fileName;

    MstFile64(const String &fileName, DirectAccessMode mode);
    MstFile64(const MstFile64 &) = delete;
    MstFile64(const MstFile64 &&) = delete;
    MstFile64& operator = (const MstFile64 &) = delete;
    MstFile64& operator = (const MstFile64 &&) = delete;
    ~MstFile64();

    MstRecord64 readRecord(long position);
};

//=========================================================

#pragma pack(push, 1)
class PLUSIRBIS_EXPORTS MstRecordLeader64 final
{
public:
    const static int LeaderSize;

    uint32_t mfn { 0 };
    uint32_t length { 0 };
    uint64_t previous { 0 };
    uint32_t base { 0 };
    uint32_t nvf { 0 };
    uint32_t status { 0 };
    uint32_t version { 0 };

    void read(FILE *file);
};
#pragma pack(pop)

//=========================================================

#pragma pack(push, 1)
class PLUSIRBIS_EXPORTS MstRecord64 final
{
public:
    MstRecordLeader64 leader;
    uint64_t offset { 0 };
    std::vector<MstDictionaryEntry64> dictionary;

    bool deleted() const;
    MarcRecord toMarcRecord() const;
};
#pragma pack(pop)

//=========================================================

class PLUSIRBIS_EXPORTS NetworkException final
    : public IrbisException
{
public:
};

//=========================================================

class PLUSIRBIS_EXPORTS NotImplementedException final
    : public IrbisException
{
public:
};

//=========================================================

class PLUSIRBIS_EXPORTS NumberChunk final
{
    bool setUp(const String &str, const String &number);

    friend class NumberText;

public:
    String prefix;
    int64_t value { 0 };
    int length { 0 };
    bool haveValue { false };

    NumberChunk() = default;
    NumberChunk(const NumberChunk &) = delete;
    NumberChunk(NumberChunk &&) = delete;
    NumberChunk& operator = (const NumberChunk &) = delete;
    NumberChunk& operator = (NumberChunk &&) = delete;
    ~NumberChunk() = default;

    int compareTo(const NumberChunk &other) const;
    bool havePrefix() const;
    std::wstring toString() const;
};

//=========================================================

class PLUSIRBIS_EXPORTS NumberText final
{
public:
};

//=========================================================

class PLUSIRBIS_EXPORTS OptFile final
{
public:
    std::vector<OptLine> lines;

    void parse(const StringList &text);
};

//=========================================================

class PLUSIRBIS_EXPORTS OptLine final
{
public:
    std::wstring key;
    std::wstring value;
};

//=========================================================

class PLUSIRBIS_EXPORTS ParFile final
{
public:
    String xrf;
    String mst;
    String cnt;
    String n01;
    String n02;
    String l01;
    String l02;
    String ifp;
    String any;
    String pft;
    String ext;

    void assign (const String &path);
    void parse(const StringList &lines);
    std::map<int, String> toDictionary() const;
};

//=========================================================

/// \brief Невладеющее подполе.
class PLUSIRBIS_EXPORTS PhantomSubField final
{
public:

    /// \brief Одноисмвольный код подполя.
    Byte code { '\0' };
    ByteSpan value; ///< Значение подполя (может быть пустым).

    PhantomSubField() = default;
    PhantomSubField (Byte code_, ByteSpan value_) : code(code_), value(value_) {}
    PhantomSubField (const PhantomSubField &other) = default;
    PhantomSubField (PhantomSubField &&other) = default;
    PhantomSubField& operator = (const PhantomSubField &other) = default;
    PhantomSubField& operator = (PhantomSubField &&other) = default;
    ~PhantomSubField() = default;

    PhantomSubField clone() const;
    void decode (ByteSpan line);
    bool empty() const noexcept;
    bool verify (bool throwOnError) const;
    std::string toString() const;

    SubField materialize() const;
};

//=========================================================

/// \brief Невладеющее поле.
class PLUSIRBIS_EXPORTS PhantomField final
{
public:

    int tag { 0 };
    ByteSpan value;
    std::list<PhantomSubField> subfields;

    PhantomField() = default;
    PhantomField (int tag_, ByteSpan value_) : tag(tag_), value(value_) {}
    PhantomField (const PhantomField &other) = default;
    PhantomField (PhantomField &&other) = default;
    PhantomField& operator = (const PhantomField &other) = default;
    PhantomField& operator = (PhantomField &&other) = default;
    ~PhantomField() = default;

    PhantomField& add (Byte code, ByteSpan value);
    PhantomField& clear();
    PhantomField clone() const;
    void decode (ByteSpan line);
    bool empty() const noexcept;
    PhantomSubField* getFirstSubfield (Byte code) const noexcept;
    ByteSpan getFirstSubfieldValue (Byte code) const noexcept;
    PhantomField& removeSubfield (Byte code);
    PhantomField& setSubfield (Byte code, ByteSpan newValue);
    bool verify (bool throwOnError) const;
    std::string toString() const;

    RecordField materialize() const;
};

//=========================================================

/// \brief Невладеющая запись.
class PLUSIRBIS_EXPORTS PhantomRecord final
{
public:
    Mfn mfn { 0u };                 ///< MFN (порядковый номер в базе) записи.
    Flag status { 0u };             ///< Статус записи. Представляет собой набор флагов.
    unsigned int version { 0u };    ///< Номер версии записи.
    std::list<PhantomField> fields; ///< Список полей.
    std::string database;           ///< База данных.

    PhantomRecord() = default;                                        ///< Конструктор по умолчанию.
    PhantomRecord (const PhantomRecord &other) = default;             ///< Конструктор копирования.
    PhantomRecord (PhantomRecord &&other) = default;                  ///< Конструктор перемещения.
    PhantomRecord& operator = (const PhantomRecord &other) = default; ///< Оператор копирования.
    PhantomRecord& operator = (PhantomRecord &&other) = default;      ///< Оператор перемещения.
    ~PhantomRecord() = default;                                       ///< Деструктор.

    PhantomField& add (int tag, ByteSpan value);
    PhantomRecord clone() const;
    void decode (const std::vector<ByteSpan> &lines);
    bool deleted() const noexcept;
    void encode (ByteSpan buffer, ByteSpan delimiter) const;
    std::string fm (int tag, Byte code = 0) const noexcept;
    std::vector<std::string> fma (int tag, char code = 0) const;
    PhantomField* getField (int tag, int occurrence = 0) const noexcept;
    std::vector<PhantomField*> getFields (int tag) const;
    PhantomRecord& reset() noexcept;
    bool verify (bool throwOnError) const;

    MarcRecord materialize() const;
};

//=========================================================

class PLUSIRBIS_EXPORTS PostingParameters final
{
public:
    StringList listOfTerms;
    String database;
    String format;
    String term;
    int firstPosting { 0 };
    int numberOfPostings { 0 };
};

//=========================================================

class PLUSIRBIS_EXPORTS ProcessInfo final
{
public:
    String number;
    String ipAddress;
    String name;
    String clientId;
    String workstation;
    String started;
    String lastCommand;
    String commandNumber;
    String processId;
    String state;

    static std::vector<ProcessInfo> parse(ServerResponse &response);
};

//=========================================================

class PLUSIRBIS_EXPORTS RawRecord final
{
public:
    Mfn mfn { 0 };
    unsigned int status { 0 };
    unsigned int version { 0 };
    StringList fields;
    String database;

    friend PLUSIRBIS_EXPORTS std::wostream& operator << (std::wostream &stream, const RawRecord &record);

    String encode(const String &delimiter) const;
    void parseSingle(const StringList &lines);
    String toString() const;
};

//=========================================================

class PLUSIRBIS_EXPORTS RecordField final
{
public:
    //const static int NoTag = 0;

    int tag { 0 };
    String value;
    std::list<SubField> subfields;

    RecordField() = default;
    RecordField (int tag, const String &value = L"") : tag(tag), value(value) {}
    RecordField (const RecordField &other) = default;
    RecordField (RecordField &&other) = default;
    RecordField& operator = (const RecordField &other) = default;
    RecordField& operator = (RecordField &&other) = default;
    ~RecordField() = default;

    RecordField& add (Char code, const String &value = L"");
    RecordField& clear();
    RecordField clone() const;
    void decode (const String &line);
    bool empty() const noexcept;
    SubField* getFirstSubfield( Char code) const noexcept;
    String getFirstSubfieldValue (Char code) const noexcept;
    RecordField& removeSubfield (Char code);
    RecordField& setSubfield (Char code, const String &newValue);
    bool verify (bool throwOnError) const;
    String toString() const;

    friend PLUSIRBIS_EXPORTS std::wostream& operator << (std::wostream &stream, const RecordField &field);
};

//=========================================================

class PLUSIRBIS_EXPORTS RecordSerializer
{
    std::wiostream &stream;

public:
    explicit RecordSerializer(std::wiostream &stream);

    MarcRecord deserialize();
    void serialize(const MarcRecord &record);
};

//=========================================================

/// \brief Статус записи.
enum RecordStatus : unsigned int
{
    LogicallyDeleted = 1u,                          ///< Запись логически удалена.
    PhysicallyDeleted = 2u,                         ///< Запись физически удалена.
    Deleted = LogicallyDeleted | PhysicallyDeleted, ///< Запись удалена.
    Absent = 4u,                                    ///< Запись отсутствует.
    NonActualized = 8u,                             ///< Не актуализирована.
    Last = 32u,                                     ///< Последняя версия записи.
    Locked = 64u                                    ///< Запись заблокирована.
};

//=========================================================

/// \brief Построитель запросов.
class PLUSIRBIS_EXPORTS Search final
{
public:
    static Search all();
    Search& and_ (const String &text);
    Search& and_ (const String &text1, const String &text2);
    Search& and_ (const String &text1, const String &text2, const String &text3);
    Search& and_ (const Search &item);
    Search& and_ (const Search &item1, const Search &item2);
    Search& and_ (const Search &item1, const Search &item2, const Search &item3);
    static Search equals (const String &prefix, const String &text);
    static Search equals (const String &prefix, const String &text1, const String &text2);
    static Search equals (const String &prefix, const String &text1, const String &text2, const String &text3);
    static bool needWrap (const String &text) noexcept;
    Search& not_ (const String &text);
    Search& not_ (const Search &item);
    Search& or_ (const String &text);
    Search& or_ (const String &text1, const String &text2);
    Search& or_ (const String &text1, const String &text2, const String &text3);
    Search& or_ (const Search &item);
    Search& or_ (const Search &item1, const Search &item2);
    Search& or_ (const Search &item1, const Search &item2, const Search &item3);
    Search& sameField (const String &text);
    Search& sameRepeat (const String &text);
    String toString() const noexcept;
    static String wrap (const String &text);
    static String wrap (const Search &item);

private:
    String _buffer;
};

Search keyword (const String &value1);
Search author (const String &value1);
Search title (const String &value1);
Search publisher (const String &value1);
Search place (const String &value1);
Search subject (const String &value1);
Search language (const String &value1);
Search year (const String &value1);
Search magazine (const String &value1);
Search documentKind (const String &value1);
Search udc (const String &value1);
Search bbk (const String &value1);
Search rzn (const String &value1);
Search mhr (const String &value1);

//=========================================================

/// \brief Параметры для поиска записей.
class PLUSIRBIS_EXPORTS SearchParameters final
{
public:
    String searchExpression;        ///< Выражение для поиска по словарю.
    String database;                ///< Имя базы данных.
    Mfn firstRecord { 1 };          ///< Индекс первой требуемой записи.
    String formatSpecification;     ///< Формат для расформатирования записей.
    Mfn maxMfn { 0 };               ///< Максимальный MFN.
    Mfn minMfn { 0 };               ///< Минимальный MFN.
    Mfn numberOfRecords { 0 };      ///< Общее число требуемых записей.
    String sequentialSpecification; ///< Выражение для последовательного поиска.
    String filterSpecification;     ///< Выражение для локальной фильтрации.
};

//=========================================================

/// \brief Сценарий поиска.
class PLUSIRBIS_EXPORTS SearchScenario final
{
public:
    String name;              ///< Название поискового атрибута(автор, инвентарный номер и т. д.).
    String prefix;            ///< Префикс соответствующих терминов в словаре (может быть пустым).
    int dictionaryType { 0 }; ///< Тип словаря для соответствующего поиска.
    String menuName;          ///< Имя файла справочника.
    String oldFormat;         ///< Имя формата (без расширения).
    String correction;        ///< Способ корректировки по словарю.
    String truncation;        ///< Исходное положение переключателя "Усечение".
    String hint;              ///< Текст подсказки/предупреждения.
    String modByDicAuto;      ///< Параметр пока не задействован.
    String logic;             ///< Применимые логические операторы.
    String advance;           ///< Правила автоматического расширения поиска на основе авторитетного файла или тезауруса.
    String format;            ///< Имя формата показа документов.
};

//=========================================================

/// \brief Статистика работы ИРБИС-сервера.
class PLUSIRBIS_EXPORTS ServerStat final
{
public:
    std::vector<ClientInfo> runningClients; ///< Подключенные в данный момент клиенты.
    int clientCount { 0 }; ///< Число клиентов, подключенных в данный момент.
    int totalCommandCount { 0 }; ///< Общее количество команд, выполненных сервером с момента запуска.

    void parse (ServerResponse &response);
};

//=========================================================

/// \brief Подполе записи. Состоит из кода и значения.
class PLUSIRBIS_EXPORTS SubField final
{
public:
    /// \brief Отсутствующий код подполя.
    const static Char NoCode { L'\0' };

    /// \brief Одноисмвольный код подполя.
    Char code { L'\0' };
    String value; ///< Значение подполя (может быть пустой строкой).

    SubField() = default;
    SubField(Char code, const String &value = L"") : code(code), value(value) {}
    SubField(const SubField &other) = default;
    SubField(SubField &&other) = default;
    SubField& operator = (const SubField &other) = default;
    SubField& operator = (SubField &&other) = default;
    ~SubField() = default;

    SubField clone() const;
    void decode(const String &line);
    bool empty() const noexcept;
    bool verify(bool throwOnError) const;
    String toString() const;

    friend PLUSIRBIS_EXPORTS std::wostream& operator << (std::wostream &stream, const SubField &subfield);
};

//=========================================================

/// \brief Данные для метода printTable.
class PLUSIRBIS_EXPORTS TableDefinition final
{
public:
    String database;        ///< Имя базы данных.
    String table;           ///< Имя таблицы.
    StringList headers;     ///< Заголовки таблицы.
    String mode;            ///< Режим таблицы.
    String searchQuery;     ///< Поисковый запрос.
    int minMfn { 0 };       ///< Минимальный MFN.
    int maxMfn { 0 };       ///< Максимальный MFN.
    String sequentialQuery; ///< Запрос для последовательного поиска.
    MfnList mfnList;        ///< Список MFN, по которым строится таблица.
};

//=========================================================

/// \brief Информация о термине поискового словаря.
class PLUSIRBIS_EXPORTS TermInfo final
{
public:
    int count { 0 }; ///< Количество ссылок на данный термин.
    String text;     ///< Значение поискового термина.

    TermInfo() = default;
    TermInfo(int count_, const String text_) : count(count_), text(text_) {}
    TermInfo(const TermInfo &other) = default;
    TermInfo(TermInfo &&other) = default;
    TermInfo& operator = (const TermInfo &other) = default;
    TermInfo& operator = (TermInfo &&other) = default;
    ~TermInfo() = default;

    static std::vector<TermInfo> parse(const StringList &lines);
    String toString() const;
};

//=========================================================

/// \brief Параметры извлечения терминов из поискового словаря.
class PLUSIRBIS_EXPORTS TermParameters final
{
public:
    String database;              ///< Имя базы данных.
    Mfn numberOfTerms { 0 };      ///< Количество извлекаемых терминов.
    bool reverseOrder { false };  ///< Выдавать термины в обратном порядке?
    String startTerm;             ///< Стартовый термин.
    String format;                ///< Спецификация формата (опционально).
};

//=========================================================

/// \brief Постинг термина.
class PLUSIRBIS_EXPORTS TermPosting final
{
public:
    Mfn mfn { 0 };
    Mfn tag { 0 };
    Mfn occurrence { 0 };
    Mfn count { 0 };
    String text;

    static std::vector<TermPosting> parse(const StringList &lines);
    String toString() const;
};

//=========================================================

class PLUSIRBIS_EXPORTS TreeFile final
{
public:
    std::vector<TreeNode> roots;

    void parse(const StringList &lines);
};

//=========================================================

/// \brief Узел дерева TRE-файла.
class PLUSIRBIS_EXPORTS TreeNode final
{
public:
    String value; ///< Значение, хранящееся в узле.
    std::vector<TreeNode> children; ///< Дочерние узлы.
    int level { 0 }; ///< Уровень вложенности узла (отслеживается автоматически).

    TreeNode() = default;
    TreeNode(const String &value_) : value(value_) {}
    TreeNode(const TreeNode &other) = default;
    TreeNode(TreeNode &&other) = default;
    TreeNode& operator = (const TreeNode &other) = default;
    TreeNode& operator = (TreeNode &&other) = default;
    ~TreeNode() = default;

    TreeNode& add(const String &name);
};

//=========================================================

/// \brief Информация о зарегистрированном пользователе системы (по данным client_m.mnu).
class PLUSIRBIS_EXPORTS UserInfo final
{
public:
    String number;        ///< Номер по порядку в списке.
    String name;          ///< Логин.
    String password;      ///< Пароль.
    String cataloger;     ///< Доступность АРМ Каталогизатор.
    String reader;        ///< Доступность АРМ Читатель.
    String circulation;   ///< Доступность АРМ Книговыдача.
    String acquisitions;  ///< Доступность АРМ Комплектатор.
    String provision;     ///< Доступность АРМ Книгообеспеченность.
    String administrator; ///< Доступность АРМ Администратор.

    UserInfo() = default;
    UserInfo(const UserInfo &other) = default;
    UserInfo(UserInfo &&other) = default;
    UserInfo& operator = (const UserInfo &other) = default;
    UserInfo& operator = (UserInfo &&other) = default;
    ~UserInfo() = default;

    String toString() const;
    static std::vector<UserInfo> parse(const StringList &lines);
};

//=========================================================

/// \brief UTF-версия RecordField.
class PLUSIRBIS_EXPORTS LiteField final
{
public:
    int tag { 0 };
    std::string value;
    std::list<LiteSubField> subfields;

    LiteField() = default;
    LiteField (int tag_, const std::string &value_ = "") : tag(tag_), value(value_) {}
    LiteField (const LiteField &other) = default;
    LiteField (LiteField &&other) = default;
    LiteField& operator = (const LiteField &other) = default;
    LiteField& operator = (LiteField &&other) = default;
    ~LiteField() = default;

    LiteField& add (char code, const std::string &value = "");
    LiteField& clear();
    LiteField clone() const;
    void decode (const std::string &line);
    bool empty() const noexcept;
    LiteSubField* getFirstSubfield (char code) const noexcept;
    std::string getFirstSubfieldValue (char code) const noexcept;
    LiteField& removeSubfield (char code);
    LiteField& setSubfield (char code, const std::string &newValue);
    bool verify (bool throwOnError) const;
    std::string toString() const;

    RecordField materialize() const;
};

//=========================================================

/// \brief UTF-версия MarcRecord.
class PLUSIRBIS_EXPORTS LiteRecord final
{
public:
    Mfn mfn { 0u };                ///< MFN (порядковый номер в базе) записи.
    Flag status { 0u };            ///< Статус записи. Представляет собой набор флагов.
    unsigned int version { 0u };   ///< Номер версии записи.
    std::list<LiteField> fields;   ///< Список полей.
    std::string database;          ///< База данных.

    LiteRecord() = default;                                     ///< Конструктор по умолчанию.
    LiteRecord (const LiteRecord &other) = default;             ///< Конструктор копирования.
    LiteRecord (LiteRecord &&other) = default;                  ///< Конструктор перемещения.
    LiteRecord& operator = (const LiteRecord &other) = default; ///< Оператор копирования.
    LiteRecord& operator = (LiteRecord &&other) = default;      ///< Оператор перемещения.
    ~LiteRecord() = default;                                    ///< Деструктор.

    LiteField& add (int tag, const std::string &value);
    LiteRecord clone() const;
    void decode (const std::vector<std::string> &lines);
    bool deleted() const noexcept;
    std::string encode (const std::string &delimiter = "\x1F\x1E") const;
    std::string fm (int tag, char code = 0) const noexcept;
    std::vector<std::string> fma (int tag, char code = 0) const;
    LiteField* getField (int tag, int occurrence = 0) const noexcept;
    std::vector<LiteField*> getFields (int tag) const;
    LiteRecord& reset() noexcept;
    bool verify (bool throwOnError) const;

    MarcRecord materialize() const;
};

//=========================================================

/// \brief UTF-версия SubField.
class PLUSIRBIS_EXPORTS LiteSubField final
{
public:
    /// \brief Одноисмвольный код подполя.
    char code { '\0' };
    std::string value; ///< Значение подполя (может быть пустым).

    LiteSubField() = default;
    LiteSubField (char code_, const std::string &value_ = "") : code(code_), value(value_) {}
    LiteSubField (const LiteSubField &other) = default;
    LiteSubField (LiteSubField &&other) = default;
    LiteSubField& operator = (const LiteSubField &other) = default;
    LiteSubField& operator = (LiteSubField &&other) = default;
    ~LiteSubField() = default;

    LiteSubField clone() const;
    void decode (const std::string &line);
    bool empty() const noexcept;
    bool verify (bool throwOnError) const;
    std::string toString() const;

    SubField materialize() const;
};

//=========================================================

class PLUSIRBIS_EXPORTS VerificationException final
    : public IrbisException
{
public:
};

//=========================================================

/// \brief Информация о версии ИРБИС-сервера
class PLUSIRBIS_EXPORTS Version final
{
public:
    String organization;        ///< На какое юридическое лицо приобретен сервер.
    String version;             ///< Собственно версия сервера. Например, 64.2008.1.
    int maxClients { 0 };       ///< Максимальное количество одновременных подключений.
    int connectedClients { 0 }; ///< Текущее количество подключений.

    Version() = default;
    Version(const Version &other) = default;
    Version(Version &&other) = default;
    Version& operator = (const Version &other) = default;
    Version& operator = (Version &&other) = default;
    ~Version() = default;

    void parse(ServerResponse &response);
    String toString() const;
};

//=========================================================

/// \brief XRF-файл -- файл перекрестных ссылок.
class PLUSIRBIS_EXPORTS XrfFile64 final
{
public:

    XrfFile64 (const String &fileName, DirectAccessMode mode);
    XrfFile64 (const XrfFile64 &) = delete;
    XrfFile64 (const XrfFile64 &&) = delete;
    XrfFile64& operator = (const XrfFile64 &) = delete;
    XrfFile64& operator = (const XrfFile64 &&) = delete;
    ~XrfFile64();

    void close();
    XrfRecord64 readRecord (Mfn mfn);
    void writeRecord (Mfn mfn, XrfRecord64 record);

    static XrfFile64 create (const String &fileName);

private:

    String _fileName;
    File *_file;
    DirectAccessMode _mode;
    std::mutex _mutex;

    static Offset getOffset(Mfn mfn) noexcept;
};

//=========================================================

/// \brief Запись в XRF-файле. Содержит смещение и флаги для записи в MST-файле.
class PLUSIRBIS_EXPORTS XrfRecord64 final
{
public:
    const static int RecordSize;

    Offset offset { 0 }; ///< Смещение записи в MST-файле.
    Mfn    status { 0 }; ///< Статус записи (удалена, заблокирована и т.д.).

    bool deleted() const noexcept;
    bool locked() const noexcept;
    String toString() const;
};

//=========================================================

// Utilities

PLUSIRBIS_EXPORTS uint32_t libraryVersion() noexcept;
PLUSIRBIS_EXPORTS std::string libraryVersionString();

PLUSIRBIS_EXPORTS std::string wide2string(const String &s);
PLUSIRBIS_EXPORTS String string2wide(const std::string &s);

PLUSIRBIS_EXPORTS bool isWindows();

PLUSIRBIS_EXPORTS String describeError(int errorCode);

}

//=========================================================

#if defined(_MSC_VER)

    #pragma warning(pop)

#endif

// IRBIS_H
#endif
