#include <fcntl.h>
#include <poll.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <Poco/Exception.h>

#include "io/SerialPort.h"

using namespace BeeeOn;
using namespace Poco;
using namespace std;

static const int BUFFER_SIZE = 1024;

void throwFromErrno(const std::string &func, int err)
{
	switch (errno) {
	case EINTR:
		throw SignalException(func + ": " + strerror(err), err);
	case EINVAL:
		throw InvalidArgumentException(func + ": " + strerror(err), err);
	case EBADF:
	case EFAULT:
	case EPERM:
		throw InvalidAccessException(func + ": " + strerror(err), err);
	case EACCES:
		throw FileAccessDeniedException(func + ": " + strerror(err), err);
	case EIO:
		throw IOException(func + ": " + strerror(err), err);
	case ENODEV:
	case ENOENT:
	case ENXIO:
	case ENAMETOOLONG:
		throw FileNotFoundException(func + ": " + strerror(err), err);
	default:
		throw SystemException(func + ": " + strerror(err), err);
	}
}

static void prepareBaudRate(struct termios &term, int baudrate)
{
	speed_t speed;

	switch (baudrate) {
	case 38400:
		speed = B38400;
		break;
	case 57600:
		speed = B57600;
		break;
	case 115200:
		speed = B115200;
		break;
	default:
		throw InvalidArgumentException(
			"unsupported serial line speed: " + to_string(baudrate));
	}

	if (cfsetspeed(&term, speed) == -1)
		throwFromErrno("cfsetspeed", errno);
}

void prepareStopBits(struct termios &term, SerialPort::StopBits stopBits)
{
	switch (stopBits) {
	case SerialPort::STOPBITS_1:
		term.c_cflag &= ~CSTOPB;
		break;
	case SerialPort::STOPBITS_2:
		term.c_cflag |= CSTOPB;
		break;
	default:
		throw InvalidArgumentException("invalid number of stop bits: " + to_string(stopBits));
	}
}

void prepareParity(struct termios &term, SerialPort::Parity parity)
{
	switch (parity) {
	case SerialPort::PARITY_NONE:
		term.c_iflag = IGNPAR;
		break;
	case SerialPort::PARITY_EVEN:
		term.c_cflag |= PARENB;
		term.c_iflag |= INPCK;
		break;
	case SerialPort::PARITY_ODD:
		term.c_cflag |= PARENB | PARODD;
		term.c_iflag |= INPCK;
		break;
	default:
		throw InvalidArgumentException("invalid parity setting: " + to_string(parity));
	}
}

void prepareDataBits(struct termios &term, SerialPort::DataBits dataBits)
{
	switch (dataBits) {
	case SerialPort::DATABITS_5:
		term.c_cflag |= CS5;
		break;
	case SerialPort::DATABITS_6:
		term.c_cflag |= CS6;
		break;
	case SerialPort::DATABITS_7:
		term.c_cflag |= CS7;
		// Fixed only bits were received.
		term.c_iflag |= ISTRIP;
		break;
	case SerialPort::DATABITS_8:
		term.c_cflag |= CS8;
		break;
	default:
		throw InvalidArgumentException("invalid data bits size: " + to_string(dataBits));
	}
}

SerialPort::SerialPort(const string &devicePath):
	m_fd(-1),
	m_devicePath(devicePath),
	m_baudRate(57600),
	m_stopBits(STOPBITS_1),
	m_parity(PARITY_NONE),
	m_dataBits(DATABITS_8),
	m_nonBlocking(false)
{
}

SerialPort::~SerialPort()
{
	close();
}

void SerialPort::installBlocking()
{
	int flags;

	flags = fcntl(m_fd, F_GETFL, 0);
	if(flags == -1)
		throwFromErrno("fcntl", errno);

	if (!(flags & O_NONBLOCK))
		return;

	// clear the blocking flag
	flags &= ~O_NONBLOCK;
	if (fcntl(m_fd, F_SETFL, flags) == -1)
		throwFromErrno("fcntl", errno);
}

