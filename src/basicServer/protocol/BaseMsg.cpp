#include "pch.h"
#include "BaseMsg.h"
#include <utils/encodings/CharEncodings.h>

YBaseMsg::YBaseMsg(const YOverlappedBuffer* pBuffer)
{
	WSABUF* pb = pBuffer->getWSABuf();
	memcpy(&head, pb->buf, sizeof(msgHead));

	std::vector<char> vecData(head.length, 0);
	memcpy(&vecData[0], pb->buf + sizeof(msgHead), head.length);

	std::string rawContent(vecData.begin(), vecData.end());
	content = NS_Yutils::MBToWChar(rawContent);
}
