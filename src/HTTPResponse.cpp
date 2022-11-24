#include "HTTPResponse.hpp"

/**----------------------
 * * HeaderType         |
 *----------------------*/
std::string HeaderType::GET_DAY(long tm_wday)
{
  switch (tm_wday)
  {
    case 0:
      return ("Sun");
    case 1:
      return ("Mon");
    case 2:
      return ("Tue");
    case 3:
      return ("Wed");
    case 4:
      return ("Thu");
    case 5:
      return ("Fri");
    case 6:
      return ("Sat");
    default:
      return ("null");
  }
}

std::string HeaderType::GET_MON(long tm_wmon)
{
  switch (tm_wmon)
  {
    case 0:
      return ("Jan");
    case 1:
      return ("Feb");
    case 2:
      return ("Mar");
    case 3:
      return ("Apr");
    case 4:
      return ("May");
    case 5:
      return ("Jun");
    case 6:
      return ("Jul");
    case 7:
      return ("Aug");
    case 8:
      return ("Sep");
    case 9:
      return ("Oct");
    case 10:
      return ("Nov");
    case 11:
      return ("Dec");
    default:
      return ("null");
  }
}

std::string HeaderType::getDate()
{
  time_t curTime = time(NULL);          // get current time info
  struct tm* pLocal = gmtime(&curTime); // convert to struct for easy use
  if (pLocal == NULL)
  {
    // ...
    return ("null");
  }
  std::string result;
  result = GET_DAY(pLocal->tm_wday) + ", " + std::to_string(pLocal->tm_mday) + " " + GET_MON(pLocal->tm_mon) + " " + std::to_string(pLocal->tm_year + 1900) +
           " GMT";
  return (result);
}

HeaderType::t_pair HeaderType::CONTENT_LENGTH(const size_t& len)
{
  return (std::pair<std::string, std::string>("Content-Length", std::to_string(len)));
}

HeaderType::t_pair HeaderType::CONTENT_LANGUAGE(const std::string& language)
{
  return (std::pair<std::string, std::string>("Content-Language", language));
}

HeaderType::t_pair HeaderType::CONTENT_TYPE(const std::string& type)
{
  return (std::pair<std::string, std::string>("Content-type", type));
}

HeaderType::t_pair HeaderType::DATE()
{
  return (std::pair<std::string, std::string>("Date", getDate()));
}

HeaderType::t_pair HeaderType::SERVER(const std::string& serverName)
{
  return (std::pair<std::string, std::string>("Server", serverName));
}

HeaderType::t_pair HeaderType::CONNECTION(const std::string& connectionMode)
{
  return (std::pair<std::string, std::string>("Connection", connectionMode));
}

HeaderType::t_pair HeaderType::TRANSFER_ENCODING(const std::string& transfer_encoding_type)
{
  return (std::pair<std::string, std::string>("Transfer-Encoding", transfer_encoding_type));
}

HeaderType::t_pair HeaderType::ALLOW(const std::string& allowedMethod)
{
  return (std::pair<std::string, std::string>("Allow", allowedMethod));
}

HeaderType::t_pair HeaderType::LOCATION(const std::string& redirect_location)
{
  return (std::pair<std::string, std::string>("Location", redirect_location));
}

HeaderType::t_pair HeaderType::SET_COOKIE(const std::string& entity)
{
  return (std::pair<std::string, std::string>("Set-Cookie", entity));
}

/**----------------------
 * * HttpResponseHeader |
 *----------------------*/

HTTPResponseHeader::HTTPResponseHeader()
        :
        _version("HTTP/1.1"), _status_code(-1), _statusMessage("null")
{
  this->addHeader(HTTPResponseHeader::SERVER("null"));
  setDefaultHeaderDescription();
}

HTTPResponseHeader::HTTPResponseHeader(const std::string& version, const int& statusCode, const std::string& statusMessage, const std::string& serverName)
        :
        _version(version),
        _status_code(statusCode),
        _statusMessage(statusMessage)
{
  this->addHeader(HTTPResponseHeader::SERVER(serverName));
  setDefaultHeaderDescription();
}

HTTPResponseHeader& HTTPResponseHeader::operator=(const HTTPResponseHeader& header)
{
  _version = header._version;
  _status_code = header._status_code;
  _statusMessage = header._statusMessage;
  _description = header._description;
  return (*this);
}

void HTTPResponseHeader::addHeader(const std::pair<std::string, std::string>& descriptionPair)
{
  this->_description[descriptionPair.first] = descriptionPair.second;
}

void HTTPResponseHeader::addHeader(const std::string& key, const std::string& value)
{
  this->_description[key] = value;
}

void HTTPResponseHeader::setVersion(const std::string& version)
{
  this->_version = version;
};

