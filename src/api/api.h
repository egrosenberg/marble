#ifndef API_H
#define API_H

#include "../tracklist.h"
#include "httplib.h"
#include <map>


namespace api {

typedef struct context {
  Tracklist *activeTL;
} context;

// Typedef for api functions
typedef void (*api_fn)(const httplib::Request &, httplib::Response &, const context &);
// Map of api functions
typedef std::map<std::string, api_fn> fn_map;

// handle-request wrapper function
typedef void (*request_handler)(const httplib::Request &, httplib::Response &, const context &);

// Registry for a specific route
typedef struct route_handlers {
  request_handler GET;
  request_handler POST;
} route_registry;

void bindRoute(httplib::Server &svr, const char *route, const context &, const route_handlers &registry);

void handleRequest(const httplib::Request &req, httplib::Response &res, const context &, const api::fn_map &fnMap);

namespace handlers {}

void bindRoutes(httplib::Server &svr, const context &);

} // namespace api

#endif