// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "irbis.h"
#include "irbis_private.h"

#include <random>

#if defined(_MSC_VER)
#pragma warning(disable: 4068)
#endif

#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

namespace irbis {

/// \brief Единственный конструктор для данного класса.
ConnectionBase::ConnectionBase()
        : _connected(false),
          host(L"127.0.0.1"),
          port(6666),
          username(L""),
          password(L""),
          database(L"IBIS"),
          workstation(L"C"),
          clientId(0),
          queryId(0),
          lastError(0) {
    this->socket = new Tcp4Socket();
}

/// \brief Деструктор для данного класса.
///
/// Если необходимо, выполняет отключение от сервера.
ConnectionBase::~ConnectionBase() {
    this->disconnect();
    delete this->socket;
}

bool ConnectionBase::_checkConnection()
{
    if (!this->connected()) {
        this->lastError = -100003;
        return false;
    }
    // this->lastError = 0;
    return true;
}

/// \brief Подключение к серверу.
/// \return Признак успешности выполнения операции.
/// \warning Подключение некоторыми типами клиентов увеличивает на сервере счетчик использованных лицензий!
///
/// Повторные попытки подключения с помощью того же объекта Connection игнорируются.
/// При этом возвращается true.
bool ConnectionBase::connect()
{
    if (this->connected()) {
        return true;
    }

    this->lastError = 0;

    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dis(100000, 900000);

    AGAIN: this->clientId = dis(gen);
    this->queryId = 1;
    ClientQuery query(*this, "A");
    query.addAnsi(this->username).newLine()
            .addAnsi(this->password);

    try {
        ServerResponse response(*this, query);
        if (!response.success()) {
            return false;
        }

        response.getReturnCode();
        if (response.returnCode == -3337) {
            // клиент с данным идентификатором уже зарегистрирован
            goto AGAIN;
        }

        if (response.returnCode < 0) {
            return false;
        }

        this->_connected = true;
        this->serverVersion = response.serverVersion;
        this->interval = response.readInteger();
        const auto lines = response.readRemainingAnsiLines();
        this->iniFile.parse(lines);
    }
    catch (...) {
        this->lastError = -100002;
        return false;
    }

    return true;
}

/// \brief Отключение от сервера.
///
/// Повторные попытки отключения уже отключенного экземпляра клиента игнорируются.
/// Если при подключении был увеличен счётчик использованных лицензий, он соответственно уменьшается.
void ConnectionBase::disconnect()
{
    if (!this->connected()) {
        return;
    }

    ClientQuery query(*this, "B");
    query.addAnsi(this->username).newLine();
    this->execute(query);
    _connected = false;
}

/// \brief Результат исполнения запроса на сервере.
/// \param query Полностью сформированный клиентский запрос.
/// \return Признак успешного выполнения запроса.
/// \warning Должно быть установлено подключение к серверу!
///
/// Если метод вернул false, конкретный код ошибки находится в lastError.
bool ConnectionBase::execute(ClientQuery &query)
{
    if (!this->_checkConnection()) {
        return false;
    }

    bool result = false;

    try {
        ServerResponse response(*this, query);
        result = response.checkReturnCode();
    }
    catch (...) {
        // Do nothing
    }

    return result;
}

/// \brief Получение максимального MFN для базы данных с указанным именем.
/// \param databaseName Имя базы данных.
/// \return Максимальный MFN. 0 или отрицательное число означают ошибку.
Mfn ConnectionBase::getMaxMfn (const String &databaseName)
{
    if (!this->_checkConnection()) {
        return 0;
    }

    ClientQuery query (*this, "O");
    query.addAnsi (databaseName);
    ServerResponse response (*this, query);
    if (!response.checkReturnCode()) {
        return 0;
    }

    const auto result = response.returnCode;
    return result;
}

/// \brief Пустая операция (не требует от сервера никаких действий).
/// \return Признак успешности выполнения операции.
///
/// Используется для подтверждения подключения клиента
/// при длительном бездействии пользователя.
bool ConnectionBase::noOp() {
    if (!this->_checkConnection()) {
        return false;
    }

    ClientQuery query(*this, "N");
    return this->execute(query);
}

/// \brief Разбор строки подключения.
/// \param connectionString Строка подключения.
/// \throw IrbisException Ошибка в структуре строки подключения.
void ConnectionBase::parseConnectionString (const String &connectionString) {
    const auto items = split (connectionString, L";");
    for (auto &item : items) {
        auto parts = maxSplit(item, L'=', 2);
        if (parts.size() != 2) {
            throw IrbisException();
        }
        auto name = toLower (parts[0]);
        const auto value = parts[1];
        if (name.empty() || value.empty()) {
            throw IrbisException();
        }
        if (name == L"host" || name == L"server" || name == L"address") {
            this->host = value;
        } else if (name == L"port") {
            this->port = (short) fastParse32 (value);
        } else if (name == L"user" || name == L"username"
                   || name == L"name" || name == L"login") {
            this->username = value;
        } else if (name == L"pwd" || name == L"password") {
            this->password = value;
        } else if (name == L"db" || name == L"catalog" || name == L"database") {
            this->database = value;
        } else if (name == L"arm" || name == L"workstation") {
            this->workstation = value.at (0);
        } else {
            throw IrbisException();
        }
    }
}

/// \brief Разбор строки подключения.
/// \param connectionString Строка подключения.
/// \throw IrbisException Ошибка в структуре строки подключения.
void ConnectionBase::parseConnectionString (const std::string &connectionString) {
    const auto items = split (connectionString, ";");
    for (auto &item : items) {
        auto parts = maxSplit (item, '=', 2);
        if (parts.size() != 2) {
            throw IrbisException();
        }
        auto name = toLower (parts[0]);
        const auto value = parts[1];
        if (name.empty() || value.empty()) {
            throw IrbisException();
        }
        if (name == "host" || name == "server" || name == "address") {
            this->host = fromUtf (value);
        } else if (name == "port") {
            this->port = (short) fastParse32 (value);
        } else if (name == "user" || name == "username"
                   || name == "name" || name == "login") {
            this->username = fromUtf (value);
        } else if (name == "pwd" || name == "password") {
            this->password = fromUtf (value);
        } else if (name == "db" || name == "catalog" || name == "database") {
            this->database = fromUtf (value);
        } else if (name == "arm" || name == "workstation") {
            this->workstation = value.at (0);
        } else {
            throw IrbisException();
        }
    }
}

/// \brief Возврат к предыдущей базе данных.
/// \return Имя базы данных, использовавшейся до возврата.
String ConnectionBase::popDatabase()
{
    const auto result = this->database;
    this->database = _databaseStack.back();
    _databaseStack.pop_back();

    return result;
}

/// \brief Запоминание имени текущей базы данных для последующего возврата к ней.
/// \param newDatabase Имя базы данных, устанавливаемой в качестве текущей.
/// \return Имя базы данных, использовавшейся в качестве текущей.
String ConnectionBase::pushDatabase (const String &newDatabase)
{
    const auto result = this->database;
    _databaseStack.push_back(newDatabase);
    this->database = newDatabase;

    return result;
}

String ConnectionBase::toConnectionString() const
{
    return String(L"host=") + this->host
           + L";port=" + std::to_wstring(this->port)
           + L";username=" + this->username
           + L";password=" + this->password
           + L";database" + this->database
           + L";arm=" + this->workstation
           + L";";
}

}