void HTTPResponseHeader::setStatus(const int& statusCode, const std::string& statusMessage)
{
  this->_status_code = statusCode;
  this->_statusMessage = statusMessage;
};

std::string HTTPResponseHeader::getVersion() const
{
  return this->_version;
};

int HTTPResponseHeader::getStatusCode() const
{
  return this->_status_code;
};

std::string HTTPResponseHeader::getStatusMessage() const
{
  return this->_statusMessage;
};

const std::map<std::string, std::string>& HTTPResponseHeader::getDescription() const
{
  return this->_description;
}

int HTTPResponseHeader::getContentLength() const
{
  std::string len = this->_description.find("Content-Length")->second;
  return ft_stoi(len);
}

std::string HTTPResponseHeader::toString() const
{
  // (1) if _status_code is out of range, throw error
  if (_status_code < 100 || _status_code > 599)
  {
    throw std::runtime_error("Status Code:" + std::to_string(_status_code) + " -> HttpResponse::toString() : status code is out of range\n");
  }

  // (2) if there is no status messege, throw error
  if (_statusMessage == "null")
  {
    throw std::runtime_error("HttpResponse::toString() : status messege unset\n");
  }

  // (3) if server name unset.
  if (this->_description.find("Server")->second == "null")
  {
    throw std::runtime_error("HttpResponse::toString() : server name unset.\n");
  }

  // TODO: std::to_string은 C++11이라 쓰면 안된다!
  std::string header_message = _version + " " + std::to_string(_status_code) + " " + _statusMessage + "\r\n";
  HTTPResponseHeader::t_iterator itr = _description.find("Tranfer-Encoding");
  itr = _description.begin();
  bool is_chunked = isTransferChunked();
  while (itr != _description.end())
  {
    if (is_chunked && (itr->first == "Content-Length"))
    {
      // ignore Content-Length
      itr++;
      continue;
    }
    header_message += (itr->first + ": " + itr->second + "\r\n");
    itr++;
  }
  return header_message;
}

bool HTTPResponseHeader::isTransferChunked() const
{
  bool is_chunked = false;
  HTTPResponseHeader::t_iterator itr = _description.find("Tranfer-Encoding");
  if (itr != _description.end() && itr->second.find("chunked") != std::string::npos)
  {
    is_chunked = true;
  }
  return (is_chunked);
}

void HTTPResponseHeader::setDefaultHeaderDescription()
{
  this->addHeader(HTTPResponseHeader::DATE());
  this->addHeader(HTTPResponseHeader::CONNECTION("keep-alive"));
  this->addHeader(HTTPResponseHeader::CONTENT_LENGTH(-1));
}

HTTPResponseHeader::HTTPResponseHeader(const HTTPResponseHeader& header)
{
  *this = header;
}

/**----------------------
 * * HttpResponse       |
 *----------------------*/

HTTPResponse::HTTPResponse(const int& statusCode, const std::string& statusMessage, const std::string& serverName)
        :
        HTTPResponseHeader("HTTP/1.1", statusCode, statusMessage, serverName)
{
}

HTTPResponse::~HTTPResponse()
{
}

void HTTPResponse::setFd(const FileDescriptor& fd)
{
  _fd = fd;
}

HTTPResponseHeader HTTPResponse::getHeader() const
{
  return static_cast<HTTPResponseHeader>(*this);
}

FileDescriptor HTTPResponse::getFd() const
{
  return _fd;
}

