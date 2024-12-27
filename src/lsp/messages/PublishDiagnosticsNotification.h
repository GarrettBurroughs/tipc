#pragma once

#include "TextDocument.h"

struct Diagnostic {
  Range range;
  // TODO: Severity
  int severity;
  std::string message;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Diagnostic, range, severity, message);
};

struct PublishDiagnosticsParams {
  std::string uri;
  int version;
  std::vector<Diagnostic> diagnostics;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(PublishDiagnosticsParams, uri, version,
                                 diagnostics)
};

struct PublishDiagnosticsNotification {
  std::string jsonrpc;
  std::string method;
  PublishDiagnosticsParams params;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(PublishDiagnosticsNotification, jsonrpc,
                                 method, params)
};
