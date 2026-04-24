#pragma once

#include <vector>

enum class LogSeverity { LOW, MEDIUM, HIGH };
enum class LogType { SYSTEM, INFO };

struct EngineLog
{
    std::vector<std::string> messages;
    LogSeverity severity;
    LogType type;
};

struct EngineLogger
{
    std::vector<EngineLog> logs;
};