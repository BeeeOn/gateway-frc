#include <Poco/Exception.h>
#include <Poco/RandomStream.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/Object.h>

#include "util/CryptoParams.h"
#include "util/JsonUtil.h"

using namespace std;
using namespace Poco;
using namespace Poco::Crypto;
using namespace Poco::JSON;
using namespace BeeeOn;

CryptoParams::CryptoParams():
	m_iterationCount(0)
{
}

void CryptoParams::setAlgorithm(const string &name)
{
	m_algorithm = name;
}

string CryptoParams::algorithm() const
{
	return m_algorithm;
}

void CryptoParams::setSalt(const string &salt)
{
	m_salt = salt;
}

string CryptoParams::salt() const
{
	return m_salt;
}

void CryptoParams::setIterationCount(const int count)
{
	if (count < 0)
		throw InvalidArgumentException("iteration count must be non-negative");

	m_iterationCount = count;
}

int CryptoParams::iterationCount() const
{
	return m_iterationCount;
}

CipherKey CryptoParams::createKey(
		const string &passphrase) const
{
	return CipherKey(m_algorithm, passphrase, m_salt, m_iterationCount);
}

/**
 * Convert the given byte to a safe printable character. By means 'safe',
 * consider a character the does not introduce common injections into
 * SQL, shell, JSON or XML.
 */
static unsigned char toPrintable(const unsigned char c)
{
	static const string printable =
		"abcdefghijklmnopqrstuvwxyz"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"0123456789@#$%^)-=]:;,";

	const int index = c % printable.size();
	return printable[index];
}

CipherKey CryptoParams::randomKey(const unsigned int length) const
{
	return createKey(randomString(length));
}

string CryptoParams::toString() const
{
	Object::Ptr json(new Object);

	if (!m_algorithm.empty())
		json->set("name", m_algorithm);

	if (!m_salt.empty())
		json->set("salt", m_salt);

	if (m_iterationCount > 0)
		json->set("it", m_iterationCount);

	return Dynamic::Var::toString(json);
}

bool CryptoParams::empty() const
{
	return m_algorithm.empty() && m_salt.empty() && m_iterationCount == 0;
}

CryptoParams CryptoParams::parse(const string &input)
{
	CryptoParams params;
	Object::Ptr json = JsonUtil::parse(input);

	if (json->has("name"))
		params.setAlgorithm(JsonUtil::extract<string>(json, "name"));

	if (json->has("salt"))
		params.setSalt(JsonUtil::extract<string>(json, "salt"));

	if (json->has("it"))
		params.setIterationCount(JsonUtil::extract<int>(json, "it"));

	return params;
}

CryptoParams CryptoParams::create(const std::string &algorithm)
{
	CryptoParams params;
	params.setAlgorithm(algorithm);
	params.setIterationCount(CipherKey::DEFAULT_ITERATION_COUNT);

	return params;
}

CryptoParams CryptoParams::createEmpty()
{
	return CryptoParams();
}

string CryptoParams::randomString(const unsigned int length)
{
	string text(length, '0');
	RandomInputStream random;

	for (size_t i = 0; i < text.size(); ++i) {
		text[i] = toPrintable(
			static_cast<unsigned char>(random.get())
		);
	}

	return text;
}
