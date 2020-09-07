// NOLINT(namespace-envoy)
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../lib/proxy_wasm_sdk/proxy_wasm_intrinsics.h"

class ExpoWorkerRootContext : public RootContext {
   public:
    explicit ExpoWorkerRootContext(uint32_t id, StringView root_id) : RootContext(id, root_id) {}
    bool onConfigure(size_t /* configuration_size */) override;

    bool onStart(size_t) override;
    void onTick() override;

   private:
    std::string service_id_;
    std::string cluster_name_;
    int grpc_timeout_;
};

bool ExpoWorkerRootContext::onConfigure(size_t) {
    LOG_INFO("onConfigure");

    int poll_interval_milliseconds;
    poll_interval_milliseconds = 1000;
    proxy_set_tick_period_milliseconds(poll_interval_milliseconds);
    return true;
}

bool ExpoWorkerRootContext::onStart(size_t) {
    LOG_INFO("onStart");
    return true;
}

void ExpoWorkerRootContext::onTick() {
    LOG_INFO("onTick");
    return;
}

static RegisterContextFactory register_ExpoWorkerRootContext(
    ROOT_FACTORY(ExpoWorkerRootContext), "ExpoWorkerRootId");