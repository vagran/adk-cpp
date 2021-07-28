module adk.common.exceptions;

import std.core;
import adk.common.string_utils;


using namespace adk;

constexpr int INDENT_SIZE = 4;


Exception::Exception(const std::string_view &msg):
    msg(msg)
{}

const char*
Exception::what() const noexcept
{
    if (!fullMsg.empty()) {
        return fullMsg.c_str();
    }
    fullMsg = CreateFullMessage();
    return fullMsg.c_str();
}

void
Exception::AddSuppressed(std::exception_ptr e)
{
    suppressed.emplace_back(e);
    fullMsg.clear();
}

std::string
Exception::GetFullMessage(int indent) const
{
    if (!fullMsg.empty() && indent == 0) {
        return fullMsg;
    }
    return CreateFullMessage(indent);
}

std::string
Exception::CreateFullMessage(int indent) const
{
    std::string result;
    result.append(indent * INDENT_SIZE, ' ');
    result += adk::DemangleName(typeid(*this).name());
    if (srcLoc.file_name()) {
        result += " at [";
        result += adk::GetFileBaseName(srcLoc.file_name());
        result += ':';
        result += std::to_string(srcLoc.line());
        result += "]: ";
    } else {
        result += ": ";
    }
    result += GetMessage();

    if (!suppressed.empty()) {
        result += "\n";
        result.append(indent * INDENT_SIZE, ' ');
        result += "Suppressed:\n";
        bool isFirst = true;
        for (std::exception_ptr e: suppressed) {
            if (!isFirst) {
                result += '\n';
            } else {
                isFirst = false;
            }
            result += GetFullMessage(e, indent + 1);
        }
    }

    try {
        std::rethrow_if_nested(*this);
    } catch (Exception &e) {
        result += "\n";
        result.append(indent * INDENT_SIZE, ' ');
        result += "Caused by:\n";
        result += e.CreateFullMessage(indent);
    } catch (std::exception &e) {
        result += "\n";
        result.append(indent * INDENT_SIZE, ' ');
        result += "Caused by:\n";
        result += GetFullMessage(e, indent);
    } catch(...) {
        result += "\n";
        result.append(indent * INDENT_SIZE, ' ');
        result += "Caused by unknown exception type";
    }
    return result;
}

std::string
Exception::GetFullMessage(std::exception_ptr ptr, int indent)
{
    if (!ptr) {
        return std::string(indent * INDENT_SIZE, ' ') + "None";
    }
    try {
        std::rethrow_exception(ptr);
    } catch (Exception &e) {
        return e.GetFullMessage(indent);
    } catch (std::exception &e) {
        return GetFullMessage(e, indent);
    } catch (...) {
        return std::string(indent * INDENT_SIZE, ' ') + "Unknown exception type";
    }
}

std::string
Exception::GetFullMessage(const std::exception &e, int indent)
{
    if (const Exception *ePtr = dynamic_cast<const Exception *>(&e)) {
        return ePtr->CreateFullMessage(indent);
    }
    std::string result;
    if (const std::system_error *ePtr = dynamic_cast<const std::system_error *>(&e)) {
        result.append(indent * INDENT_SIZE, ' ');
        result += "std::system_error: code ";
        result += std::to_string(ePtr->code().value());
        result += " - ";
        result += ePtr->code().message();
        result += "; ";
        result += ePtr->what();
    } else {
        result.append(indent * INDENT_SIZE, ' ');
        result += e.what();
    }

    try {
        std::rethrow_if_nested(e);
    } catch (Exception &eNested) {
        result += "\n";
        result.append(indent * INDENT_SIZE, ' ');
        result += "Caused by:\n";
        result += eNested.CreateFullMessage(indent);
    } catch (std::exception &eNested) {
        result += "\n";
        result.append(indent * INDENT_SIZE, ' ');
        result += "Caused by:\n";
        result += GetFullMessage(eNested, indent);
    } catch(...) {
        result += "\n";
        result.append(indent * INDENT_SIZE, ' ');
        result += "Caused by unknown exception type";
    }
    return result;
}
