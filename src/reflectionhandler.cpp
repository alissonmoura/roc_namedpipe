#include "reflectionhandler.hpp"
#include "objects/bike.hpp"
#include "objects/car.hpp"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include <iostream>
#include <sstream>
#include <string>

static constexpr auto FUNC_TEXT = "func";
static constexpr auto OBJECT_ID_TEXT = "object_id";
static constexpr auto TYPE_TEXT = "type";
static constexpr auto ARGS_TEXT = "args";
static constexpr auto OBJ_ID_TEXT = "obj_id";
static constexpr auto CLIENT_ID_TEXT = "client_id";
static constexpr auto OBJECT_TYPE_TEXT = "object_type";
static constexpr auto PROCEDURE_TEXT = "procedure";
static constexpr auto METHOD_TEXT = "method";
static constexpr auto RESPONSE_TEXT = "response";
static constexpr auto CREATE_TEXT = "create";
static constexpr auto OBJECT_TEXT = "object";
static constexpr auto RETRIEVE_TEXT = "retrieve";

static std::string build_ok_response(const rttr::variant & var,
                                     const RequestMessage & msg);
static std::string build_error_response(const std::string & error);
static std::string build_no_ok_args(std::vector<rttr::argument> & args,
                                    const std::string & func);
static void write_procedure(rapidjson::Writer<rapidjson::StringBuffer> & writer,
                            const RequestMessage & msg);
static void write_object(rapidjson::Writer<rapidjson::StringBuffer> & writer,
                         const RequestMessage & msg);
static void write_return(rapidjson::Writer<rapidjson::StringBuffer> & writer,
                         const rttr::variant & var);
static void check_string_ok(const rapidjson::Document & doc,
                            const std::string & key);
static void check_int_ok(const rapidjson::Document & doc,
                         const std::string & key);
static void check_array_ok(const rapidjson::Document & doc,
                           const std::string & key);
static void check_type_ok(const rapidjson::Document & doc);
static std::string handle_procedure(const RequestMessage & msg);
static std::string handle_create(const RequestMessage & msg,
                                 AbstractRepo & repo);
static std::string handle_object(const RequestMessage & msg,
                                 AbstractRepo & repo);
static std::string handle_retrieve(const RequestMessage & msg,
                                   AbstractRepo & repo);

ReflectionHandler::ReflectionHandler()
{
    init_repos();
    init_handlers();
}

void ReflectionHandler::init_repos()
{
    m_repos[CarRepo::REPO_NAME] = std::make_unique<CarRepo>();
    m_repos[BikeRepo::REPO_NAME] = std::make_unique<BikeRepo>();
}

void ReflectionHandler::init_handlers()
{
    m_handlers[MessageType::CREATE] = handle_create;
    m_handlers[MessageType::OBJECT] = handle_object;
    m_handlers[MessageType::RETRIEVE] = handle_retrieve;
}

std::string ReflectionHandler::handle(const std::string & json)
{
    std::lock_guard<std::mutex> lock{m_mutex};
    std::string response{};
    try
    {
        const RequestMessage msg{json};
        if (msg.type == MessageType::PROCEDURE)
        {
            response = handle_procedure(msg);
        }
        else
        {
            check_repo(msg.object_type);
            check_handler(msg.type);
            AbstractRepo & repo = *m_repos[msg.object_type].get();
            response = m_handlers[msg.type](msg, repo);
        }
    }
    catch (const std::exception & ex)
    {
        response = build_error_response(ex.what());
    }

    return response;
}

void ReflectionHandler::check_repo(const std::string & repo)
{
    if (m_repos.find(repo) == m_repos.end())
    {
        throw std::runtime_error{"ERROR: Object Type " + repo + " not found."};
    }
}

void ReflectionHandler::check_handler(const MessageType & handler)
{
    if (m_handlers.find(handler) == m_handlers.end())
    {
        throw std::runtime_error{"ERROR: Handler Type not found."};
    }
}

