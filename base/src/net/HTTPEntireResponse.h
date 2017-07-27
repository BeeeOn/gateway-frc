#ifndef GATEWAY_HTTP_ENTIRE_RESPONSE_H
#define GATEWAY_HTTP_ENTIRE_RESPONSE_H

#include <string>

#include <Poco/Net/HTTPResponse.h>

namespace BeeeOn {

/*
 * @brief The class is derivated from Poco::Net::HTTPResponse
 * and allows to keep response body.
 */
class HTTPEntireResponse : public Poco::Net::HTTPResponse {
public:
	HTTPEntireResponse();

	/*
	 * @brief Move semantics allows to return HTTPEntireResponse instance
	 * from methods without unnecessary copying of headers or body contents.
	 */
	HTTPEntireResponse(HTTPEntireResponse&& other);
	HTTPEntireResponse(Poco::Net::HTTPResponse::HTTPStatus status);
	HTTPEntireResponse(Poco::Net::HTTPResponse::HTTPStatus status, const std::string& reason);
	HTTPEntireResponse(const std::string& version, Poco::Net::HTTPResponse::HTTPStatus status);
	HTTPEntireResponse(const std::string& version, Poco::Net::HTTPResponse::HTTPStatus status, const std::string& reason);
	~HTTPEntireResponse();

	HTTPEntireResponse& operator=(HTTPEntireResponse&& other);

	void readBody(std::istream& istr);
	void setBody(const std::string& body);
	std::string getBody() const;

private:
	std::string m_body;
};

}

#endif
