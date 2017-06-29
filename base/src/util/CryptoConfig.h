#ifndef BEEEON_CRYPTO_CONFIG_H
#define BEEEON_CRYPTO_CONFIG_H

#include <string>

#include <Poco/Crypto/CipherKey.h>

#include "util/Loggable.h"

namespace BeeeOn {

class CryptoParams;

class CryptoConfig : public Loggable {
public:
	static const std::string DEFAULT_ALGORITHM;

	/**
	 * Construct the config with default parameters:
	 *
	 * - algorithm: "aes256",
	 * - iterationCount: CipherKey::DEFAULT_ITERATION_COUNT
	 */
	CryptoConfig();

	void setAlgorithm(const std::string &name);
	void setPassphrase(const std::string &passphrase);
	void setIterationCount(const int count);

	/**
	 * Create appropriate key based on the given params.
	 * @throws InvalidArgumentException when params are incompatible
	 */
	Poco::Crypto::CipherKey createKey(const CryptoParams &params) const;

	/**
	 * Derive appropriate params based on this configuration.
	 * If the given salt is empty a random salt is generated.
	 */
	CryptoParams deriveParams(const std::string &salt = "") const;

private:
	std::string m_algorithm;
	std::string m_passphrase;
	int m_iterationCount;
};

}

#endif