void HTTPResponse::sendToClient(int socketFD, struct sockaddr_in addr, ServerManager* manager)
{
  /** ------------------
   * * (1) Send Header |
   * ------------------*/

  // *! ---------------------------------------------------------------------------------------
  // *! 문제 : fd로 보내면 Content Lenght 를 어찌 암? 설정이 안되서 들어올텐데?                           |
  // *! (1) CGI일때는 파이프가 들어오니까 길이를 모른다. 이 부분은 chunked로 나눠서 보내거나... 일단 팀 회의 보류. |
  // *! (2) 그 외에는 ContentLegth와 total_read_size를 비교해서, 같아질 경우 close.                    |
  // *! ---------------------------------------------------------------------------------------

  struct ResponseContext* newContext = new struct ResponseContext(socketFD,
                                                                  this->getFd(),
                                                                  this->getHeader().toString(), // 헤더 내용을 버퍼로 전달.
                                                                  addr,
                                                                  socketSendHandler,
                                                                  manager,
                                                                  0, 0); // socketSendHandler는 이거 필요없음.
  // event 세팅
  struct kevent event;
  EV_SET(&event, newContext->fileFD, EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, newContext);

  // kevent에 등록한다.
  if (kevent(manager->getKqueue(), &event, 0, NULL, 0, NULL) < 0)
  {
    printLog("error: " + getClientIP(&addr) + " : event attach failed\n", PRINT_RED);
    throw (std::runtime_error("Event attach failed (response)\n"));
  }

  // *! --------------------------------------------------------------------------
  // *!   WARN : 이렇게 하면 순서가 꼬여서 헤더보다 바디가 먼저 전송되는거 아니야? 한번 체크해보자.  |
  // *! --------------------------------------------------------------------------

  /** ------------------
   * * (1) Send Body   |
   * ------------------*/

  // if body exists, read body and store them to buffer.
  // 만약 body가 있다면, (정상코드이고  Content Length가 있다면) 읽는다.
  if (this->getFd() >= 0 && this->getContentLength() > 0 && this->getStatusCode() != ST_NO_CONTENT)
  {
    // (2-1) kevent에 read할 fd를 등록 (이제 read_fd가 read 가능해지면 bodyFdReadHandler가 호출된다.
    // std::string buffer;

    // ResponseContext 생성.
    struct ResponseContext* newContext = new struct ResponseContext(socketFD,
                                                                    this->getFd(),
                                                                    std::string(""), // hand-over empty buffer
                                                                    addr,
                                                                    bodyFdReadHandler,
                                                                    manager,
                                                                    0,                       // total_rd_size를 0으로 초기화
                                                                    this->getContentLength()); // contentLength 설정.

    // event 세팅
    struct kevent event;
    EV_SET(&event, newContext->fileFD, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, newContext);

    // kevent에 등록한다.
    if (kevent(manager->getKqueue(), &event, 0, NULL, 0, NULL) < 0)
    {
      printLog("error: " + getClientIP(&addr) + " : event attach failed\n", PRINT_RED);
      throw (std::runtime_error("Event attach failed (response)\n"));
    }
  }
}

void HTTPResponse::socketSendHandler(struct ResponseContext* context)
{
  // 이 콜백은 socekt send 가능한 시점에서 호출되기 때문에, 이대로만 사용하면 된다.
  if (send(context->socketFD, context->buffer.c_str(), context->buffer.size(), MSG_DONTWAIT) < 0)
  {
    printLog("error: " + getClientIP(&context->addr) + " : send failed\n", PRINT_RED);
    throw (std::runtime_error("Send Failed\n"));
  }
  else if (context->fileFD < 0) // no body.
  {
    close(context->socketFD); // close socket
  }
  delete (context);
}

void HTTPResponse::bodyFdReadHandler(struct ResponseContext* context)
{
  const int BUF_SIZE = 1024; // TODO:  버퍼의 크기는 추후에 조정할 것.
  char buffer[BUF_SIZE] = {0};

  ssize_t current_rd_size = read(context->fileFD, buffer, sizeof(buffer));
  if (current_rd_size < 0)
  {
    // if read failed
    printLog("error: client: " + getClientIP(&context->addr) + " : read failed\n", PRINT_RED);
    throw (std::runtime_error("Read Failed\n"));
  }
  else // 데이터가 들어왔다면, 소켓에 버퍼에 있는 데이터를 전송하는 socket send event를 등록.
  {
    context->totalReadSize += current_rd_size; // 읽은 길이를 누적.

    // ㅊontent_length와 누적 읽은 길이가 같아지면 file_fd 닫고 file_fd에 -1대입.
    bool is_read_finished = false;
    if (context->totalReadSize >= context->contentLength)
    {
      close(context->fileFD); // fd_file을 닫고.
      context->fileFD = -1;   // socketSendHandler가 file_fd가 -1이면 소켓을 종료.
      is_read_finished = true; // 마지막에 context delete하기 위함.
    }

    // ResponseContext를 만들어서 넘긴다.
    struct kevent event;
    struct ResponseContext* newContext = new struct ResponseContext(context->fileFD,
                                                                    context->socketFD,
                                                                    std::string(buffer),
                                                                    context->addr,
                                                                    socketSendHandler,
                                                                    context->manager,
                                                                    0, 0); // socketSendHandler는 이걸 쓰지 않음.
    // event를 세팅한 후 kevent에 등록.
    EV_SET(&event, newContext->socketFD, EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0, newContext);
    if (kevent(context->manager->getKqueue(), &event, 0, NULL, 0, NULL) < 0)
    {
      printLog("error: " + getClientIP(&context->addr) + " : event attach failed\n", PRINT_RED);
      throw (std::runtime_error("Event attach failed (response)\n"));
    }
    // 만약 다 읽어서 flag가 true가 되면 삭제.
    if (is_read_finished)
    {
      delete (context);
    }
  }
}

std::string HTTPResponse::getClientIP(const struct sockaddr_in* addr)
{
  char str[INET_ADDRSTRLEN];
  const struct sockaddr_in* pV4Addr = addr;
  struct in_addr ipAddr = pV4Addr->sin_addr;
  inet_ntop(AF_INET, &ipAddr, str, INET_ADDRSTRLEN);
  return (str);
}