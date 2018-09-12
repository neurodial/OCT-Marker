/*
 * Copyright (c) 2018 Kay Gawlik <kaydev@amarunet.de> <kay.gawlik@beuth-hochschule.de> <kay.gawlik@charite.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef Q_DEBUGSTREAM_H
#define Q_DEBUGSTREAM_H

#include <iostream>
#include <streambuf>
#include <string>

#include <QTextEdit>

#include <widgets/dwdebugoutput.h>

class Q_DebugStream : public std::basic_streambuf<char>
{
public:
	Q_DebugStream(std::ostream& stream, DWDebugOutput* debug, const QString& pretext)
	: m_stream(stream)
	, pretext(pretext)
	{
		debugOutputObj = debug;
		m_old_buf      = stream.rdbuf();
		stream.rdbuf(this);

		pretextEmpty = QString(pretext.length(), ' ');
	}

	~Q_DebugStream()
	{
	// output anything that is left
		if(!m_string.empty())
			debugOutputObj->printMessage(m_string.c_str());

		m_stream.rdbuf(m_old_buf);

	}

// 	static void registerQDebugMessageHandler()
// 	{
// 		qInstallMessageHandler(myQDebugMessageHandler);
// 	}

private:

// 	static void myQDebugMessageHandler(QtMsgType type, const QMessageLogContext& , const QString& msg)
// 	{
// 		std::cout << msg.toStdString().c_str();
// 	}

protected:

	//This is called when a std::endl has been inserted into the stream
	virtual int_type overflow(int_type v)
	{
		if(v == '\n')
		{
			debugOutputObj->printMessage(m_string.c_str());
			m_string.erase(m_string.begin(), m_string.end());
		}
		else
			m_string += static_cast<char>(v);

		return v;
	}


	virtual std::streamsize xsputn(const char* p, std::streamsize n)
	{
		m_string.append(p, p + n);

		std::size_t pos = 0;
		while(pos != std::string::npos)
		{
			pos = m_string.find('\n');
			if(pos != std::string::npos)
			{
				std::string tmp(m_string.begin(), m_string.begin() + pos);
				debugOutputObj->printMessage(pretext + ": " + tmp.c_str());
				m_string.erase(m_string.begin(), m_string.begin() + pos + 1);
			}
		}

		return n;
	}

private:
	std::ostream&   m_stream;
	std::streambuf* m_old_buf;
	DWDebugOutput*  debugOutputObj;
	QString         pretext;
	QString         pretextEmpty;
	std::string m_string;
};


#endif // Q_DEBUGSTREAM_H