static std::string handle_procedure(const RequestMessage & msg)
{
    const auto ret = rttr::type::invoke(msg.func.c_str(), msg.get_args());
    if (ret.is_valid())
    {
        return build_ok_response(ret, msg);
    }
    else
    {
        return build_no_ok_args(msg.get_args(), msg.func);
    }
}

static std::string build_error_response(const std::string & error)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer{buffer};

    writer.StartObject();
    writer.Key(RESPONSE_TEXT);
    writer.String(error.c_str());
    writer.EndObject();

    return std::string{buffer.GetString()};
}

static std::string handle_create(const RequestMessage & msg,
                                 AbstractRepo & repo)
{
    const auto ret = rttr::variant{repo.create_object(msg.obj_id)};
    return build_ok_response(ret, msg);
}

static std::string handle_object(const RequestMessage & msg,
                                 AbstractRepo & repo)
{
    const auto & obj = repo.get_object(msg.obj_id);
    if (&obj == nullptr)
    {
        return build_error_response("ERROR: Object " +
                                    std::to_string(msg.obj_id) + " not found!");
    }
    else
    {
        const rttr::method meth =
            rttr::type::get(obj).get_method(msg.func.c_str());
        const auto ret = meth.invoke_variadic(obj, msg.get_args());
        if (ret.is_valid())
        {
            return build_ok_response(ret, msg);
        }
        else
        {
            return build_no_ok_args(msg.get_args(), msg.func);
        }
    }
}

static std::string build_no_ok_args(std::vector<rttr::argument> & args,
                                    const std::string & func)
{
    if (!args.empty())
    {
        std::stringstream ss;
        for (const auto & piece : args)
            ss << piece.get_value<std::string>() << ", ";
        return build_error_response("ERROR: function " + func + " with args (" +
                                    ss.str() + ") was not found!");
    }
    else
    {
        return build_error_response("ERROR: function " + func +
                                    " was not found!");
    }
}

static std::string handle_retrieve(const RequestMessage & msg,
                                   AbstractRepo & repo)
{
    auto & obj = repo.get_object(msg.obj_id);
    return obj.to_json(msg.obj_id);
}

static std::string build_ok_response(const rttr::variant & var,
                                     const RequestMessage & msg)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer{buffer};
    writer.StartObject();

    if (msg.type == MessageType::PROCEDURE)
    {
        write_procedure(writer, msg);
    }
    else
    {
        write_object(writer, msg);
    }

    write_return(writer, var);

    return std::string{buffer.GetString()};
}

RequestMessage::RequestMessage(const std::string & json)
{
    build_request(json);
}

void RequestMessage::build_request(const std::string & json)
{
    if (json.empty())
        throw std::runtime_error("ERROR: request is empty!");

    rapidjson::Document doc;

    doc.Parse(json.c_str());
    if (doc.HasParseError())
        throw std::runtime_error("ERROR: " + json + " json has parse errors!");

    check_string_ok(doc, TYPE_TEXT);
    check_type_ok(doc);
    type = MessageTypeParser::str_to_enum[doc[TYPE_TEXT].GetString()];

    check_string_ok(doc, OBJECT_TYPE_TEXT);
    object_type = doc[OBJECT_TYPE_TEXT].GetString();

    check_string_ok(doc, FUNC_TEXT);
    func = doc[FUNC_TEXT].GetString();

    check_array_ok(doc, ARGS_TEXT);
    process_array(doc[ARGS_TEXT]);
    const auto & array = doc[ARGS_TEXT];

    check_int_ok(doc, OBJ_ID_TEXT);
    obj_id = doc[OBJ_ID_TEXT].GetInt();

    check_int_ok(doc, CLIENT_ID_TEXT);
    client_id = doc[CLIENT_ID_TEXT].GetInt();
}

std::vector<rttr::argument> RequestMessage::get_args() const
{
    std::vector<rttr::argument> ret;
    std::copy(args.begin(), args.end(), std::back_inserter(ret));
    return ret;
}

