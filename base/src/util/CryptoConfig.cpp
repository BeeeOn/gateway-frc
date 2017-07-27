#include <Poco/Exception.h>
#include <Poco/Logger.h>

#include "di/Injectable.h"
#include "util/CryptoConfig.h"
#include "util/CryptoParams.h"

BEEEON_OBJECT_BEGIN(BeeeOn, CryptoConfig)
BEEEON_OBJECT_TEXT("algorithm", &CryptoConfig::setAlgorithm)
BEEEON_OBJECT_TEXT("passphrase", &CryptoConfig::setPassphrase)
BEEEON_OBJECT_NUMBER("interationCount", &CryptoConfig::setIterationCount)
BEEEON_OBJECT_END(BeeeOn, CryptoConfig)

using namespace std;
using namespace Poco;
using namespace Poco::Crypto;
using namespace BeeeOn;

const std::string CryptoConfig::DEFAULT_ALGORITHM = "aes256";

CryptoConfig::CryptoConfig():
	m_algorithm(DEFAULT_ALGORITHM),
	m_iterationCount(CipherKey::DEFAULT_ITERATION_COUNT)
{
}

void CryptoConfig::setAlgorithm(const string &name)
{
	m_algorithm = name;
}

void CryptoConfig::setPassphrase(const string &passphrase)
{
	m_passphrase = passphrase;
}

void CryptoConfig::setIterationCount(const int count)
{
	if (count < 0)
		throw InvalidArgumentException("iteration count must be non-negative");

	if (count < CipherKey::DEFAULT_ITERATION_COUNT) {
		logger().critical(
			"iteration count "
			+ to_string(count)
			+ " is too low for enough security",
			__FILE__, __LINE__
		);
	}

	m_iterationCount = count;
}

/**
 * The number of iterations does not have to match because
 * it is a parameter of crypted data that we are (very probably)
 * going to decrypt after this call.
 */
CipherKey CryptoConfig::createKey(const CryptoParams &params) const
{
	if (params.algorithm() != m_algorithm) {
		throw InvalidArgumentException(
			"inappropriate algorithm "
			+ params.algorithm()
			+ ", required "
			+ m_algorithm
		);
	}

	return params.createKey(m_passphrase);
}

CryptoParams CryptoConfig::deriveParams(const string &salt) const
{
	CryptoParams params = CryptoParams::create(m_algorithm);
	params.setIterationCount(m_iterationCount);
	params.setSalt(salt.empty()? CryptoParams::randomString(32) : salt);

	return params;
}
