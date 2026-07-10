
#include "api.h"
#include "httplib.h"

#include "routes/player.h"
#include <sstream>

const std::vector<std::pair<const char *, api::route_handlers>> routes = {{"/player", api::handlers::player}};

void api::bindRoute(httplib::Server &svr, const char *route, const api::context &ctx,
                    const api::route_handlers &registry) {
  if (registry.GET) {
    svr.Get(route,
            [&registry, &ctx](const httplib::Request &req, httplib::Response &res) { registry.GET(req, res, ctx); });
  }
  if (registry.POST) {
    svr.Post(route,
             [&registry, &ctx](const httplib::Request &req, httplib::Response &res) { registry.POST(req, res, ctx); });
  }
}

void api::bindRoutes(httplib::Server &svr, const api::context &ctx) {
  for (const std::pair<const char *, route_handlers> &route : routes) {
    bindRoute(svr, route.first, ctx, route.second);
  }
}

void api::handleRequest(const httplib::Request &req, httplib::Response &res, const api::context &ctx,
                        const api::fn_map &fnMap) {
  std::string routeName = req.get_param_value("name");
  if (!routeName.length()) {
    printf("No api route name provided");
    res.set_content("No name provided", "text/plain");
    res.status = 400;
    return;
  }

  try {
    res.set_header("Access-Control-Allow-Origin", "*");

    nlohmann::json options = api::getOpts(req);
    nlohmann::json data;

    // find api function (if exists)
    api::fn_map::const_iterator function = fnMap.find(routeName);

    if (function != fnMap.end()) {
      // run api function
      (function->second)(options, data, ctx);
      res.set_content(data.dump(2).c_str(), "application/json");
    } else {
      res.status = 404;
      res.set_content("Unable to find route " + routeName, "text/plain");
    }
  } catch (const char *msg) {
    res.status = 400;
    res.set_content(msg, "text/plain");
  }
}

nlohmann::json api::getOpts(const httplib::Request &req) {
  return nlohmann::json::parse(std::stringstream(req.body.length() ? req.body : "{}"));
}