#pragma once

#include "handler.hpp"
#include "objects/object.hpp"

#include "rapidjson/document.h"
#include <rttr/registration>

#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

/**
 * @brief The type of the messages that can be transfered between the server and
 * the clients
 *
 */
enum class MessageType
{
    PROCEDURE,
    CREATE,
    OBJECT,
    RETRIEVE
};

/**
 * @brief The representation of the messages that can be transfered between the
 * server and the clients.
 *
 */
struct RequestMessage
{
  public:
    MessageType type{};
    std::string object_type{};
    std::string func{};
    int obj_id{};
    int client_id{};

    /**
     * @brief Get the args as an rttr::argument
     *
     * @return std::vector<rttr::argument> It returns rttr::argument because it
     * is easier to call the rttr reflection methods.
     */
    std::vector<rttr::argument> get_args() const;

    /**
     * @brief Construct a new Request Message object.
     *
     * @param json The message requested by the client.
     */
    RequestMessage(const std::string & json);

  private:
    std::vector<rttr::variant> args;

    /**
     * @brief Populate all the members of the object using the json message.
     *
     * @param json The message requested by the client.
     */
    void build_request(const std::string & json);

    /**
     * @brief Populate the args since it comes as an array JSON format.
     *
     * @param json  The message requested by the client.
     */
    void process_array(const rapidjson::Value & json);

    /**
     * @brief Add all args to the @args member.
     *
     * @param arg The arg to be added in the @args member.
     */
    void add_to_args(rttr::variant arg);
};

/**
 * @brief The Class that will handle all the requests coming from the clients.
 * Its implementation is higly based on refecltion which is provided by the RTTR
 * library. It inherits from AbstractHandler in order to hide the implementation
 * of it from the NamedPipeConnection.
 *
 */
class ReflectionHandler : public AbstractHandler
{
  public:
    ReflectionHandler();

    /**
     * @brief It handles all the requests coming from the clients.
     *
     * @param json The message requested by the client.
     * @return std::string The response returned to the client in JSON format.
     */
    std::string handle(const std::string & json) override;

  private:
    /**
     * @brief This object holds all the repositories of all objects.
     *
     */
    std::unordered_map<std::string, std::unique_ptr<AbstractRepo>> m_repos{};

    /**
     * @brief This object holds some functions that are able to call the RTTR
     * methods by reflection.
     *
     */
    std::unordered_map<
        MessageType,
        std::function<std::string(const RequestMessage &, AbstractRepo &)>>
        m_handlers{};
    std::mutex m_mutex{};

    void init_repos();
    void init_handlers();

    /**
     * @brief Checks if the string repo is contained in @m_repos member.
     *
     * @param repo string of repo name
     */
    void check_repo(const std::string & repo);

    /**
     * @brief Checks if the string handler is contained in @m_handlers member.
     *
     * @param handler string of handler which is typed as MessageType
     */
    void check_handler(const MessageType & handler);
};

/**
 * @brief This is used to convert easily the enum MessageType to string.
 *
 */
struct MessageTypeParser
{
  public:
    typedef std::unordered_map<std::string, MessageType> MessageTypeStrEnum;
    static MessageTypeStrEnum str_to_enum;
};