void RequestMessage::process_array(const rapidjson::Value & array)
{
    for (rapidjson::SizeType i = 0; i < array.Size(); i++)
    {
        if (array[i].IsInt())
        {
            add_to_args(rttr::variant(array[i].GetInt()));
        }
        else if (array[i].IsString() && array[i].GetStringLength() > 0)
        {
            add_to_args(rttr::variant(std::string{array[i].GetString()}));
        }
        else if (array[i].IsDouble())
        {
            add_to_args(rttr::variant(array[i].GetDouble()));
        }
        else if (array[i].IsFloat())
        {
            add_to_args(rttr::variant(array[i].GetFloat()));
        }
        else if (array[i].IsBool())
        {
            add_to_args(rttr::variant(array[i].GetBool()));
        }
    }
}

void RequestMessage::add_to_args(rttr::variant arg) { args.push_back(arg); }

static void check_string_ok(const rapidjson::Document & doc,
                            const std::string & key)
{
    if (!doc[key.c_str()].IsString())
        throw std::runtime_error{"ERROR: " + key + " is not a json string!"};
}

static void check_int_ok(const rapidjson::Document & doc,
                         const std::string & key)
{
    if (!doc[key.c_str()].IsInt())
        throw std::runtime_error{"ERROR: " + key + " is not a json integer!"};
}

static void check_array_ok(const rapidjson::Document & doc,
                           const std::string & key)
{
    if (!doc[key.c_str()].IsArray())
        throw std::runtime_error{"ERROR: " + key + " is not a json array!"};
}

static void check_type_ok(const rapidjson::Document & doc)
{
    if (MessageTypeParser::str_to_enum.find(doc[TYPE_TEXT].GetString()) ==
        MessageTypeParser::str_to_enum.end())
        throw std::runtime_error{
            "ERROR: " + std::string(doc[TYPE_TEXT].GetString()) +
            " is not an acceptable type"};
}

static void write_procedure(rapidjson::Writer<rapidjson::StringBuffer> & writer,
                            const RequestMessage & msg)
{
    writer.Key(PROCEDURE_TEXT);
    writer.String(msg.func.c_str());
}

static void write_object(rapidjson::Writer<rapidjson::StringBuffer> & writer,
                         const RequestMessage & msg)
{
    writer.Key(OBJECT_ID_TEXT);
    writer.Int(msg.obj_id);
    writer.Key(METHOD_TEXT);
    writer.String(msg.func.c_str());
}

static void write_return(rapidjson::Writer<rapidjson::StringBuffer> & writer,
                         const rttr::variant & var)
{
    writer.Key(RESPONSE_TEXT);
    const rttr::type type = var.get_type();
    if (type == rttr::type::get<std::string>())
        writer.String(var.to_string().c_str());
    else if (type == rttr::type::get<bool>())
        writer.Bool(var.to_bool());
    else if (type == rttr::type::get<char>())
        writer.Bool(var.to_bool());
    else if (type == rttr::type::get<int8_t>())
        writer.Int(var.to_int8());
    else if (type == rttr::type::get<int16_t>())
        writer.Int(var.to_int16());
    else if (type == rttr::type::get<int32_t>())
        writer.Int(var.to_int32());
    else if (type == rttr::type::get<int64_t>())
        writer.Int64(var.to_int64());
    else if (type == rttr::type::get<uint8_t>())
        writer.Uint(var.to_uint8());
    else if (type == rttr::type::get<uint16_t>())
        writer.Uint(var.to_uint16());
    else if (type == rttr::type::get<uint32_t>())
        writer.Uint(var.to_uint32());
    else if (type == rttr::type::get<uint64_t>())
        writer.Uint64(var.to_uint64());
    else if (type == rttr::type::get<float>())
        writer.Double(var.to_double());
    else if (type == rttr::type::get<double>())
        writer.Double(var.to_double());
    writer.EndObject();
}

MessageTypeParser::MessageTypeStrEnum MessageTypeParser::str_to_enum = {
    {PROCEDURE_TEXT, MessageType::PROCEDURE},
    {CREATE_TEXT, MessageType::CREATE},
    {OBJECT_TEXT, MessageType::OBJECT},
    {RETRIEVE_TEXT, MessageType::RETRIEVE}};