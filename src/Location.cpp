#include "Location.hpp"

static std::string getLocation(std::string url)
{
  size_t lastSlashPos = url.rfind('/');

  if (lastSlashPos == 0)
    return (url);
  else
    return (url.substr(0, lastSlashPos));
}

bool Location::isMatchedLocation(const std::string &url)
{
  std::string urlLocation = getLocation(url);

  return (urlLocation == _location);
}

// check url is in location first...
std::string Location::convertURLToLocationPath(const std::string &url)
{
  if (!isMatchedLocation(url))
  {
    throw (std::runtime_error("invalid url in this location : " + _location + "\n"));
  }
  std::string result;
  std::string filePath = url.substr(url.rfind('/'));
  if (filePath.length() == 1)  // index file case
  {
    filePath += _index;
  }
  result = _root + filePath;
  return (result);
}

bool Location::isCGIRequest(const std::string &file)
{
  for (
          std::vector<std::string>::iterator it = cgiInfo.begin();
          it != cgiInfo.end();
          ++it
      )
  {
    if (file == *it)
      return (true);
  }
  return (false);
}