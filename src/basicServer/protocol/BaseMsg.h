#pragma once
#include <macros\ttype.h>
#include <basicServer\buffer\OverlappedBuffer.h>

struct msgHead {
	unsigned char	version;
	unsigned short	length;
	unsigned char	cmd;
};

//Unicode
class YBaseMsg {
public:
	YBaseMsg(const YOverlappedBuffer* pBuffer);
	virtual ~YBaseMsg() = default;

	unsigned char getVersion() { return head.version; }
	unsigned short getLength() { return head.cmd; }
	tstring getContent() { return content; }

private:
	msgHead head;
	tstring content;
};