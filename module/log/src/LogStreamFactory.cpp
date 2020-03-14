#include "LogStreamFactory.hpp"

using namespace arepa::log;

// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

LogStreamFactory::LogStreamFactory(std::shared_ptr<Adapter> adapter, std::string module)
    : _adapter(std::move(adapter))
    , _module(std::move(module)) {}

LogStreamFactory::LogStreamFactory(std::shared_ptr<Adapter> adapter)
    : _adapter(std::move(adapter)) {}

LogStreamFactory::LogStreamFactory(std::string module) noexcept
    : _module(std::move(module)) {}

LogStreamFactory::LogStreamFactory() noexcept {}
