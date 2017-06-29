#ifndef BEEEON_CRYPTO_PARAMS_H
#define BEEEON_CRYPTO_PARAMS_H

#include <Poco/Crypto/CipherKey.h>

namespace BeeeOn {

class CryptoParams {
public:
	CryptoParams();

	void setAlgorithm(const std::string &name);
	std::string algorithm() const;

	void setSalt(const std::string &salt);
	std::string salt() const;

	void setIterationCount(const int count);
	int iterationCount() const;

	Poco::Crypto::CipherKey createKey(
			const std::string &passphrase) const;
	Poco::Crypto::CipherKey randomKey(const unsigned int length = 256) const;

	std::string toString() const;
	bool empty() const;

	static CryptoParams parse(const std::string &input);
	static CryptoParams create(const std::string &algorithm);
	static CryptoParams createEmpty();

	/**
	 * Generates a secure random string that can be used as a random
	 * salt or passphrase. Do not reimplemented in other classes, prefer
	 * always this call.
	 */
	static std::string randomString(const unsigned int length);

private:
	int m_iterationCount;
	std::string m_salt;
	std::string m_algorithm;
};

}

#endif
