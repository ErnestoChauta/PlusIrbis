// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "irbis.h"

#if defined(_MSC_VER)
#pragma warning(disable: 4068)
#endif

#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "cert-err58-cpp"

///
/// \class irbis::GblStatement Файл задания на пакетную корректировку
/// представляет собой текстовый файл с расширением GBL,
/// содержит последовательность операторов корректировки,
/// каждый из которых состоит из нескольких строк.
///
/// Операторы выполняются в порядке их следования,
/// причем каждый оператор использует значения полей
/// и/или подполей измененных, возможно, предыдущими операторами.
///
/// Первая строка файла задания – это число, задающее
/// количество параметров, используемых в операторах корректировки.
///
/// Последующие пары строк, число пар должно быть равно
/// количеству параметров, используются программой
/// глобальной корректировки.
///
/// Первая строка пары - значение параметра или пусто,
/// если пользователю предлагается задать его значение
/// перед выполнением корректировки. В этой строке можно
/// задать имя файла меню (с расширением MNU)
/// или имя рабочего листа подполей (с расширением Wss),
/// которые будут поданы для выбора значения параметра.
/// Вторая строка пары – наименование параметра,
/// которое появится в названии столбца, задающего параметр.
///
/// Группы строк, описывающих операторы корректировки
/// Далее следуют группы строк, описывающих операторы корректировки.
///
/// Первая строка каждой группы – это имя оператора,
/// которое может иметь одно из значений: ADD, REP, CHA, CHAC,
/// DEL, DELR, UNDEL, CORREC, NEWMFN, END, IF, FI, ALL,
/// EMPTY, REPEAT, UNTIL, //.
///
/// Количество строк, описывающих оператор, зависит от его назначения.
/// Операторы ADD, REP, CHA, CHAC, DEL описываются пятью строками,
/// в которых задаются  следующие элементы:
/// ИМЯ ОПЕРАТОРА
/// МЕТКА ПОЛЯ/ПОДПОЛЯ: число, обозначающее метку поля,
/// + разделитель подполя + обозначение подполя.
/// Разделитель подполя с обозначением могут отсутствовать
/// ПОВТОРЕНИЕ ПОЛЯ
/// * - если корректируются все повторения
/// F - если используется корректировка по формату
/// N (число) – если корректируется N-ое повторение поля
/// L – если корректируется последнее повторение поля
/// L-N ( число) – если корректируется N-ое с конца повторение поля
/// ФОРМАТ 1 – формат
/// ФОРМАТ 2 - формат
///
/// Для каждого конкретного оператора элементы ФОРМАТ 1
/// и ФОРМАТ 2 имеют свое назначение. Некоторые из элементов
/// могут не задаваться, когда в конкретной конфигурации
/// они не имеют смысла. Тогда соответствующая строка
/// в задании должна быть пустой или занята символом-заполнителем,
/// как это формирует программа глобальной корректировки.
///
/// Содержимое строк остальных операторов определяется
/// их назначением и представлено в описании операторов.
///
/// \class irbis::GblParameter Первая строка файла задания – это число, задающее
/// количество параметров, используемых в операторах корректировки.
///
/// Последующие пары строк, число пар должно быть равно
/// количеству параметров, используются программой
/// глобальной корректировки.
///
/// Первая строка пары - значение параметра или пусто,
/// если пользователю предлагается задать его значение
/// перед выполнением корректировки. В этой строке можно
/// задать имя файла меню (с расширением MNU)
/// или имя рабочего листа подполей (с расширением Wss),
/// которые будут поданы для выбора значения параметра.
///
/// Вторая строка пары – наименование параметра,
/// которое появится в названии столбца, задающего параметр.
///
/// \var irbis::GblParameter::title Наименование параметра,
/// которое появится в названии столбца, задающего параметр.
///
/// \var irbis::GblParameter::value Значение параметра или пусто, если пользователю
/// предлагается задать его значение перед выполнением
/// корректировки. В этой строке можно задать имя файла
/// меню (с расширением MNU) или имя рабочего листа подполей
/// (с расширением Wss), которые будут поданы для выбора
/// значения параметра.
///

namespace irbis {

/// \brief Разбор строк.
/// \param lines Строки.
void GblResult::parse (const StringList& lines)
{
    for (const auto &line : lines) {
        if (!line.empty()) {
            throw NotImplementedException();
        }
    }
}

//=========================================================

}
