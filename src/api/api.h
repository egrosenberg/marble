#ifndef API_H
#define API_H

#include "../tracklist.h"
#include "httplib.h"
#include "json.hpp"
#include <map>

#define API_FN_ARGS const nlohmann::json &, nlohmann::json &, const api::context &
#define API_FN_ARGS_NAMED const nlohmann::json &options, nlohmann::json &response, const api::context &ctx
#define API_FN(name) void name(API_FN_ARGS);
#define API_FN_DEF(ns, name) void ns::name(API_FN_ARGS_NAMED)
#define API_REQUEST_HANDLE_ARGS const httplib::Request &req, httplib::Response &res, const api::context &ctx
#define POST_HANDLER                                                                                                   \
  inline void POST(API_REQUEST_HANDLE_ARGS) { api::handleRequest(req, res, ctx, postMap); }
#define GET_HANDLER                                                                                                    \
  inline void GET(API_REQUEST_HANDLE_ARGS) { api::handleRequest(req, res, ctx, getMap); }
#define NULL_GET_HANDLER inline api::request = nullptr;
#define BIND_ROUTE(name) inline api::route_handlers name = {&api::name::GET, &api::name::POST};

namespace api {

typedef struct context {
  Tracklist *activeTL;
} context;

// Typedef for api functions
typedef void (*api_fn)(API_FN_ARGS);
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

nlohmann::json getOpts(const httplib::Request &);

} // namespace api

#endif