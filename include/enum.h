#define LOG_ERROR(msg) Log::logger->error(postfix(msg))
#define LOG_WARN(msg) Log::logger->warn(postfix(msg))
#define LOG_INFO(msg) Log::logger->info(postfix(msg))
#define LOG_DEBUG(msg) Log::logger->debug(postfix(msg))

#define TCP_ALIVE_TIMEOUT 600
#define TCP_PACKET_MAX 65536
