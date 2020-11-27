// NOLINT(namespace-envoy)
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../lib/proxy_wasm_sdk/proxy_wasm_intrinsics.h"
#include "util/config_parser.h"

class ExpoFilterRootContext : public RootContext {
 public:
  explicit ExpoFilterRootContext(uint32_t id, StringView root_id) : RootContext(id, root_id) {}

  bool onConfigure(size_t config_size) override;
  bool onStart(size_t) override;
  void onTick() override;
  Config getConfig() { return config_; }

 private:
  struct Config config_;
};

bool ExpoFilterRootContext::onStart(size_t) {
  LOG_INFO("onStart");
  return true;
}

void ExpoFilterRootContext::onTick() {
    LOG_INFO("onTick");
    return;
}

bool ExpoFilterRootContext::onConfigure(size_t config_size) {
  LOG_INFO("onConfigure: " + std::to_string(config_size));
  if (config_size == 0) {
    return true;
  }

  // read configuration string from buffer
  auto configuration_data = getBufferBytes(WasmBufferType::PluginConfiguration, 0, config_size);
  std::string configuration = configuration_data->toString();

  // parse configuration string into Config
  std::string log;
  if (!parseConfig(configuration, &config_, &log)) {
    LOG_ERROR("onConfigure: " + log);
    return false;
  }
  
  int poll_interval_milliseconds;
  poll_interval_milliseconds = 1000;
  proxy_set_tick_period_milliseconds(poll_interval_milliseconds);

  LOG_INFO("onConfigure: " + log);
  return true;
}



class ExpoFilterContext : public Context {
 public:
  explicit ExpoFilterContext(uint32_t id, RootContext* root) : Context(id, root) {}

  void onCreate() override;
  FilterHeadersStatus onRequestHeaders(uint32_t headers, bool end_of_stream) override;
  void onDone() override;
  void onLog() override;
  void onDelete() override;

 private:
  std::string content_type_;
  struct Config config_;
};

void ExpoFilterContext::onCreate() {
  LOG_INFO(std::string("onCreate " + std::to_string(id())));

  // get config from root
  ExpoFilterRootContext* root = dynamic_cast<ExpoFilterRootContext*>(this->root());
  config_ = root->getConfig();
  LOG_INFO("onCreate: config loaded from root context ->" + config_.to_string());
}

FilterHeadersStatus ExpoFilterContext::onRequestHeaders(uint32_t, bool) {
  // get header pairs
  LOG_INFO(std::string("onRequestHeaders ") + std::to_string(id()));
  auto result = getRequestHeaderPairs();
  auto pairs = result->pairs();

  // log all headers
  QueryParams headers;
  LOG_INFO(std::string("headers: ") + std::to_string(pairs.size()));
  for (auto& p : pairs) {
    LOG_INFO(std::string(p.first) + std::string(" -> ") + std::string(p.second));
    headers.emplace(p.first, p.second);
  }
  LOG_INFO("all headers printed");

  // record body content type to context
  content_type_ = getRequestHeader("content-type")->toString();

  return FilterHeadersStatus::Continue;
}

void ExpoFilterContext::onDone() { LOG_INFO(std::string("onDone " + std::to_string(id()))); }

void ExpoFilterContext::onLog() { LOG_INFO(std::string("onLog " + std::to_string(id()))); }

void ExpoFilterContext::onDelete() { LOG_INFO(std::string("onDelete " + std::to_string(id()))); }


static RegisterContextFactory register_ExpoFilterContext(
  CONTEXT_FACTORY(ExpoFilterContext), 
  ROOT_FACTORY(ExpoFilterRootContext), 
  "ExpoFilterRootId");