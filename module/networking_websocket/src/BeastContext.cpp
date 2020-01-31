#include "BeastContext.hpp"

#include "Options.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/ref.hpp>

using namespace arepa::networking::websocket;
using arepa::networking::websocket::Options;
using boost::asio::io_context;
using boost::asio::ip::make_address;
using boost::system::error_code;
using std::shared_ptr;

static void thread_main(io_context* context) {
    context->run();
}


// ---------------------------------------------------------------------------------------------------------------------
#pragma mark - Constructors -
// ---------------------------------------------------------------------------------------------------------------------

BeastContext::BeastContext(const Options& options)
    : io_context(new boost::asio::io_context(options.threads.value_or(Options::DEFAULT_THREAD_COUNT)))
    , io_context_guard(boost::asio::make_work_guard(*this->io_context))
    , endpoint(make_address(options.bind_address.value_or("0.0.0.0")), options.bind_port) {

    // Start the threads.
    auto threadCount = options.threads.value_or(Options::DEFAULT_THREAD_COUNT);
    this->threads.reserve(threadCount);
    for (size_t i = 0; i < threadCount; i++) {
        this->threads.emplace_back(thread_main, this->io_context);
    }
}

BeastContext::~BeastContext() {
    delete this->io_context;
}
