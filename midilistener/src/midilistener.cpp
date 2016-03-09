
#include "MidiEndpointCommon.h"
#include "MidiMqttRetransmitter.h"

#include <boost/program_options.hpp>
#include <log4cxx/logger.h>

#include <stdexcept>

static const char *DEFAULT_SERVER = "localhost";
static const unsigned int DEFAULT_PORT = 1883;
static const char *DEFAULT_TOPIC = "midi";
static const char *DEFAULT_CLIENT_NAME = "midilistener";

static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("midilistener"));

bool parseOptions(int argc, char *argv[], std::string &mqttServer,
                  unsigned int &mqttPort, std::string &mqttTopic,
                  std::string &clientName);

int main(int argc, char *argv[])
{
    using namespace midiendpoints;

    std::string mqttServer;
    unsigned int mqttPort;
    std::string clientName;
    std::string mqttTopic;
    if (!parseOptions(argc, argv, mqttServer, mqttPort, mqttTopic, clientName))
    {
        return 0;
    }

    configureLogging();

    try
    {
        MidiMqttRetransmitter retransmitter(mqttServer, mqttPort, mqttTopic,
                                            clientName);
        retransmitter.start();

        // TODO make this right
        LOG4CXX_INFO(logger, "Reading MIDI input, press <enter> to quit...")
        char input;
        std::cin.get(input);

        retransmitter.stop();
    }
    catch (std::exception &e)
    {
        LOG4CXX_ERROR(logger, e.what())
    }

    return 0;
}

bool parseOptions(int argc, char *argv[], std::string &mqttServer,
                  unsigned int &mqttPort, std::string &mqttTopic,
                  std::string &clientName)
{
    namespace po = boost::program_options;

    try
    {

        std::string server;
        unsigned int port;
        std::string topic;
        std::string name;

        // clang-format off
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "show help")
            ("server,s", po::value<std::string>(&server)->default_value(DEFAULT_SERVER), "server address or host name")
            ("port,p", po::value<unsigned int>(&port)->default_value(DEFAULT_PORT), "server port")
            ("topic,t", po::value<std::string>(&topic)->default_value(DEFAULT_TOPIC), "MQTT topic")
            ("name,n", po::value<std::string>(&name)->default_value(DEFAULT_CLIENT_NAME), "MQTT and MIDI client name");
        // clang-format on

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            std::cout << desc << std::endl;
            return false;
        }

        mqttServer = server;
        mqttPort = port;
        mqttTopic = topic;
        clientName = name;

        return true;
    }
    catch (std::exception &e)
    {
        std::cerr << "error: " << e.what() << std::endl;
        return false;
    }
}
