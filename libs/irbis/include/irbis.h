﻿// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#pragma once

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
    #pragma warning(disable: 4251)
    #pragma warning(disable: 4275)

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
class EmbeddedField;
class Encoding;
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
class IrbisText;
class Iso2709;
class Encoding;
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
        result.success = false;
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
    Span(T *ptr_, size_t length_) noexcept : ptr(ptr_), length(length_) {}
    Span(const T *ptr_, size_t length_) noexcept : ptr(const_cast<T*>(ptr_)), length(length_) {}
    Span(const std::vector<T> &vec) : ptr(const_cast<T*>(vec.data())), length(vec.size()) {}
    Span(const std::basic_string<T> &str) : ptr(const_cast<T*>(str.data())), length(str.size()) {}

    /// \brief Спан из ASCIIZ-строки.
    /// \param ptr Указатель на начало строки.
    /// \return Полученный спан.
    static Span<T> fromString(const T *ptr) noexcept
    {
        size_t length = 0;
        const T* p = ptr;
        while (*p) {
            ++p;
            length++;
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
    size_t size() const noexcept { return this->length; }

    /// \brief Оператор индексирования.
    /// \param offset Смещение.
    /// \return Ссылка на элемент с указанным смещением.
    T& operator[](ptrdiff_t offset) const noexcept { return this->ptr[offset]; }

    /// \brief Содержит ли спан указанный элемент.
    /// \param val Искомое значение.
    /// \return `true` если содержит.
    bool contains(const T val) const noexcept
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
    Span<T>& fill(const T val) noexcept
    {
        for(size_t ofs = 0; ofs < this->length; ++ofs) {
            this->ptr[ofs] = val;
        }
        return *this;
    }

    /// \brief Поиск указанного значения в спане с начала.
    /// \param val Искомое значение.
    /// \return Индекс первого вхождения найденного элемента,
    /// либо отрицательное значение.
    ptrdiff_t indexOf(const T val) const noexcept
    {
        for (size_t i = 0; i < this->length; ++i) {
            if (this->ptr[i] == val) {
                return static_cast<ptrdiff_t >(i);
            }
        }
        return -1;
    }

    /// \brief Поиск указанного значения в спане с конца.
    /// \param val Искомое значение.
    /// \return Индекс последнего вхождения найденного элемента,
    /// либо отрицательное значение.
    ptrdiff_t lastIndexOf(const T val) const noexcept
    {
        for (ptrdiff_t i = static_cast<ptrdiff_t>(this->length - 1); i >= 0; --i) {
            if (this->ptr[i] == val) {
                return i;
            }
        }
        return -1;
    }

    /// \brief Срез спана.
    /// \param start Смещение начала спана.
    /// \param length Длина среза.
    /// \return Срез.
    ///
    /// Копирования данных не происходит.
    Span<T> slice(ptrdiff_t start, ptrdiff_t length=-1) const noexcept
    {
        if (length == -1) {
            length = this->length - start;
        }

        if (length < 0) {
            length = 0;
        }

        return Span<T>(this->ptr + start, length);
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
        for (size_t ofs = 0; ofs < this->length; ofs++) {
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
        for (size_t ofs = 0; ofs < this->length; ofs++) {
            result.push_back(this->ptr[ofs]);
        }

        return result;
    }

    /// \brief Совпадает ли начало спана с другим спаном?
    /// \param prefix Префикс.
    /// \return `true` если совпадает.
    bool startsWith(const Span<T> prefix) const noexcept
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
    bool endsWith(const Span<T> suffix) const noexcept
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
    int compare(const Span<T> other) const
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
    std::vector<Span<T>> split(T delimiter) const
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
    std::vector<Span<T>> split(T delimiter, int nelem) const
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

PLUSIRBIS_EXPORTS bool sameString(CharSpan first, CharSpan second);
PLUSIRBIS_EXPORTS bool sameString(WideSpan first, WideSpan second);
PLUSIRBIS_EXPORTS CharSpan trimStart(CharSpan text);
PLUSIRBIS_EXPORTS WideSpan trimStart(WideSpan text);
PLUSIRBIS_EXPORTS CharSpan trimEnd(CharSpan text);
PLUSIRBIS_EXPORTS WideSpan trimEnd(WideSpan text);
PLUSIRBIS_EXPORTS CharSpan trim(CharSpan text);
PLUSIRBIS_EXPORTS WideSpan trim(WideSpan text);
PLUSIRBIS_EXPORTS CharSpan toupper(CharSpan text);
PLUSIRBIS_EXPORTS WideSpan toupper(WideSpan text);
PLUSIRBIS_EXPORTS CharSpan tolower(CharSpan text);
PLUSIRBIS_EXPORTS WideSpan tolower(WideSpan text);

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

/// \brief Простая обертка над файлом.
class PLUSIRBIS_EXPORTS File final
{
public:

    File(FILE *stream);
    File(const char *name, const char *mode);
    File(const std::string &name, const std::string &mode);
    File(const String &name, const String &mode);
    File(const File &other) = delete;
    File(File &&other) = delete;
    File& operator = (const File &other) = delete;
    File& operator = (File &&other) = delete;
    ~File();

    std::size_t read(Byte *buffer, std::size_t count);
    int64_t seek(int64_t position);
    int64_t tell();
    std::size_t write(const Byte *buffer, std::size_t count);

private:

    FILE *_stream;
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

class PLUSIRBIS_EXPORTS ChunkedBuffer final
{
private:
    MemoryChunk *_first, *_current, *_last;
    size_t _chunkSize, _position, _read;

    bool _advance();
    void _appendChunk();

public:
    const static size_t DefaultChunkSize;

    explicit ChunkedBuffer(size_t chunkSize = DefaultChunkSize);
    ChunkedBuffer(const ChunkedBuffer &) = delete;
    ChunkedBuffer(ChunkedBuffer &&) = delete;
    ChunkedBuffer operator = (const ChunkedBuffer &) = delete;
    ChunkedBuffer operator = (ChunkedBuffer &&) = delete;
    ~ChunkedBuffer();

    bool eof() const;
    int peek();
    size_t read(char *buffer, size_t offset, size_t count);
    int readByte();
    //std::wstring readLine(QTextCodec *codec);
    void rewind();
    void write(const char *buffer, size_t offset, size_t count);
    //void write(const std::wstring &text, QTextCodec *codec);
    void writeByte(char value);
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

/// \brief Абстрактный клиентский сокет.
///
/// Наследники обязательно должны переопределить методы send и receive.
/// Объекты данного класса неперемещаемые.
class PLUSIRBIS_EXPORTS ClientSocket // abstract
{
public:
    String host { L"localhost" }; ///< Адрес сервера в виде строки.
    short port { 6666 }; ///< Номер порта сервера.

    ClientSocket() = default;
    ClientSocket(ClientSocket &) = delete;
    ClientSocket(ClientSocket &&) = delete;
    ClientSocket& operator = (ClientSocket &) = delete;
    ClientSocket& operator = (ClientSocket &&) = delete;
    virtual ~ClientSocket() = default;

    virtual void open();
    virtual void close();

    /// \brief Отсылка данных на сервер.
    /// \param buffer Указатель на буфер с данными.
    /// \param size Размер данных в байтах.
    virtual void send(const Byte *buffer, size_t size) = 0;

    /// \brief Получение отвера от сервера (частями).
    /// \param buffer Буфер для размещения полученного ответа.
    /// \param size Размер буфера в байтах.
    /// \return Количество полученных данных в байтах.
    /// Отрицательное число означает ошибку.
    /// 0 означает, что сервер закончил передачу данных.
    /// Положительное число означает, что приём продолжается,
    /// и вызов нужно будет повторить для получения следующей порции.
    virtual size_t receive(Byte *buffer, size_t size) = 0;
};

//=========================================================

/// \brief Клиентский запрос.
///
/// Объекты данного класса неперемещаемые.
class PLUSIRBIS_EXPORTS ClientQuery final
{
    std::vector<Byte> _content;

    void _write(const Byte *bytes, size_t size);
    void _write(Byte byte);

public:
    ClientQuery(const Connection &connection, const std::string &commandCode);
    ClientQuery(ClientQuery &) = delete;
    ClientQuery(ClientQuery &&) = delete;
    ClientQuery& operator = (ClientQuery &) = delete;
    ClientQuery& operator = (ClientQuery &&) = delete;
    ~ClientQuery() = default;

    ClientQuery& add(int value);
    ClientQuery& add(const FileSpecification &specification);
    ClientQuery& add(const MarcRecord &record, const std::wstring &delimiter);
    ClientQuery& addAnsi(const std::string &text);
    ClientQuery& addAnsi(const String &text);
    bool addFormat(const String &format);
    ClientQuery& addUtf(const String &text);
    void dump(std::ostream &stream) const;
    Bytes encode() const;
    ClientQuery& newLine();
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

    bool actualizeDatabase(const String &databaseName);
    bool actualizeRecord(const String &databaseName, int mfn);
    bool connect();
    // std::future<bool> connectAsync();
    bool createDatabase(const String &databaseName, const String &description, bool readerAccess);
    bool createDictionary(const String &databaseName);
    bool deleteDatabase(const String &databaseName);
    bool deleteRecord(int mfn);
    bool connected() const noexcept { return this->_connected; }
    void disconnect();
    // std::future<void> disconnectAsync();
    bool execute(ClientQuery &query);
    // std::future<bool> executeAsync(ClientQuery &query);
    String formatRecord(const String &format, Mfn mfn);
    String formatRecord(const String &format, const MarcRecord &record);
    DatabaseInfo getDatabaseInfo(const String &databaseName);
    Mfn getMaxMfn(const String &databaseName);
    GblResult globalCorrection(const GblSettings &settings);
    ServerStat getServerStat();
    Version getServerVersion();
    std::vector<UserInfo> getUserList();
    std::vector<DatabaseInfo> listDatabases(const IniFile &iniFile, const String &defaultFileName);
    std::vector<DatabaseInfo> listDatabases(const FileSpecification &specification);
    StringList listFiles(const FileSpecification &specification);
    StringList listFiles(const std::vector<FileSpecification> &specifications);
    std::vector<ProcessInfo> listProcesses();
    StringList listTerms(const String &prefix);
    bool noOp();
    // std::future<bool> noOpAsync();
    void parseConnectionString(const String &connectionString);
    String popDatabase();
    String printTable(const TableDefinition &definition);
    String pushDatabase(const String &newDatabase);
    Bytes readBinaryFile(const FileSpecification &specification);
    IniFile readIniFile(const FileSpecification &specification);
    MenuFile readMenuFile(const FileSpecification &specification);
    std::vector<TermPosting> readPostings(const PostingParameters &parameters);
    RawRecord readRawRecord(Mfn mfn);
    MarcRecord readRecord(Mfn mfn);
    MarcRecord readRecord(const String &databaseName, Mfn mfn);
    MarcRecord readRecord(const String &databaseName, Mfn mfn, int version);
    std::vector<SearchScenario> readSearchScenario(const FileSpecification &specification);
    std::vector<TermInfo> readTerms(const String &startTerm, int numberOfTerms);
    std::vector<TermInfo> readTerms(const TermParameters &parameters);
    String readTextFile(const FileSpecification &specification);
    StringList readTextFiles(std::vector<FileSpecification> specifications);
    StringList  readTextLines(const FileSpecification &specification);
    bool reloadDictionary(const String &databaseName);
    bool reloadMasterFile(const String &databaseName);
    bool restartServer();
    String requireTextFile(const FileSpecification &specification);
    MfnList search(const Search &search);
    MfnList search(const String &expression);
    MfnList search(const SearchParameters &parameters);
    String toConnectionString() const;
    bool truncateDatabase(const String &databaseName);
    bool unlockDatabase(const String &databaseName);
    bool unlockRecords(const String &databaseName, const MfnList &mfnList);
    bool updateIniFile(const StringList &lines);
    bool updateUserList(const std::vector<UserInfo> &users);
    int writeRawRecord(RawRecord &record, bool lockFlag, bool actualize, bool dontParseResponse);
    int writeRecord(MarcRecord &record, bool lockFlag, bool actualize, bool dontParseResponse);
    bool writeRecords(std::vector<MarcRecord*> &records, bool lockFlag, bool actualize, bool dontParseResponse);
    void writeTextFile(const FileSpecification &specification);
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

class PLUSIRBIS_EXPORTS Encoding // abstract
{
    static Encoding *_ansi;
    static Encoding *_utf;

public:
    Encoding() = default;
    virtual ~Encoding() = default;

    virtual std::vector<Byte> fromUnicode(const String &text) const = 0;
    virtual String toUnicode(const Byte *bytes, size_t count) const = 0;

    static Encoding* ansi();
    static String fromAnsi(const Byte *bytes, size_t count);
    static String fromUtf(const Byte *bytes, size_t count);
    static Bytes toAnsi(const String &text);
    static Bytes toUtf(const String &text);
    static Encoding* utf();
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

/// \brief Утилиты для ввода-вывода.
class PLUSIRBIS_EXPORTS IO final
{
public:

    IO() = delete;

    static bool readInt32 (FILE* file, uint32_t *value);
    static bool readInt64 (FILE* file, uint64_t *value);
    static bool writeInt32 (FILE* file, uint32_t value);
    static bool writeInt64 (FILE* file, uint64_t value);
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

class PLUSIRBIS_EXPORTS IrbisText final
{
public:
    const static char CrLf[];
    const static char Lf[];
    const static String IrbisDelimiter;
    const static String ShortDelimiter;
    const static String MsDosDelimiter;
    const static String UnixDelimiter;
    const static String SearchDelimiter;

    static String fromIrbisToDos(String &text);
    static String fromIrbisToUnix(String &text);
    static String fromDosToIrbis(String &text);
    static String fromDosToUnix(String &text);
    static StringList fromFullDelimiter (const String &text);
    static StringList fromShortDelimiter (const String &text);
    static String readAllAnsi(const String &filename);
    static String readAllUtf(const String &filename);
    static StringList readAnsiLines(const String &filename);
    static StringList readUtfLines(const String &filename);
};

//=========================================================

class PLUSIRBIS_EXPORTS Iso2709 final
{
public:
    static const int MarkerLength = 24;
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

    RecordField& add(int tag, const String &value);
    MarcRecord clone() const;
    void decode(const StringList &lines);
    bool deleted() const noexcept;
    String encode(const String &delimiter) const;
    String fm(int tag, Char code) const noexcept;
    StringList fma(int tag, Char code) const;
    RecordField* getField(int tag, int occurrence) const noexcept;
    std::vector<RecordField*> getFields(int tag) const;
    MarcRecord& reset() noexcept;
    bool verify(bool throwOnError) const;

    friend PLUSIRBIS_EXPORTS std::wostream& operator << (std::wostream &stream, const MarcRecord &record);
};

//=========================================================

class PLUSIRBIS_EXPORTS MemoryChunk final
{
public:
    char *data;
    MemoryChunk *next;

    explicit MemoryChunk(size_t size);
    MemoryChunk(const MemoryChunk &other) = delete;
    MemoryChunk(MemoryChunk &&other) = delete;
    MemoryChunk& operator = (const MemoryChunk &other) = delete;
    MemoryChunk& operator = (MemoryChunk &&other) = delete;
    ~MemoryChunk();
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
    RecordField(int tag, const String &value = L"") : tag(tag), value(value) {}
    RecordField(const RecordField &other) = default;
    RecordField(RecordField &&other) = default;
    RecordField& operator = (const RecordField &other) = default;
    RecordField& operator = (RecordField &&other) = default;
    ~RecordField() = default;

    RecordField& add(Char code, const String &value = L"");
    RecordField& clear();
    RecordField clone() const;
    void decode(const String &line);
    bool empty() const noexcept;
    SubField* getFirstSubfield(Char code) const noexcept;
    String getFirstSubfieldValue(Char code) const noexcept;
    bool verify(bool throwOnError) const;
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
    LogicallyDeleted = 1u, ///< Запись логически удалена.
    PhysicallyDeleted = 2u, ///< Запись физически удалена.
    Deleted = LogicallyDeleted | PhysicallyDeleted, ///< Запись удалена.
    Absent = 4u, ///< Запись отсутствует.
    NonActualized = 8u, ///< Не актуализирована.
    Last = 32u, ///< Последняя версия записи.
    Locked = 64u ///< Запись заблокирована.
};

//=========================================================

/// \brief Построитель запросов.
class PLUSIRBIS_EXPORTS Search final
{
public:
    static Search all();
    Search& and_(const String &text);
    Search& and_(const String &text1, const String &text2);
    Search& and_(const String &text1, const String &text2, const String &text3);
    Search& and_(const Search &item);
    Search& and_(const Search &item1, const Search &item2);
    Search& and_(const Search &item1, const Search &item2, const Search &item3);
    static Search equals(const String &prefix, const String &text);
    static Search equals(const String &prefix, const String &text1, const String &text2);
    static Search equals(const String &prefix, const String &text1, const String &text2, const String &text3);
    static bool needWrap(const String &text) noexcept;
    Search& not_(const String &text);
    Search& not_(const Search &item);
    Search& or_(const String &text);
    Search& or_(const String &text1, const String &text2);
    Search& or_(const String &text1, const String &text2, const String &text3);
    Search& or_(const Search &item);
    Search& or_(const Search &item1, const Search &item2);
    Search& or_(const Search &item1, const Search &item2, const Search &item3);
    Search& sameField(const String &text);
    Search& sameRepeat(const String &text);
    String toString() const noexcept;
    static String wrap(const String &text);
    static String wrap(const Search &item);

private:
    String _buffer;
};

Search keyword(const String &value1);
Search author(const String &value1);
Search title(const String &value1);
Search publisher(const String &value1);
Search place(const String &value1);
Search subject(const String &value1);
Search language(const String &value1);
Search year(const String &value1);
Search magazine(const String &value1);
Search documentKind(const String &value1);
Search udc(const String &value1);
Search bbk(const String &value1);
Search rzn(const String &value1);
Search mhr(const String &value1);

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

/// \brief Ответ сервера на клиентский запрос.
class PLUSIRBIS_EXPORTS ServerResponse final
{
public:
    String command;       ///< Код команды (дублирует клиентский запрос).
    int clientId;         ///< Идентификатор клиента (дублирует клиентский запрос).
    int queryId;          ///< Номер команды (дублирует клиентский запрос).
    int answerSize;       ///< Размер ответа сервера в байтах (в некоторых сценариях отсутствует).
    int returnCode;       ///< Код возврата (бывает не у всех ответов).
    String serverVersion; ///< Версия сервера (в некоторых сценариях отсутствует).

    ServerResponse(Connection &connection, ClientQuery &query);
    ServerResponse(ServerResponse &) = delete;
    ServerResponse(ServerResponse &&) = delete;
    ServerResponse& operator = (ServerResponse &) = delete;
    ServerResponse& operator = (ServerResponse &&) = delete;
    ~ServerResponse() = default;

    bool eot() const;
    bool success() const;

    bool checkReturnCode();
    bool checkReturnCode(int nargs, ...);
    std::string getLine();
    std::string getRemaining();
    int getReturnCode();
    String readAnsi();
    int readInteger();
    StringList readRemainingAnsiLines();
    String readRemainingAnsiText();
    StringList readRemainingUtfLines();
    String readRemainingUtfText();
    String readUtf();

private:
    Connection *_connection;
    bool _success;
    size_t _position;
    Bytes _content;
    void _write(const Byte *bytes, size_t size);
};

//=========================================================

/// \brief Статистика работы ИРБИС-сервера.
class PLUSIRBIS_EXPORTS ServerStat final
{
public:
    std::vector<ClientInfo> runningClients; ///< Подключенные в данный момент клиенты.
    int clientCount { 0 }; ///< Число клиентов, подключенных в данный момент.
    int totalCommandCount { 0 }; ///< Общее количество команд, выполненных сервером с момента запуска.

    void parse(ServerResponse &response);
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

class PLUSIRBIS_EXPORTS Tcp4Socket final
    : public ClientSocket
{
    void *_impl;

public:
    explicit Tcp4Socket(const std::wstring& host=L"localhost", short port=6666);
    Tcp4Socket(const Tcp4Socket &) = delete;
    Tcp4Socket(Tcp4Socket &&) = delete;
    Tcp4Socket& operator = (const Tcp4Socket &) = delete;
    Tcp4Socket& operator = (Tcp4Socket &&) = delete;
    ~Tcp4Socket() override;

    void open() override;
    void close() override;
    void send(const Byte *buffer, size_t size) override;
    size_t receive(Byte *buffer, size_t size) override;
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

/// \brief Навигация по тексту.
class PLUSIRBIS_EXPORTS TextNavigator final
{
public:
    const static Char EOT;

    TextNavigator(const Char* text, std::size_t length) noexcept;
    TextNavigator(WideSpan text) noexcept;
    TextNavigator(const String &text) noexcept;
    TextNavigator(const TextNavigator &other) noexcept;
    TextNavigator(TextNavigator&&) = delete;
    TextNavigator& operator = (const TextNavigator &) = delete;
    TextNavigator& operator = (TextNavigator &&) = delete;
    ~TextNavigator() = default;

    std::size_t column()   const noexcept;
    std::size_t line()     const noexcept;
    std::size_t length()   const noexcept;
    std::size_t position() const noexcept;
    Char* begin()          const noexcept;
    const Char* cbegin()   const noexcept;
    Char* current()        const noexcept;
    const Char* ccurrent() const noexcept;
    Char* end()            const noexcept;
    const Char* cend()     const noexcept;
    bool eot() const noexcept;

    Char at(std::size_t position) const noexcept;
    Char front() const noexcept;
    Char back() const noexcept;
    Char lookAhead(std::ptrdiff_t distance = 1) const noexcept;
    Char lookBehind(std::ptrdiff_t distance = 1) const noexcept;
    TextNavigator& move(std::ptrdiff_t distance) noexcept;
    Char peekChar() const noexcept;
    Char readChar() noexcept;
    WideSpan peekString(std::size_t length) const noexcept;
    WideSpan peekTo(Char stopChar) const noexcept;
    WideSpan peekUntil(Char stopChar) const noexcept;
    WideSpan readLine() noexcept;
    bool isControl() const noexcept;
    bool isDigit() const noexcept;
    bool isLetter() const noexcept;
    bool isWhitespace() const noexcept;
    WideSpan readInteger() noexcept;
    WideSpan readString(std::size_t length) noexcept;
    WideSpan readTo(Char stopChar) noexcept;
    WideSpan readUntil(Char stopChar) noexcept;
    WideSpan readWhile(Char goodChar) noexcept;
    WideSpan readWord() noexcept;
    WideSpan recentText(std::ptrdiff_t length) const noexcept;
    WideSpan remainingText() const noexcept;
    TextNavigator& skipWhitespace() noexcept;
    TextNavigator& skipPunctuation() noexcept;
    WideSpan substr(std::ptrdiff_t offset, std::size_t length) const noexcept;

private:
    std::size_t _column, _length, _line, _position;
    const Char *_text;
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

    XrfFile64(const String &fileName, DirectAccessMode mode);
    XrfFile64(const XrfFile64 &) = delete;
    XrfFile64(const XrfFile64 &&) = delete;
    XrfFile64& operator = (const XrfFile64 &) = delete;
    XrfFile64& operator = (const XrfFile64 &&) = delete;
    ~XrfFile64();

    XrfRecord64 readRecord(Mfn mfn);

private:

    String _fileName;
    FILE *_file;
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

    Mfn    mfn    { 0 }; ///< MFN записи.
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

PLUSIRBIS_EXPORTS bool sameChar(Char first, Char second) noexcept;
PLUSIRBIS_EXPORTS bool sameString(const String &first, const String &second) noexcept;

PLUSIRBIS_EXPORTS String toLower(String &text) noexcept;
PLUSIRBIS_EXPORTS String toUpper(String &text) noexcept ;

PLUSIRBIS_EXPORTS bool contains(const String &text, const String &fragment);
PLUSIRBIS_EXPORTS bool contains(const String &text, Char c);

PLUSIRBIS_EXPORTS std::string replace(const std::string &text, const std::string &from, const std::string &to);
PLUSIRBIS_EXPORTS String replace(const String &text, const String &from, const String &to);

PLUSIRBIS_EXPORTS String trimStart(const String &text);
PLUSIRBIS_EXPORTS String trimEnd(const String &text);
PLUSIRBIS_EXPORTS String trim(const String &text);

PLUSIRBIS_EXPORTS String describeError(int errorCode);

PLUSIRBIS_EXPORTS int fastParse32(const String &text);
PLUSIRBIS_EXPORTS int fastParse32(CharSpan text);
PLUSIRBIS_EXPORTS int fastParse32(WideSpan text);
PLUSIRBIS_EXPORTS int fastParse32(const Char *text);
PLUSIRBIS_EXPORTS int fastParse32(const Char *text, size_t length);
PLUSIRBIS_EXPORTS int fastParse32(const std::string &text);
PLUSIRBIS_EXPORTS int fastParse32(const char *text);
PLUSIRBIS_EXPORTS int fastParse32(const char *text, size_t length);

PLUSIRBIS_EXPORTS unsigned int fastParseUnsigned32(const String &text);
PLUSIRBIS_EXPORTS unsigned int fastParseUnsigned32(const Char *text);
PLUSIRBIS_EXPORTS unsigned int fastParseUnsigned32(const Char *text, size_t length);
PLUSIRBIS_EXPORTS unsigned int fastParseUnsigned32(const std::string &text);
PLUSIRBIS_EXPORTS unsigned int fastParseUnsigned32(const char *text);
PLUSIRBIS_EXPORTS unsigned int fastParseUnsigned32(const char *text, size_t length);

PLUSIRBIS_EXPORTS const std::string& iif(const std::string &s1, const std::string &s2);
PLUSIRBIS_EXPORTS const String& iif(const String &s1, const String &s2);
PLUSIRBIS_EXPORTS const std::string& iif(const std::string &s1, const std::string &s2, const std::string &s3);
PLUSIRBIS_EXPORTS const String& iif(const String& s1, const String &s2, const String &s3);

PLUSIRBIS_EXPORTS std::wstring safeAt(const StringList &list, size_t index);

PLUSIRBIS_EXPORTS std::vector<std::string> split(const std::string &text, char delimiter);
PLUSIRBIS_EXPORTS StringList split(const String &text, Char delimiter);
PLUSIRBIS_EXPORTS std::vector<std::string> split(const std::string &text, const std::string &delimiter);
PLUSIRBIS_EXPORTS StringList split(const String &text, const String &delimiter);
PLUSIRBIS_EXPORTS StringList maxSplit(const String &text, Char separator, int count);

PLUSIRBIS_EXPORTS String cp866_to_unicode(const std::string &text);
PLUSIRBIS_EXPORTS String cp1251_to_unicode(const std::string &text);
PLUSIRBIS_EXPORTS String koi8r_to_unicode(const std::string &text);

PLUSIRBIS_EXPORTS std::string unicode_to_cp866(const String &text);
PLUSIRBIS_EXPORTS std::string unicode_to_cp1251(const String &text);
PLUSIRBIS_EXPORTS void unicode_to_cp1251(Byte *dst, const Char *src, size_t size);
PLUSIRBIS_EXPORTS std::string unicode_to_koi8r(const String &text);

PLUSIRBIS_EXPORTS std::string narrow(const String &wide, const std::locale &loc);
PLUSIRBIS_EXPORTS String widen(const std::string &str, const std::locale &loc);
PLUSIRBIS_EXPORTS String new_cp1251_to_unicode(const std::string &text);
PLUSIRBIS_EXPORTS std::string new_unicode_to_cp1251(const String &text);
PLUSIRBIS_EXPORTS std::string new_toUtf(const String &text);
PLUSIRBIS_EXPORTS String new_fromUtf(const std::string &text);

PLUSIRBIS_EXPORTS Byte* toUtf(Byte *dst, const Char *src, size_t length);
PLUSIRBIS_EXPORTS Char* fromUtf(Char *dst, const Byte *src, size_t length);
PLUSIRBIS_EXPORTS size_t countUtf(const Char *src, size_t length);
PLUSIRBIS_EXPORTS size_t countUtf(const Byte *src, size_t length);
PLUSIRBIS_EXPORTS const Byte* fromUtf(const Byte *src, size_t &size, Byte stop, String &result);
PLUSIRBIS_EXPORTS Byte* toUtf(Byte *dst, const String &text);
PLUSIRBIS_EXPORTS String fromUtf(const std::string &text);
PLUSIRBIS_EXPORTS std::string toUtf(const String &text);

PLUSIRBIS_EXPORTS String removeComments(const String &text);
PLUSIRBIS_EXPORTS String prepareFormat(const String &text);

}

//=========================================================

#if defined(_MSC_VER)

    #pragma warning(pop)

#endif