void SerialPort::installNonBlocking()
{
	if (fcntl(m_fd, F_SETFL, O_NONBLOCK) == -1)
		throwFromErrno("fcntl", errno);
}

void SerialPort::open()
{
	if (m_fd >= 0)
		::close(m_fd);

	/**
	 * O_RDWR - Open for reading and writing.
	 * O_NOCTTY - If set and path identifies a terminal device, open() will not
	 *            cause the terminal device to become the controlling terminal
	 *            for the process.
	 * O_NDELAY - When possible, the file is opened in nonblocking mode.
	 */
	m_fd = ::open(m_devicePath.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

	if (m_fd < 0)
		throwFromErrno("open", errno);

	struct termios term;
	::memset(&term, 0, sizeof(term));

	try {
		prepareDataBits(term, m_dataBits);
		prepareParity(term, m_parity);
		prepareStopBits(term, m_stopBits);
		prepareBaudRate(term, m_baudRate);

		if (m_nonBlocking)
			installNonBlocking();
		else
			installBlocking();

		if (tcsetattr(m_fd, TCSAFLUSH, &term) < 0)
			throwFromErrno("tcsetattr", errno);
	}
	catch (const Exception &ex) {
		close();
		ex.rethrow();
	}
}

unsigned int SerialPort::write(const char *buffer, size_t size)
{
	ssize_t ret = ::write(m_fd, buffer, size);

	if (ret < 0) {
		if (m_nonBlocking && errno == EAGAIN)
			return 0;

		if (errno == EIO)
			throw WriteFileException("write: " + string(strerror(errno)), errno);

		throwFromErrno("write", errno);
	}

	return ret;
}

unsigned int SerialPort::write(const string &data)
{
	return write(data.c_str(), data.length());
}

string SerialPort::readDirect(int fd)
{
	char buf[BUFFER_SIZE];

	ssize_t ret = ::read(fd, buf, sizeof(buf));

	if (ret < 0) {
		if (errno == EAGAIN)
			return "";

		if (errno == EIO)
			throw ReadFileException("read: " + string(strerror(errno)), errno);

		throwFromErrno("read", errno);
	}

	return string(buf, ret);
}

string SerialPort::read(const Timespan &timeout = Timespan(0))
{
	if (timeout.totalMilliseconds() == 0)
		return readDirect(m_fd);

	struct pollfd pfd[1];
	pfd[0].fd = m_fd;
	pfd[0].events = POLLIN | POLLERR | POLLRDBAND;
	pfd[0].revents = 0;

	while (true) {
		int ret;
		if ((ret = poll(pfd, 1, timeout.totalMilliseconds())) < 0)
			throwFromErrno("poll", errno);

		if (ret == 0)
			throw TimeoutException(m_devicePath + ": timeout expired");

		if (pfd[0].revents & POLLERR)
			throw IOException("serial port seems to be closed");

		if (pfd[0].revents & (POLLRDBAND | POLLIN))
			return readDirect(m_fd);
	}
}

void SerialPort::close()
{
	if (m_fd < 0)
		::close(m_fd);

	m_fd = -1;
}

void SerialPort::setDevicePath(const string &devicePath)
{
	m_devicePath = devicePath;
}

void SerialPort::setBaudRate(int baudRate)
{
	m_baudRate = baudRate;
}

void SerialPort::setNonBlocking(bool nonBlocking)
{
	m_nonBlocking = nonBlocking;
}

void SerialPort::setStopBits(SerialPort::StopBits stopBits)
{
	m_stopBits = stopBits;
}

void SerialPort::setParity(SerialPort::Parity parity)
{
	m_parity = parity;
}

void SerialPort::setDataBits(SerialPort::DataBits dataBits)
{
	m_dataBits = dataBits;
}

bool SerialPort::isOpen()
{
	return m_fd >= 0;
}

void SerialPort::flush()
{
	if (tcflush(m_fd, TCIFLUSH) == -1 || tcflush(m_fd, TCOFLUSH) == -1)
		throwFromErrno("tcflush", errno);
}
