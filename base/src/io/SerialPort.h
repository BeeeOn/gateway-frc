#ifndef BEEEON_SERIAL_PORT_H
#define BEEEON_SERIAL_PORT_H

#include <string>

#include <Poco/Timespan.h>

namespace BeeeOn {

class SerialPort {
public:
	enum StopBits {
		STOPBITS_1,
		STOPBITS_2,
	};

	enum Parity {
		PARITY_NONE,
		PARITY_EVEN,
		PARITY_ODD,
	};

	enum DataBits {
		DATABITS_5,
		DATABITS_6,
		DATABITS_7,
		DATABITS_8,
	};

	SerialPort(const std::string &devicePath = "");
	~SerialPort();

	void setDevicePath(const std::string &devicePath);
	void setBaudRate(int baudRate);
	void setStopBits(StopBits stopBits);
	void setParity(Parity parity);
	void setDataBits(DataBits dataBits);
	void setNonBlocking(bool nonBlocking);

	void open();
	std::string read(const Poco::Timespan &timeout);
	static std::string readDirect(int fd);
	unsigned int write(const char* buffer, std::size_t size);
	unsigned int write(const std::string &data);
	void close();
	void flush();

	bool isOpen();

private:
	void installNonBlocking();
	void installBlocking();

private:
	int m_fd;

	std::string m_devicePath;
	int m_baudRate;
	StopBits m_stopBits;
	Parity m_parity;
	DataBits m_dataBits;
	bool m_nonBlocking;
};

}

#endif
